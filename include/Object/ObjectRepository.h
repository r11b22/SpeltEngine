#pragma once

#include "Object/Object.h"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <cassert>
#include <cstdint>

template<typename T> class ObjectReference;

// ---------------------------------------------------------------------------
// Slot — shared state between ObjectRepository and ObjectReference.
//
// The repository owns one slot per live object (shared_ptr).
// ObjectReferences hold a weak_ptr to the slot.
//
// `ptr` points directly at the live object, so dereferencing a reference is
// a single pointer read — no virtual call through TypedPool. `pool` is kept
// only for the type-erased pool operations that ObjectRepository::remove()
// needs (swap_and_pop); it is never touched on the hot read path anymore.
//
// On swap-and-pop / pool growth the repo updates `ptr` (and `dense`) in
// place; on removal it sets `valid = false` and drops its shared_ptr.
// Either way every existing ObjectReference automatically reflects the
// change on next dereference.
// ---------------------------------------------------------------------------
struct TypedPool; // forward — Slot needs it, TypedPool is defined below

struct Slot
{
    std::type_index  type  { typeid(void) };
    Object*          ptr   { nullptr };
    std::size_t      dense { 0 };
    bool             valid { false };
    TypedPool*       pool  { nullptr };
};

// ---------------------------------------------------------------------------
// TypedPool — type-erased interface for a contiguous array of one Object type.
// Only used by ObjectRepository's own bookkeeping (add/remove); never on the
// ObjectReference read path.
// ---------------------------------------------------------------------------
struct TypedPool
{
    virtual Object*       get(std::size_t i)          = 0;
    virtual const Object* get(std::size_t i) const    = 0;
    virtual std::size_t   emplace()                   = 0;
    virtual std::size_t   swap_and_pop(std::size_t i) = 0; // returns old last index
    virtual std::size_t   size() const                = 0;
    virtual              ~TypedPool()                 = default;
};

// ---------------------------------------------------------------------------
// ConcretePool<T> — objects stored by value in a contiguous std::vector<T>.
// Must stay in the header because it is a template.
// ---------------------------------------------------------------------------
template<typename T>
struct ConcretePool final : TypedPool
{
    static_assert(std::is_base_of_v<Object, T>,
                  "ConcretePool<T>: T must inherit from Object");

    std::vector<T> data;

    Object*       get(std::size_t i)       override { return &data[i]; }
    const Object* get(std::size_t i) const override { return &data[i]; }

    std::size_t emplace() override
    {
        data.emplace_back();
        return data.size() - 1;
    }

    std::size_t swap_and_pop(std::size_t i) override
    {
        const std::size_t last = data.size() - 1;
        if (i != last)
            std::swap(data[i], data[last]);
        data.pop_back();
        return last;
    }

    std::size_t size() const override { return data.size(); }
};

// ---------------------------------------------------------------------------
// ObjectReference<T> — stable, typed handle to a T living in an
// ObjectRepository.
//
// Remains valid (and auto-updates) across swap-and-pop moves and pool
// growth. Returns nullptr / is_valid()==false once the object is removed.
//
// get() uses a static_cast, never a dynamic_cast: the invariant is that a
// non-default ObjectReference<T> only ever exists if T is genuinely correct
// for the object it points to. That's enforced once, at construction time —
// either by ObjectRepository::make_reference<T>() matching the exact
// registered type, or by as<U>() having already verified the relationship
// with a real dynamic_cast — so the frequently-called accessor itself stays
// branch-free and free of RTTI dispatch.
// ---------------------------------------------------------------------------
template<typename T>
class ObjectReference
{
public:
    ObjectReference() = default;

    T*       get()       const;
    T*       operator->() const { return get(); }
    T&       operator*()  const { return *get(); }

    // Attempt to view this reference as a different type. Unlike get(), the
    // relationship between T and U genuinely isn't known at compile time
    // here (U might be a base, a sibling, or unrelated) — this is the one
    // place in the whole repository that legitimately needs dynamic_cast.
    // Returns a default-constructed (invalid) ObjectReference<U> if the
    // underlying object isn't actually a U.
    template<typename U>
    ObjectReference<U> as() const;

    bool is_valid()               const;
    explicit operator bool()      const { return is_valid(); }

private:
    friend class ObjectRepository;
    template<typename> friend class ObjectReference;

    explicit ObjectReference(std::weak_ptr<Slot> slot) : m_slot(std::move(slot)) {}

    std::weak_ptr<Slot> m_slot;
};

template<typename T>
T* ObjectReference<T>::get() const
{
    auto slot = m_slot.lock();
    if (!slot || !slot->valid)
        return nullptr;

    assert((std::is_same_v<T, Object> || slot->type == std::type_index(typeid(T))) &&
           "ObjectReference<T>::get(): T does not match the stored type — "
           "use as<U>() if you genuinely need a runtime-checked cast");

    return static_cast<T*>(slot->ptr);
}

template<typename T>
bool ObjectReference<T>::is_valid() const
{
    auto slot = m_slot.lock();
    return slot && slot->valid;
}

template<typename T>
template<typename U>
ObjectReference<U> ObjectReference<T>::as() const
{
    auto slot = m_slot.lock();
    if (!slot || !slot->valid)
        return ObjectReference<U>();

    if (dynamic_cast<U*>(slot->ptr) == nullptr)
        return ObjectReference<U>();

    return ObjectReference<U>(m_slot);
}

// ---------------------------------------------------------------------------
// ObjectRepository
// ---------------------------------------------------------------------------
class ObjectRepository
{
public:
    using ObjectID = uint32_t;

    // -----------------------------------------------------------------------
    // Add a default-constructed T under id.
    // Returns a raw T* for immediate initialisation.
    // For a long-lived stable handle call make_reference<T>(id) afterwards.
    // Template: must stay in header.
    // -----------------------------------------------------------------------
    template<typename T>
    T* add(ObjectID id)
    {
        static_assert(std::is_base_of_v<Object, T>,
                      "ObjectRepository::add<T>: T must inherit from Object");
        assert(!contains(id) && "ObjectID already registered");

        auto& pool = get_or_create_pool<T>();
        const std::size_t old_capacity = pool.data.capacity();

        pool.data.emplace_back();
        const std::size_t dense = pool.data.size() - 1;

        if (id >= m_sparse.size())
            m_sparse.resize(id + 1);

        auto slot   = std::make_shared<Slot>();
        slot->type  = std::type_index(typeid(T));
        slot->dense = dense;
        slot->valid = true;
        slot->pool  = &pool;
        m_sparse[id] = slot;

        auto& back = m_back_map[std::type_index(typeid(T))];
        if (dense >= back.size())
            back.resize(dense + 1);
        back[dense] = id;

        if (pool.data.capacity() != old_capacity)
        {
            // The vector reallocated, so every existing pointer into this
            // pool is now stale. Refresh them all directly off pool.data —
            // we already have the concrete pool right here, so this is
            // plain array indexing, no virtual dispatch involved.
            for (std::size_t i = 0; i < back.size(); ++i)
            {
                const ObjectID owner = back[i];
                if (owner < m_sparse.size() && m_sparse[owner])
                    m_sparse[owner]->ptr = &pool.data[i];
            }
        }
        else
        {
            slot->ptr = &pool.data[dense];
        }

        return &pool.data[dense];
    }

    // Typed convenience getter — template, must stay in header.
    template<typename T>
    T* get_as(ObjectID id)
    {
        if (!contains(id))
            return nullptr;

        Slot& slot = *m_sparse[id];
        assert((std::is_same_v<T, Object> || slot.type == std::type_index(typeid(T))) &&
               "ObjectRepository::get_as<T>(): T does not match the stored type");

        return static_cast<T*>(slot.ptr);
    }

    // Template: must stay in header.
    template<typename T>
    ObjectReference<T> make_reference(ObjectID id)
    {
        assert(contains(id) && "ObjectID not registered");
        assert((std::is_same_v<T, Object> ||
                m_sparse[id]->type == std::type_index(typeid(T))) &&
               "ObjectRepository::make_reference<T>(): T does not match the stored type");
        return ObjectReference<T>(m_sparse[id]);
    }

    Object*       get(ObjectID id);
    const Object* get(ObjectID id) const;

    void        remove(ObjectID id);
    bool        contains(ObjectID id) const;
    std::size_t size()          const;

private:
    std::vector<std::shared_ptr<Slot>>                              m_sparse;
    std::unordered_map<std::type_index, std::unique_ptr<TypedPool>> m_pools;
    std::unordered_map<std::type_index, std::vector<ObjectID>>      m_back_map;

    // Template: must stay in header.
    template<typename T>
    ConcretePool<T>& get_or_create_pool()
    {
        auto key = std::type_index(typeid(T));
        if (m_pools.find(key) == m_pools.end())
        {
            m_pools[key]    = std::make_unique<ConcretePool<T>>();
            m_back_map[key] = {};
        }
        return static_cast<ConcretePool<T>&>(*m_pools[key]);
    }
};
