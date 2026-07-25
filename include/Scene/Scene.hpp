//
// Created by joost on 5/23/26.
//

#pragma once

#include "Asset/AssetLoader.hpp"
#include "Asset/AssetManager.hpp"
#include "Lighting/ILight.h"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshReference.hpp"
#include "Object/Object.h"
#include "Object/ObjectID.h"
#include "Object/ObjectRepository.h"
#include "Renderer/RenderPass.hpp"
#include "Scene/SceneLoadRequest.hpp"
#include "SceneHierarchy.h"
#include "../Camera.h"
#include "../Renderer/IDrawable.h"
#include "../Window.h"
#include "Renderer/RenderQueue.h"
#include "Lighting/LightData.h"
#include "Texture/CubemapAsset.hpp"
#include "Texture/TextureAsset.hpp"
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <algorithm>
#include <format>

namespace Spelt {
    class Renderer;

    enum class SceneError{
        NotLoaded
    };

    class Scene {
    private:
        ObjectRepository mObjects;

        // ID ROOT_ID (0) is a virtual sentinel whose firstChild is the head of the
        // root-level sibling chain. All real objects have IDs > 0.
        SceneHierarchy mHierarchy;

        std::vector<RenderPass> mRenderPasses;

        AssetManager* mAssetManager = nullptr;

        std::vector<ObjectReference<IDrawable>> mDrawables;
        std::vector<ObjectReference<ILight>> mLights;

        ObjectReference<Camera> mCurrentCamera;

        bool mRunning = false;

        std::vector<std::unique_ptr<ILoadRequest>> mAssets = {};

        bool mDrawn = true;
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
        * Get a list of all render passes, these render passes contain all the objects that need to be drawn
        */
        const std::vector<RenderPass>& getRenderPasses();

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

        void setAssetManager(AssetManager* assetManager);

        Result<AssetManager&, SceneError> getAssetManager();
        Result<const AssetManager&, SceneError> getAssetManager() const;

        template<typename T>
        void addAsset(AssetLoadInfo<T> toLoad){
            mAssets.push_back(std::make_unique<LoadRequest<T>>(toLoad));
        }

        void loadAssets(AssetManager& assetManager) const;

        void createRenderPass();
        size_t renderPassCount();

        Option<RenderPass&> getRenderPass(size_t idx);

        void setDrawn(bool drawn);
        bool isDrawn();

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
}
