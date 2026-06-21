#pragma once

#include "Object/Object.h"
#include "Object/ObjectID.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <utility>

template<typename T> class ObjectReference;

struct TypedPool;

struct Slot
{
    std::type_index  type  { typeid(void) };
    Object* ptr   { nullptr };
    std::size_t      dense { 0 };
    TypedPool*       pool = nullptr;
    bool             valid { false };
};

struct TypedPool
{
    virtual Object* get(std::size_t i)          = 0;
    virtual const Object* get(std::size_t i) const    = 0;
    virtual std::size_t   swapAndPop(std::size_t i) = 0;
    virtual std::size_t   size() const                = 0;
    virtual              ~TypedPool()                 = default;
};

template<typename T>
struct ConcretePool final : TypedPool
{
    static_assert(std::is_base_of_v<Object, T>,
                  "ConcretePool<T>: T must inherit from Object");

    std::vector<T> data;

    Object* get(std::size_t i)       override { return &data[i]; }
    const Object* get(std::size_t i) const override { return &data[i]; }


    std::size_t swapAndPop(std::size_t i) override
    {
        const std::size_t last = data.size() - 1;
        if (i != last)
            std::swap(data[i], data[last]);
        data.pop_back();
        return last;
    }

    std::size_t size() const override { return data.size(); }
};

class IObjectReference
{
public:
    virtual ~IObjectReference() = default;
    virtual Object* getUntyped() const = 0;
};

// ---------------------------------------------------------------------------
// ObjectReference<T>
// ---------------------------------------------------------------------------
template<typename T>
class ObjectReference : public IObjectReference
{
public:
    // Default constructor naturally represents the "NoReference" state
    ObjectReference() = default;

    // Explicit named state factory for readability
    static ObjectReference<T> noReference() { return ObjectReference<T>(); }

    Object* getUntyped() const override;

    T* get()       const;
    T* operator->() const { return get(); }

    // Warning: Dereferencing a NoReference will return a reference to nullptr.
    // Ensure you check isValid() or operator bool() first!
    T& operator*()  const { return *get(); }

    template<typename U>
    ObjectReference<U> as() const;

    bool isValid()               const;
    explicit operator bool()      const { return isValid(); }

    // Check explicitly for NoReference state
    bool isNoReference()         const { return !isValid(); }


    bool operator==(const ObjectReference<T>& other) const {
        return !mSlot.owner_before(other.mSlot) && !other.mSlot.owner_before(mSlot);
    }

    bool operator!=(const ObjectReference<T>& other) const {
        return !(*this == other);
    }

    // Compare with a reference of a DIFFERENT type (e.g., Base vs Derived)
    template<typename U>
    bool operator==(const ObjectReference<U>& other) const {
        return !mSlot.owner_before(other.mSlot) && !other.mSlot.owner_before(mSlot);
    }

    template<typename U>
    bool operator!=(const ObjectReference<U>& other) const {
        return !(*this == other);
    }
private:
    friend class ObjectRepository;
    template<typename> friend class ObjectReference;

    void updatePointer() const;

    explicit ObjectReference(std::weak_ptr<Slot> slot) : mSlot(std::move(slot)) {
        updatePointer();
    }

    std::weak_ptr<Slot> mSlot;
    mutable Object* mInternal = nullptr;
    mutable T* mAccessPointer = nullptr;
};

template <typename T>
void ObjectReference<T>::updatePointer() const{
    auto slot = mSlot.lock();
    if (!slot || !slot->valid)
    {
        mInternal = nullptr;
        mAccessPointer = nullptr;
        return;
    }

    if (slot->ptr != mInternal){
        mInternal = slot->ptr;
        if constexpr (std::is_base_of_v<Object, T>) {
            // EXACT MATCH PATH: Only compile static_cast if T is exactly Object.
            // This avoids compiler errors with virtual base classes.
            if constexpr (std::is_same_v<T, Object>) {
                mAccessPointer = static_cast<T*>(slot->ptr);
            }
            else {
                // HIERARCHY PATH: Handles complex multiple / virtual inheritance setups safely
                mAccessPointer = dynamic_cast<T*>(slot->ptr);
            }
        }
        else {
            // INDEPENDENT INTERFACE PATH
            mAccessPointer = dynamic_cast<T*>(slot->ptr);
        }
    }

}

template<typename T>
Object* ObjectReference<T>::getUntyped() const{
    updatePointer();
    return mInternal;
}

template<typename T>
T* ObjectReference<T>::get() const
{
    updatePointer();
    return mAccessPointer;
}

template<typename T>
bool ObjectReference<T>::isValid() const
{
    auto slot = mSlot.lock();
    return slot && slot->valid;
}

template<typename T>
template<typename U>
ObjectReference<U> ObjectReference<T>::as() const
{
    auto slot = mSlot.lock();
    if (!slot || !slot->valid)
        return ObjectReference<U>::noReference();

    // Use our updated logic: if the target type U cannot be cast from this object's
    // underlying pointer, get() will return nullptr, validating the conversion.
    ObjectReference<U> targetRef(mSlot);
    if (targetRef.get() == nullptr) {
        return ObjectReference<U>::noReference();
    }

    return targetRef;
}


// ---------------------------------------------------------------------------
// ObjectView<T>
// ---------------------------------------------------------------------------
// A lightweight, non-owning view over a contiguous run of T objects.
// Returned by ObjectRepository::getObjectsOfType<T>(). Construction is just
// a pointer + size taken directly from a ConcretePool<T>'s backing vector,
// so there is no per-object dynamic_cast and no allocation involved.
//
// Caveat: like raw pointers into the pool, a view is only valid until the
// next ObjectRepository::add<T>() (which may reallocate that pool's vector)
// or remove() of an object of type T (which may move elements around via
// swap-and-pop). Don't hold on to it across mutations.
template<typename T>
class ObjectView
{
public:
    ObjectView() = default;
    ObjectView(T* data, std::size_t size) : mData(data), mSize(size) {}

    T* begin() const { return mData; }
    T* end()   const { return mData + mSize; }

    std::size_t size()  const { return mSize; }
    bool        empty() const { return mSize == 0; }

    T& operator[](std::size_t i) const { return mData[i]; }

private:
    T*          mData = nullptr;
    std::size_t mSize = 0;
};

// ---------------------------------------------------------------------------
// ObjectRepository
// ---------------------------------------------------------------------------
class ObjectRepository
{
public:
    template<typename T, typename... Args>
    ObjectID add(Args&&... args)
    {
        static_assert(std::is_base_of_v<Object, T>,
                      "ObjectRepository::add<T>: T must inherit from Object");

        auto& pool = getOrCreatePool<T>();
        const std::size_t old_capacity = pool.data.capacity();

        pool.data.emplace_back(std::forward<Args>(args)...);
        const std::size_t dense = pool.data.size() - 1;

        const ObjectID id = pool.data[dense].getID();

        // Return an invalid ID or handle gracefully if already registered
        if (contains(id)) return id;

        if (id >= mSparse.size())
            mSparse.resize(id + 1);

        auto slot   = std::make_shared<Slot>();
        slot->type  = std::type_index(typeid(T));
        slot->dense = dense;
        slot->valid = true;
        slot->pool  = &pool;
        mSparse[id] = slot;

        auto& back = mBackMap[std::type_index(typeid(T))];
        if (dense >= back.size())
            back.resize(dense + 1);
        back[dense] = id;

        if (pool.data.capacity() != old_capacity)
        {
            for (std::size_t i = 0; i < back.size(); ++i)
            {
                const ObjectID owner = back[i];
                if (owner < mSparse.size() && mSparse[owner])
                    mSparse[owner]->ptr = &pool.data[i];
            }
        }
        else
        {
            slot->ptr = &pool.data[dense];
        }

        return id;
    }

    template <typename T>
    bool isOfType(ObjectID id) const{
        if (!contains(id)) return false;
        Slot& slot = *mSparse[id];
        return std::is_same_v<T, Object> || slot.type == std::type_index(typeid(T));
    }

    // Removed assert: safely returns nullptr if mismatch or not found
    template<typename T>
    T* getAs(ObjectID id)
    {
        if (!contains(id))
            return nullptr;

        Slot& slot = *mSparse[id];
        if (!(std::is_same_v<T, Object> || slot.type == std::type_index(typeid(T))))
            return nullptr;

        return static_cast<T*>(slot.ptr);
    }

    template<typename T>
    ObjectReference<T> makeReference(ObjectID id) const
    {
        if (!contains(id))
            return ObjectReference<T>::noReference();

        if constexpr (std::is_base_of_v<Object, T>) {
            if (!(std::is_same_v<T, Object> || mSparse[id]->type == std::type_index(typeid(T)))) {
                // Double check via dynamic_cast if it's a valid inheritance hierarchy up-cast/down-cast
                if (dynamic_cast<T*>(mSparse[id]->ptr) == nullptr) {
                    return ObjectReference<T>::noReference();
                }
            }
        } else {
            // It's a stand-alone interface. Verify at runtime that the concrete object actually implements it.
            if (dynamic_cast<T*>(mSparse[id]->ptr) == nullptr) {
                return ObjectReference<T>::noReference();
            }
        }

        return ObjectReference<T>(mSparse[id]);
    }

    // Returns a view over every object whose EXACT (concrete) type is T -
    // inheritance is not considered, so an object of a type derived from T
    // will not show up here (use the regular begin()/end() iteration plus
    // isOfType<T>/dynamic_cast if you need that instead).
    //
    // This is O(1) plus the size of the result: the pool for T is keyed by
    // std::type_index(typeid(T)) and is guaranteed to be a ConcretePool<T>
    // whenever it exists (it's the only thing getOrCreatePool<T>() ever
    // stores under that key), so the cast back from TypedPool* is a plain,
    // provably-safe static_cast - no dynamic_cast, no per-object work, no
    // allocation. The view just points straight at the pool's contiguous
    // std::vector<T>.
    template<typename T>
    ObjectView<T> getObjectsOfType()
    {
        static_assert(std::is_base_of_v<Object, T>,
                      "ObjectRepository::getObjectsOfType<T>: T must inherit from Object");

        auto it = mPools.find(std::type_index(typeid(T)));
        if (it == mPools.end())
            return ObjectView<T>();

        auto* pool = static_cast<ConcretePool<T>*>(it->second.get());
        return ObjectView<T>(pool->data.data(), pool->data.size());
    }

    template<typename T>
    ObjectView<const T> getObjectsOfType() const
    {
        static_assert(std::is_base_of_v<Object, T>,
                      "ObjectRepository::getObjectsOfType<T>: T must inherit from Object");

        auto it = mPools.find(std::type_index(typeid(T)));
        if (it == mPools.end())
            return ObjectView<const T>();

        auto* pool = static_cast<const ConcretePool<T>*>(it->second.get());
        return ObjectView<const T>(pool->data.data(), pool->data.size());
    }

    Object* get(ObjectID id);
    const Object* get(ObjectID id) const;

    void        remove(ObjectID id);
    bool        contains(ObjectID id) const;
    std::size_t size()          const;

    // Iteration types remain unchanged...
    class iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = std::pair<ObjectID, Object*>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = value_type;

        iterator() = default;

        value_type operator*() const;
        iterator&  operator++();
        iterator   operator++(int);

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        friend class ObjectRepository;

        using PoolMapIt   = std::unordered_map<std::type_index, std::unique_ptr<TypedPool>>::iterator;
        using BackMapType = std::unordered_map<std::type_index, std::vector<ObjectID>>;

        iterator(PoolMapIt it, PoolMapIt end, const BackMapType* back_map);

        void advance_to_valid();

        PoolMapIt          m_pool_it{};
        PoolMapIt          m_pool_end{};
        std::size_t        m_index{0};
        const BackMapType* m_back_map{nullptr};
    };

    class const_iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = std::pair<ObjectID, const Object*>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = value_type;

        const_iterator() = default;

        value_type      operator*() const;
        const_iterator& operator++();
        const_iterator  operator++(int);

        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const { return !(*this == other); }

    private:
        friend class ObjectRepository;

        using PoolMapIt   = std::unordered_map<std::type_index, std::unique_ptr<TypedPool>>::const_iterator;
        using BackMapType = std::unordered_map<std::type_index, std::vector<ObjectID>>;

        const_iterator(PoolMapIt it, PoolMapIt end, const BackMapType* back_map);

        void advance_to_valid();

        PoolMapIt          mPoolIt{};

        PoolMapIt          mPoolEnd{};
        std::size_t        mIndex{0};
        const BackMapType* mBackMap{nullptr};
    };

    iterator       begin();
    iterator       end();
    const_iterator begin()  const;
    const_iterator end()    const;
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

private:
    std::vector<std::shared_ptr<Slot>>                              mSparse;
    std::unordered_map<std::type_index, std::unique_ptr<TypedPool>> mPools;
    std::unordered_map<std::type_index, std::vector<ObjectID>>      mBackMap;

    template<typename T>
    ConcretePool<T>& getOrCreatePool()
    {
        auto key = std::type_index(typeid(T));
        if (mPools.find(key) == mPools.end())
        {
            mPools[key]    = std::make_unique<ConcretePool<T>>();
            mBackMap[key] = {};
        }
        return static_cast<ConcretePool<T>&>(*mPools[key]);
    }
};
