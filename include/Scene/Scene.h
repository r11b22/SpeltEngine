//
// Created by joost on 5/23/26.
//

#ifndef CGVCPROJECT_SCENE_H
#define CGVCPROJECT_SCENE_H
#include "Object.h"
#include "SceneHierarchyComponent.h"
#include "../Camera.h"
#include "../Renderer/IDrawable.h"
#include "../Window.h"
#include "Lighting/AmbientLight.h"
#include "Lighting/PointLight.h"
#include "Renderer/RenderQueue.h"

class Renderer;

// Reserved ID for the virtual root sentinel node (never attached to an Object)
static constexpr ObjectID ROOT_ID = 0;

class Scene {
private:
    std::unordered_map<ObjectID, std::shared_ptr<Object>> mObjects;

    // ID ROOT_ID (0) is a virtual sentinel whose firstChild is the head of the
    // root-level sibling chain. All real objects have IDs > 0.
    std::unordered_map<ObjectID, SceneHierarchyComponent> mHierarchy;

    RenderQueue mRenderQueue;

    std::vector<std::shared_ptr<PointLight>> mPointLights;
    std::shared_ptr<AmbientLight> mAmbientLight = nullptr;

    std::shared_ptr<Camera> mCurrentCamera = nullptr;

    bool mRunning = false;



public:
    Scene();
    virtual ~Scene() = default;

    /**
     * Get a list of all objects in the scene that can draw
     */
    const RenderQueue& getRenderQueue() const;

    /**
     * Get the list of PointLights in the scene
     */
    const std::vector<std::shared_ptr<PointLight>>& getPointLights() const;

    /**
     * Get the ambient light in the scene
     */
    const std::shared_ptr<AmbientLight>& getAmbientLight() const;

    /**
     * This function is called when the scene is loaded into the app
     * This does not mean the app is running yet
     */
    virtual void onLoad(Renderer& renderer, Window &window) {}

    /**
     * This function is called after all objects are loaded just before the first frame of the scene
     */
    virtual void onStart() {}

    /**
     * This function is called every frame before drawing
     */
    virtual void onUpdate(Renderer& renderer, Window& window, float deltaT) {}

    /**
     * Mark the scene as running
     */
    void markRunning();

    /**
     * Load in starting objects that are already in the scene before starting
     */
    void loadObjects();

    /**
     * Calls the update function on all objects
     */
    void updateObjects(float deltaT);

    /**
     * Add a new object to the scene,
     * Objects are automatically parented to the root
     */
    void addObject(std::shared_ptr<Object> object);



    /**
     * Set the current active camera
     */
    void setCamera(const std::shared_ptr<Camera>& camera);

    /**
     * Get the current active camera
     */
    std::shared_ptr<Camera> getCamera() const;

    /**
     * remove an object from the scene
     * @param object the object to remove
     */
    void destroyObject(const std::shared_ptr<Object>& object);

    /**
     * Finds the first object with this name
     * @param name the name of the object
     * @return
     */
    std::shared_ptr<Object> getObjectByName(const std::string& name);

    /**
     * Set the parent of an object
     * @param obj the object to set the parent of
     * @param parent the new parent of the object
     */
    void setParent(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& parent);

    /**
     * Get the parent of an object
     * @param obj the object to get the parent of, nullptr if parent is root
     * @return
     */
    std::shared_ptr<Object> getParent(const std::shared_ptr<const Object>& obj) const;

    /**
     * Get the list of children of this object
     * @param obj the object to get the children for
     * @return
     */
    std::vector<std::shared_ptr<Object>> getChildren(const std::shared_ptr<const Object>& obj) const;

    /**
     * Get the list of siblings of this object
     * @param obj the object to get the siblings for
     * @return
     */
    std::vector<std::shared_ptr<Object>> getSiblings(const std::shared_ptr<Object>& obj);

    /**
     * Get the list of children from the root
     * @return
     */
    std::vector<std::shared_ptr<Object>> getRootChildren();

private:
    // Internal hierarchy helpers
    void unlinkFromParent(ObjectID id);
    void appendToParent(ObjectID id, ObjectID parentID);

    void addPointLight(std::shared_ptr<PointLight> light);
    void removePointLight(std::shared_ptr<PointLight> light);
};

#endif //CGVCPROJECT_SCENE_H