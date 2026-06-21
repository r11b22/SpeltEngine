//
// Created by joost on 5/23/26.
//

#ifndef CGVCPROJECT_SCENE_H
#define CGVCPROJECT_SCENE_H
#include "Lighting/ILight.h"
#include "Object/Object.h"
#include "Object/ObjectID.h"
#include "Object/ObjectRepository.h"
#include "SceneHierarchy.h"
#include "../Camera.h"
#include "../Renderer/IDrawable.h"
#include "../Window.h"
#include "Renderer/RenderQueue.h"
#include "Lighting/LightData.h"
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <algorithm>
#include <format>

class Renderer;

class Scene {
private:
    ObjectRepository mObjects;

    // ID ROOT_ID (0) is a virtual sentinel whose firstChild is the head of the
    // root-level sibling chain. All real objects have IDs > 0.
    SceneHierarchy mHierarchy;

    RenderQueue mRenderQueue;

    std::vector<ObjectReference<IDrawable>> mDrawables;
    std::vector<ObjectReference<ILight>> mLights;

    ObjectReference<Camera> mCurrentCamera;

    bool mRunning = false;



public:
    Scene();
    virtual ~Scene() = default;

    /**
     * Create a new object owned this scene
     */
    template <typename T, typename... Args>
    ObjectReference<T> createObject(Args&&... args){
        ObjectID result = mObjects.add<T>(std::forward<Args>(args)...);
        ObjectReference<T> obj = mObjects.makeReference<T>(result);
        addObject(obj);

        return std::move(obj);
    }


    /**
     * Get a list of all objects in the scene that can draw
     */
    const RenderQueue& getRenderQueue();

    /**
     *
     */
    std::vector<LightData> getLightData();

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
     * Get the current active camera
     */
    Camera* getCamera() const;

    /**
     * remove an object from the scene
     * @param object the object to remove
     */
    void destroyObject(IObjectReference& object);

    /**
     * remove an object from the scene using its unique ID
     * @param object the object to remove
     */
    void destroyObjectByID(ObjectID id);

    /**
     * Finds the first object with this name
     * @param name the name of the object
     * @return
     */
    template <typename T>
    ObjectReference<T> getObjectByName(const std::string& name){
        auto it = std::find_if(mObjects.begin(), mObjects.end(), [&name](auto objIdPair) {
            return objIdPair.second && objIdPair.second->getName() == name;
        });

        if (it != mObjects.end())
        {
            ObjectID foundId = (*it).first;

            return mObjects.makeReference<T>(foundId);

        }
        else
        {
            return ObjectReference<T>::noReference();
        }
    }

    /**
     * Set the parent of an object
     * @param obj the object to set the parent of
     * @param parent the new parent of the object
     */
    void setParent(IObjectReference& obj, IObjectReference& parent);

    void setParentByID(ObjectID objID, ObjectID parentID);


    /**
     * Get a list of all objects in the scene of the specified type
     */
    template<typename T>
    ObjectView<T> getObjectsOfType() {
        return mObjects.getObjectsOfType<T>();
    }

    /**
     * Get a list of all objects in the scene of the specified type
     */
    template<typename T>
    ObjectView<const T> getObjectsOfType() const {
        return mObjects.getObjectsOfType<T>();
    }


    /**
     * Get the parent of an object
     * @param obj the object to get the parent of, nullptr if parent is root
     * @return
     */
    template <typename T>
    ObjectReference<T> getParent(IObjectReference& obj) const{
        ObjectID id = obj.getUntyped()->getID();
        return getParentByID<T>(id);
    }

    /**
     * Get the parent of an object using its unique ID
     * @param obj the object to get the parent of, nullptr if parent is root
     * @return
     */
    template <typename T>
    ObjectReference<T> getParentByID(ObjectID id) const{
        ObjectID parentId = mHierarchy.getParent(id);

        return mObjects.makeReference<T>(parentId);
    }

    /**
     * Get the list of children of this object
     * @param obj the object to get the children for
     * @return
     */
    template <typename T>
    std::vector<ObjectReference<T>> getChildrenOfType(IObjectReference& obj) const{
        ObjectID id = obj.getUntyped()->getID();
        return getChildrenOfTypeByID<T>(id);
    }



    /**
     * Get the list of children of an object using its unique ID
     * @param obj the object to get the children for
     * @return
     */
    template <typename T>
    std::vector<ObjectReference<T>> getChildrenOfTypeByID(ObjectID id) const{
        std::vector<ObjectReference<T>> result;

        std::vector<ObjectID> childrenIDs = mHierarchy.getChildren(id);

        for (auto child : childrenIDs){
            if(mObjects.isOfType<T>(child)){
                ObjectReference<T> ref = mObjects.makeReference<T>(child);
                result.push_back(ref);
            }
        }

        return result;
    }


    /**
     * Get the list of siblings of an object using its Unique ID
     * @param obj the object to get the siblings for
     * @return
     */
    template <typename T>
    std::vector<ObjectReference<T>> getSiblingsOfTypeByID(ObjectID id){
        std::vector<ObjectReference<T>> result;

        std::vector<ObjectID> siblings = mHierarchy.getSiblings(id);

        for (auto sibling : siblings){
            if (mObjects.isOfType<T>(sibling)){
                ObjectReference<T> ref = mObjects.makeReference<T>(sibling);
                result.push_back(ref);
            }
        }

        return result;
    }

    /**
     * Get the list of siblings of this object
     * @param obj the object to get the siblings for
     * @return
     */
    template <typename T>
    std::vector<ObjectReference<T>> getSiblingsOfType(IObjectReference& obj){

        ObjectID id = obj.getUntyped()->getID();
        return getSiblingsOfTypeByID<T>(id);
    }

    /**
     * Get the list of children from the root
     * @return
     */
    template <typename T>
    std::vector<ObjectReference<T>> getRootChildrenOfType(){
        std::vector<ObjectReference<T>> result;

        std::vector<ObjectID> childrenIDs = mHierarchy.getRootChildren();

        for (auto child : childrenIDs){
            if(mObjects.isOfType<T>(child)){
                ObjectReference<T> ref = mObjects.makeReference<T>(child);
                result.push_back(ref);
            }
        }

        return result;
    }

private:

    /**
     * Add a new object to the scene,
     * Objects are automatically parented to the root
     */
    template <typename T>
    void addObject(ObjectReference<T> object) {
        ObjectID id = object.getUntyped()->getID();

        mHierarchy.addObject(id);

        object.getUntyped()->setScene(this);


        ObjectReference<IDrawable> asDrawable = object.template as<IDrawable>();
        if(!asDrawable.isNoReference()){
            addDrawable(asDrawable);
        }

        ObjectReference<ILight> asLight = object.template as<ILight>();
        if(!asLight.isNoReference()){
            addLight(asLight);
        }

        ObjectReference<Camera> asCamera = object.template as<Camera>();
        if(!asCamera.isNoReference()){
            mCurrentCamera = asCamera;
        }

        if (mRunning) {
            object.getUntyped()->onLoad();
        }
    }


    void addLight(ObjectReference<ILight> light);
    void removeLight(ObjectReference<ILight> light);

    void addDrawable(ObjectReference<IDrawable> drawable);
    void removeDrawable(ObjectReference<IDrawable> drawable);
};

#endif //CGVCPROJECT_SCENE_H
