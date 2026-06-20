#include "Object/ObjectRepository.h"

// ---------------------------------------------------------------------------
// ObjectRepository
//
// Note: ObjectReference<T>'s methods and make_reference<T>() now live in the
// header — they're templates, so they have to. What's left here is the
// type-erased, non-template surface.
// ---------------------------------------------------------------------------

Object* ObjectRepository::get(ObjectID id)
{
    if (!contains(id)) return nullptr;
    return m_sparse[id]->ptr;
}

const Object* ObjectRepository::get(ObjectID id) const
{
    if (!contains(id)) return nullptr;
    return m_sparse[id]->ptr;
}

void ObjectRepository::remove(ObjectID id)
{
    assert(contains(id) && "ObjectID not registered");

    auto& slot              = *m_sparse[id];
    TypedPool& pool         = *slot.pool;
    auto& back              = m_back_map.at(slot.type);
    const std::size_t removed_dense = slot.dense;

    std::size_t moved_dense = pool.swap_and_pop(removed_dense);

    if (removed_dense < pool.size())
    {
        ObjectID moved_id           = back[moved_dense];
        m_sparse[moved_id]->dense   = removed_dense;
        // Only the moved element's address changed. One virtual call here
        // is fine — removal is far rarer than reference dereferencing,
        // which is exactly the path we optimised against TypedPool calls.
        m_sparse[moved_id]->ptr     = pool.get(removed_dense);
        back[removed_dense]         = moved_id;
    }

    slot.valid = false;
    slot.ptr   = nullptr;
    m_sparse[id].reset();
}

bool ObjectRepository::contains(ObjectID id) const
{
    return id < m_sparse.size() && m_sparse[id] && m_sparse[id]->valid;
}

std::size_t ObjectRepository::size() const
{
    std::size_t total = 0;
    for (const auto& [_, pool] : m_pools)
        total += pool->size();
    return total;
}
