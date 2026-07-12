
#include "Scene/SceneHierarchy.h"
#include "Object/ObjectID.h"
#include <algorithm>
#include <iostream>
#include <vector>


namespace Spelt {
    // ---------------------------------------------------------------------------
    // Constructor — seed the sentinel root node
    // ---------------------------------------------------------------------------

    SceneHierarchy::SceneHierarchy() {
        // ROOT_ID (0) is never attached to an Object; it acts purely as the
        // virtual parent of all top-level objects. Its firstChild field is the
        // head of the root-level sibling chain.
        mHierarchy.emplace(ROOT_ID, SceneHierarchyComponent{});
    }



    // ---------------------------------------------------------------------------
    // Object management
    // ---------------------------------------------------------------------------

    void SceneHierarchy::addObject(ObjectID id) {
        // Register a fresh hierarchy node and attach at root level
        mHierarchy.emplace(id, SceneHierarchyComponent{});
        appendToParent(id, ROOT_ID);

    }

    std::vector<ObjectID> SceneHierarchy::removeObject(ObjectID id) {
        if (!mHierarchy.count(id)) return {};

        std::vector<ObjectID> removedObjects{};
        // Recursively destroy all children first (depth-first, post-order)
        std::vector<ObjectID> childIDs;
        ObjectID cur = mHierarchy.at(id).firstChild;
        while (cur != 0) {
            childIDs.push_back(cur);
            cur = mHierarchy.at(cur).nextSibling;
        }
        for (ObjectID childID : childIDs) {
            auto removed = removeObject(childID);
            removedObjects.insert(removedObjects.end(), removed.begin(), removed.end());
        }
        removedObjects.insert(removedObjects.end(), childIDs.begin(), childIDs.end());
        removedObjects.push_back(id);

        unlinkFromParent(id);
        mHierarchy.erase(id);

        return std::move(removedObjects);
    }

    // ---------------------------------------------------------------------------
    // Internal hierarchy helpers
    // ---------------------------------------------------------------------------

    // Detach `id` from its current parent's sibling chain.
    // The node's own firstChild pointer (its subtree) is left completely untouched.
    void SceneHierarchy::unlinkFromParent(ObjectID id) {
        SceneHierarchyComponent& node = mHierarchy.at(id);
        ObjectID prev = node.prevSibling;
        ObjectID next = node.nextSibling;

        if (prev != 0) {
            mHierarchy.at(prev).nextSibling = next;
        } else {
            // This node was the firstChild of its parent (including the sentinel)
            mHierarchy.at(node.parent).firstChild = next;
        }

        if (next != 0) {
            mHierarchy.at(next).prevSibling = prev;
        }

        node.parent      = ROOT_ID;
        node.prevSibling = 0;
        node.nextSibling = 0;
    }

    // Append `id` as the last child of `parentID`.
    // `parentID` may be ROOT_ID for top-level objects.
    void SceneHierarchy::appendToParent(ObjectID id, ObjectID parentID) {
        SceneHierarchyComponent& parent = mHierarchy.at(parentID);
        SceneHierarchyComponent& node   = mHierarchy.at(id);

        node.parent = parentID;

        if (parent.firstChild == 0) {
            // Parent has no children yet — become the first
            parent.firstChild = id;
            node.prevSibling  = 0;
            node.nextSibling  = 0;
        } else {
            // Walk to the last existing sibling and link after it
            ObjectID last = parent.firstChild;
            while (mHierarchy.at(last).nextSibling != 0)
                last = mHierarchy.at(last).nextSibling;

            mHierarchy.at(last).nextSibling = id;
            node.prevSibling = last;
            node.nextSibling = 0;
        }
    }

    // ---------------------------------------------------------------------------
    // Public hierarchy API
    // ---------------------------------------------------------------------------

    void SceneHierarchy::setParent(ObjectID obj, ObjectID parent) {

        if (!mHierarchy.count(obj))    return;
        if (!mHierarchy.count(parent)) return;

        unlinkFromParent(obj);
        appendToParent(obj, parent);
    }

    ObjectID SceneHierarchy::getParent(ObjectID obj) const{
        if (!mHierarchy.count(obj)) return ROOT_ID;
        ObjectID parentID = mHierarchy.at(obj).parent;
        return parentID;
    }

    std::vector<ObjectID> SceneHierarchy::getChildren(ObjectID obj) const{
        std::vector<ObjectID> result;
        if (!mHierarchy.count(obj)) return result;

        ObjectID cur = mHierarchy.at(obj).firstChild;
        while (cur != 0) {
            result.push_back(cur);
            cur = mHierarchy.at(cur).nextSibling;
        }
        return result;
    }

    std::vector<ObjectID> SceneHierarchy::getSiblings(ObjectID obj) {
        std::vector<ObjectID> result;
        if (!mHierarchy.count(obj)) return result;

        // Walk the full child list of the shared parent, skipping obj itself
        ObjectID parentID = mHierarchy.at(obj).parent;
        ObjectID cur      = mHierarchy.at(parentID).firstChild;
        while (cur != 0) {
            if (cur != obj) {
                result.push_back(cur);
            }
            cur = mHierarchy.at(cur).nextSibling;
        }
        return result;
    }

    std::vector<ObjectID> SceneHierarchy::getRootChildren() {
        std::vector<ObjectID> result;
        // The sentinel's firstChild is the head of the root-level sibling chain
        ObjectID cur = mHierarchy.at(ROOT_ID).firstChild;
        while (cur != 0) {
            result.push_back(cur);
            cur = mHierarchy.at(cur).nextSibling;
        }
        return result;
    }
}
