#pragma once

#include "Object/ObjectID.h"
#include <unordered_map>
#include "SceneHierarchyComponent.h"
#include <vector>

// Reserved ID for the virtual root sentinel node (never attached to an Object)
static constexpr ObjectID ROOT_ID = 0;

class SceneHierarchy {
private:

    // ID ROOT_ID (0) is a virtual sentinel whose firstChild is the head of the
    // root-level sibling chain. All real objects have IDs > 0.
    std::unordered_map<ObjectID, SceneHierarchyComponent> mHierarchy;



public:
    SceneHierarchy();
    virtual ~SceneHierarchy() = default;

    /**
     * Add a new object to the scene,
     * Objects are automatically parented to the root
     */
    void addObject(ObjectID id);

    /**
     * remove an object from the scene
     * @param object the object to remove
     * @returns the list of objects that were removed
     */
    [[nodiscard]] std::vector<ObjectID> removeObject(ObjectID id);


    /**
     * Set the parent of an object
     * @param obj the object to set the parent of
     * @param parent the new parent of the object
     */
    void setParent(ObjectID obj, ObjectID parent);

    /**
     * Get the parent of an object
     * @param obj the object to get the parent of, nullptr if parent is root
     * @return
     */
    ObjectID getParent(ObjectID obj) const;

    /**
     * Get the list of children of this object
     * @param obj the object to get the children for
     * @return
     */
    std::vector<ObjectID> getChildren(ObjectID obj) const;

    /**
     * Get the list of siblings of this object
     * @param obj the object to get the siblings for
     * @return
     */
    std::vector<ObjectID> getSiblings(ObjectID obj);

    /**
     * Get the list of children from the root
     * @return
     */
    std::vector<ObjectID> getRootChildren();

private:
    // Internal hierarchy helpers
    void unlinkFromParent(ObjectID id);
    void appendToParent(ObjectID id, ObjectID parentID);
};
