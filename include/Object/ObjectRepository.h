#pragma once

#include "Object/Object.h"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <cassert>
#include <cstdint>

class ObjectReference;

// ---------------------------------------------------------------------------
// Slot — shared state between ObjectRepository and ObjectReference.
//
// The repository owns one slot per live object (shared_ptr).
// ObjectReferences hold a weak_ptr to the slot.
// On swap-and-pop the repo mutates slot->dense in place; on removal it sets
// slot->valid = false and drops its shared_ptr. Either way every existing
// ObjectReference automatically reflects the change on next dereference.
// ---------------------------------------------------------------------------
struct TypedPool; // forward — Slot needs it, TypedPool is defined below

struct Slot
{
    std::type_index  type  { typeid(void) };
    std::size_t      dense { 0 };
    bool             valid { false };
    TypedPool*       pool  { nullptr };
};

// ---------------------------------------------------------------------------
// TypedPool — type-erased interface for a contiguous array of one Object type.
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
// ObjectReference — stable handle to an object in an ObjectRepository.
//
// Remains valid (and auto-updates) across swap-and-pop moves.
// Returns nullptr / is_valid()==false once the object is removed.
// ---------------------------------------------------------------------------
class ObjectReference
{
public:
    ObjectReference() = default;

    Object*       get()       const;
    Object*       operator->() const { return get(); }
    Object&       operator*()  const { return *get(); }

    template<typename T>
    T* get_as() const { return dynamic_cast<T*>(get()); }

    bool is_valid()               const;
    explicit operator bool()      const { return is_valid(); }

private:
    friend class ObjectRepository;
    explicit ObjectReference(std::weak_ptr<Slot> slot) : m_slot(std::move(slot)) {}

    std::weak_ptr<Slot> m_slot;
};

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
    // For a long-lived stable handle call make_reference(id) afterwards.
    // Template: must stay in header.
    // -----------------------------------------------------------------------
    template<typename T>
    T* add(ObjectID id)
    {
        static_assert(std::is_base_of_v<Object, T>,
                      "ObjectRepository::add<T>: T must inherit from Object");
        assert(!contains(id) && "ObjectID already registered");

        auto& pool        = get_or_create_pool<T>();
        std::size_t dense = pool.emplace();

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

        return static_cast<T*>(pool.get(dense));
    }

    // Typed convenience getter — template, must stay in header.
    template<typename T>
    T* get_as(ObjectID id) { return dynamic_cast<T*>(get(id)); }

    // Typed convenience on ObjectReference — template, must stay in header.
    template<typename T>
    T* get_as(const ObjectReference& ref) { return ref.get_as<T>(); }

    ObjectReference make_reference(ObjectID id);

    Object*       get(ObjectID id);
    const Object* get(ObjectID id) const;

    void        remove(ObjectID id);
    bool        contains(ObjectID id) const;
    std::size_t size()          const;

private:
    std::vector<std::shared_ptr<Slot>>                              m_sparse;
    std::unordered_map<std::type_index, std::unique_ptr<TypedPool>> m_pools;
    std::unordered_map<std::type_index, std::vector<ObjectID>>            m_back_map;

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
