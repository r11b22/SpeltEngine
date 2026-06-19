#include "Object/ObjectRepository.h"

// ---------------------------------------------------------------------------
// ObjectReference
// ---------------------------------------------------------------------------

Object* ObjectReference::get() const
{
    if (auto slot = m_slot.lock())
        if (slot->valid)
            return slot->pool->get(slot->dense);
    return nullptr;
}

bool ObjectReference::is_valid() const
{
    auto slot = m_slot.lock();
    return slot && slot->valid;
}

// ---------------------------------------------------------------------------
// ObjectRepository
// ---------------------------------------------------------------------------

ObjectReference ObjectRepository::make_reference(ObjectID ObjectID)
{
    assert(contains(ObjectID) && "ObjectID not registered");
    return ObjectReference(m_sparse[ObjectID]);
}

Object* ObjectRepository::get(ObjectID ObjectID)
{
    if (!contains(ObjectID)) return nullptr;
    const auto& slot = *m_sparse[ObjectID];
    return slot.pool->get(slot.dense);
}

const Object* ObjectRepository::get(ObjectID ObjectID) const
{
    if (!contains(ObjectID)) return nullptr;
    const auto& slot = *m_sparse[ObjectID];
    return slot.pool->get(slot.dense);
}

void ObjectRepository::remove(ObjectID ObjectID)
{
    assert(contains(ObjectID) && "ObjectID not registered");

    auto& slot              = *m_sparse[ObjectID];
    TypedPool& pool         = *slot.pool;
    auto& back              = m_back_map.at(slot.type);
    const std::size_t removed_dense = slot.dense;

    std::size_t moved_dense = pool.swap_and_pop(removed_dense);

    if (removed_dense < pool.size())
    {
        ObjectID moved_id = back[moved_dense];
        m_sparse[moved_id]->dense     = removed_dense;
        back[removed_dense]           = moved_id;
    }

    slot.valid = false;
    m_sparse[ObjectID].reset();
}

bool ObjectRepository::contains(ObjectID ObjectID) const
{
    return ObjectID < m_sparse.size() && m_sparse[ObjectID] && m_sparse[ObjectID]->valid;
}

std::size_t ObjectRepository::size() const
{
    std::size_t total = 0;
    for (const auto& [_, pool] : m_pools)
        total += pool->size();
    return total;
}
