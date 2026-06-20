#include "Object/ObjectRepository.h"
#include "Object/Object.h"



// ---------------------------------------------------------------------------
// ObjectRepository Non-Templates
// ---------------------------------------------------------------------------

Object* ObjectRepository::get(ObjectID id)
{
    if (!contains(id)) return nullptr;
    return mSparse[id]->ptr;
}

const Object* ObjectRepository::get(ObjectID id) const
{
    if (!contains(id)) return nullptr;
    return mSparse[id]->ptr;
}

void ObjectRepository::remove(ObjectID id)
{
    // Graceful return if trying to remove an unmapped or already deleted ID
    if (!contains(id)) return;

    auto& slot              = *mSparse[id];
    TypedPool& pool         = *slot.pool;
    auto& back              = mBackMap.at(slot.type);
    const std::size_t removed_dense = slot.dense;

    std::size_t moved_dense = pool.swapAndPop(removed_dense);

    if (removed_dense < pool.size())
    {
        ObjectID moved_id           = back[moved_dense];
        mSparse[moved_id]->dense   = removed_dense;
        mSparse[moved_id]->ptr     = pool.get(removed_dense);
        back[removed_dense]         = moved_id;
    }

    slot.valid = false;
    slot.ptr   = nullptr;
    mSparse[id].reset();
}

bool ObjectRepository::contains(ObjectID id) const
{
    return id < mSparse.size() && mSparse[id] && mSparse[id]->valid;
}

std::size_t ObjectRepository::size() const
{
    std::size_t total = 0;
    for (const auto& [_, pool] : mPools)
        total += pool->size();
    return total;
}

// ---------------------------------------------------------------------------
// iterator
// ---------------------------------------------------------------------------
ObjectRepository::iterator::iterator(PoolMapIt it, PoolMapIt end, const BackMapType* back_map)
    : m_pool_it(it), m_pool_end(end), m_index(0), m_back_map(back_map)
{
    advance_to_valid();
}

void ObjectRepository::iterator::advance_to_valid()
{
    // Skip empty pools (and the just-finished pool once its index runs out)
    // until we land on a valid element or fall off the end entirely.
    while (m_pool_it != m_pool_end && m_index >= m_pool_it->second->size())
    {
        ++m_pool_it;
        m_index = 0;
    }
}

std::pair<ObjectID, Object*> ObjectRepository::iterator::operator*() const
{
    Object* obj = m_pool_it->second->get(m_index);
    ObjectID id = m_back_map->at(m_pool_it->first)[m_index];
    return {id, obj};
}

ObjectRepository::iterator& ObjectRepository::iterator::operator++()
{
    ++m_index;
    advance_to_valid();
    return *this;
}

ObjectRepository::iterator ObjectRepository::iterator::operator++(int)
{
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool ObjectRepository::iterator::operator==(const iterator& other) const
{
    return m_pool_it == other.m_pool_it && m_index == other.m_index;
}

// ---------------------------------------------------------------------------
// const_iterator
// ---------------------------------------------------------------------------
ObjectRepository::const_iterator::const_iterator(PoolMapIt it, PoolMapIt end, const BackMapType* back_map)
    : mPoolIt(it), mPoolEnd(end), mIndex(0), mBackMap(back_map)
{
    advance_to_valid();
}

void ObjectRepository::const_iterator::advance_to_valid()
{
    while (mPoolIt != mPoolEnd && mIndex >= mPoolIt->second->size())
    {
        ++mPoolIt;
        mIndex = 0;
    }
}

std::pair<ObjectID, const Object*> ObjectRepository::const_iterator::operator*() const
{
    // Go through a const TypedPool* explicitly so overload resolution picks
    // TypedPool::get(std::size_t) const, not the mutable overload.
    const TypedPool* pool = mPoolIt->second.get();
    const Object* obj     = pool->get(mIndex);
    ObjectID id           = mBackMap->at(mPoolIt->first)[mIndex];

    return {id, obj};
}

ObjectRepository::const_iterator& ObjectRepository::const_iterator::operator++()
{
    ++mIndex;
    advance_to_valid();
    return *this;
}

ObjectRepository::const_iterator ObjectRepository::const_iterator::operator++(int)
{
    const_iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool ObjectRepository::const_iterator::operator==(const const_iterator& other) const
{
    return mPoolIt == other.mPoolIt && mIndex == other.mIndex;
}

// ---------------------------------------------------------------------------
// begin() / end()
// ---------------------------------------------------------------------------
ObjectRepository::iterator ObjectRepository::begin()
{
    return iterator(mPools.begin(), mPools.end(), &mBackMap);
}

ObjectRepository::iterator ObjectRepository::end()
{
    return iterator(mPools.end(), mPools.end(), &mBackMap);
}

ObjectRepository::const_iterator ObjectRepository::begin() const
{
    return const_iterator(mPools.cbegin(), mPools.cend(), &mBackMap);
}

ObjectRepository::const_iterator ObjectRepository::end() const
{
    return const_iterator(mPools.cend(), mPools.cend(), &mBackMap);
}
