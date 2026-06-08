//
// Created by joost on 5/25/26.
//

#include "Scene/Scene.h"
#include <algorithm>
#include <iostream>

// ---------------------------------------------------------------------------
// Constructor — seed the sentinel root node
// ---------------------------------------------------------------------------

Scene::Scene() {
    // ROOT_ID (0) is never attached to an Object; it acts purely as the
    // virtual parent of all top-level objects. Its firstChild field is the
    // head of the root-level sibling chain.
    mHierarchy.emplace(ROOT_ID, SceneHierarchyComponent{});
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void Scene::markRunning() {
    mRunning = true;
}

void Scene::loadObjects() {
    for (const auto& object : mObjects) {
        object.second->onLoad();
    }
}

void Scene::updateObjects(float deltaT) {
    for (const auto& object : mObjects) {
        object.second->onUpdate(deltaT);
    }
}

// ---------------------------------------------------------------------------
// Drawables / lights / camera
// ---------------------------------------------------------------------------

const RenderQueue& Scene::getRenderQueue() const {
    return mRenderQueue;
}

void Scene::addPointLight(std::shared_ptr<PointLight> light) {
    mPointLights.push_back(std::move(light));
}

void Scene::removePointLight(std::shared_ptr<PointLight> light) {
    mPointLights.erase(std::remove(mPointLights.begin(), mPointLights.end(), light), mPointLights.end());
}

const std::vector<std::shared_ptr<PointLight>>& Scene::getPointLights() const {
    return mPointLights;
}

const std::shared_ptr<AmbientLight>& Scene::getAmbientLight() const {
    return mAmbientLight;
}

void Scene::setCamera(const std::shared_ptr<Camera>& camera) {
    mCurrentCamera = camera;
}

std::shared_ptr<Camera> Scene::getCamera() const {
    if (!mCurrentCamera) {
        std::cerr << "Warning: no camera attached to scene!" << std::endl;
    }
    return mCurrentCamera;
}

// ---------------------------------------------------------------------------
// Object management
// ---------------------------------------------------------------------------

void Scene::addObject(std::shared_ptr<Object> object) {
    ObjectID id = object->getID();
    mObjects.emplace(id, object);
    object->setScene(this);

    // Register a fresh hierarchy node and attach at root level
    mHierarchy.emplace(id, SceneHierarchyComponent{});
    appendToParent(id, ROOT_ID);

    // If it is Drawable add it to the drawables list
    // We use polymorphism because no object should be added or removed every frame
    if (std::shared_ptr<IDrawable> drawable = std::dynamic_pointer_cast<IDrawable>(object)) {
        mRenderQueue.addDrawable(drawable);
    }

    if (std::shared_ptr<PointLight> light = std::dynamic_pointer_cast<PointLight>(object)) {
        addPointLight(std::move(light));
    }

    if (!mCurrentCamera) {
        if (std::shared_ptr<Camera> camera = std::dynamic_pointer_cast<Camera>(object)) {
            setCamera(camera);
        }
    }

    if (std::shared_ptr<AmbientLight> ambientLight = std::dynamic_pointer_cast<AmbientLight>(object)) {
        if (!mAmbientLight) {
            mAmbientLight = std::move(ambientLight);
        }else {
            throw std::runtime_error("Scenes can not contain multiple ambient lights!");
        }
    }

    if (mRunning) {
        object->onLoad();
    }
}

void Scene::destroyObject(const std::shared_ptr<Object>& object) {
    ObjectID id = object->getID();
    if (!mHierarchy.count(id)) return;

    // Recursively destroy all children first (depth-first, post-order)
    std::vector<ObjectID> childIDs;
    ObjectID cur = mHierarchy.at(id).firstChild;
    while (cur != 0) {
        childIDs.push_back(cur);
        cur = mHierarchy.at(cur).nextSibling;
    }
    for (ObjectID childID : childIDs) {
        auto it = mObjects.find(childID);
        if (it != mObjects.end()) {
            destroyObject(it->second);
        }
    }

    // Now destroy this node itself
    object->onDestroy();

    // Remove it from drawables if it was a drawable
    if (std::shared_ptr<IDrawable> drawable = std::dynamic_pointer_cast<IDrawable>(object)) {
        mRenderQueue.removeDrawable(drawable);
    }

    if (std::shared_ptr<PointLight> light = std::dynamic_pointer_cast<PointLight>(object)) {
        removePointLight(std::move(light));
    }


    unlinkFromParent(id);
    mHierarchy.erase(id);
    mObjects.erase(id);
}

std::shared_ptr<Object> Scene::getObjectByName(const std::string& name) {
    auto it = std::find_if(mObjects.begin(), mObjects.end(), [&name](const auto& pair) {
        return pair.second && pair.second->getName() == name;
    });
    return (it != mObjects.end()) ? it->second : nullptr;
}

// ---------------------------------------------------------------------------
// Internal hierarchy helpers
// ---------------------------------------------------------------------------

// Detach `id` from its current parent's sibling chain.
// The node's own firstChild pointer (its subtree) is left completely untouched.
void Scene::unlinkFromParent(ObjectID id) {
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
void Scene::appendToParent(ObjectID id, ObjectID parentID) {
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

void Scene::setParent(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& parent) {
    ObjectID objID    = obj->getID();
    ObjectID parentID = parent ? parent->getID() : ROOT_ID;

    if (!mHierarchy.count(objID))    return;
    if (!mHierarchy.count(parentID)) return;

    unlinkFromParent(objID);
    appendToParent(objID, parentID);
}

std::shared_ptr<Object> Scene::getParent(const std::shared_ptr<const Object>& obj) const{
    ObjectID id = obj->getID();
    if (!mHierarchy.count(id)) return nullptr;
    ObjectID parentID = mHierarchy.at(id).parent;
    if (parentID == ROOT_ID) return nullptr;
    auto it = mObjects.find(parentID);
    return (it != mObjects.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Object>> Scene::getChildren(const std::shared_ptr<const Object>& obj) const{
    std::vector<std::shared_ptr<Object>> result;
    ObjectID id = obj->getID();
    if (!mHierarchy.count(id)) return result;

    ObjectID cur = mHierarchy.at(id).firstChild;
    while (cur != 0) {
        auto it = mObjects.find(cur);
        if (it != mObjects.end()) result.push_back(it->second);
        cur = mHierarchy.at(cur).nextSibling;
    }
    return result;
}

std::vector<std::shared_ptr<Object>> Scene::getSiblings(const std::shared_ptr<Object>& obj) {
    std::vector<std::shared_ptr<Object>> result;
    ObjectID id = obj->getID();
    if (!mHierarchy.count(id)) return result;

    // Walk the full child list of the shared parent, skipping obj itself
    ObjectID parentID = mHierarchy.at(id).parent;
    ObjectID cur      = mHierarchy.at(parentID).firstChild;
    while (cur != 0) {
        if (cur != id) {
            auto it = mObjects.find(cur);
            if (it != mObjects.end()) result.push_back(it->second);
        }
        cur = mHierarchy.at(cur).nextSibling;
    }
    return result;
}

std::vector<std::shared_ptr<Object>> Scene::getRootChildren() {
    std::vector<std::shared_ptr<Object>> result;
    // The sentinel's firstChild is the head of the root-level sibling chain
    ObjectID cur = mHierarchy.at(ROOT_ID).firstChild;
    while (cur != 0) {
        auto it = mObjects.find(cur);
        if (it != mObjects.end()) result.push_back(it->second);
        cur = mHierarchy.at(cur).nextSibling;
    }
    return result;
}
