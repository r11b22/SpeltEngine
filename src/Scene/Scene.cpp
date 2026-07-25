//
// Created by joost on 5/25/26.
//

#include "Scene/Scene.hpp"
#include "Asset/AssetManager.hpp"
#include "Error/Option.hpp"
#include "Lighting/ILight.h"
#include "Lighting/LightData.h"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshReference.hpp"
#include "Object/Object.h"
#include "Object/ObjectID.h"
#include "Object/ObjectRepository.h"
#include "Renderer/RenderPass.hpp"
#include "Texture/CubemapAsset.hpp"
#include "Texture/TextureAsset.hpp"
#include "Texture/TextureReference.hpp"
#include <Tracy/tracy/Tracy.hpp>
#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>


namespace Spelt {
    // ---------------------------------------------------------------------------
    // Constructor — seed the sentinel root node
    // ---------------------------------------------------------------------------

    Scene::Scene() {
        createRenderPass();
    }

    // ---------------------------------------------------------------------------
    // Lifecycle
    // ---------------------------------------------------------------------------

    void Scene::markRunning() {
        mRunning = true;
    }

    void Scene::loadObjects() {
        for (auto [id, object] : mObjects) {
            object->onLoad();
            object->setLoaded(true);
        }
    }

    void Scene::updateObjects(float deltaT) {
        for (auto [id, object] : mObjects) {
            object->onUpdate(deltaT);
        }
    }

    // ---------------------------------------------------------------------------
    // Drawables / lights / camera
    // ---------------------------------------------------------------------------

    const std::vector<RenderPass>& Scene::getRenderPasses(){
        ZoneScoped;
        // TODO move this to somewhere else

        for(RenderPass& pass : mRenderPasses){
            pass.clearRenderQueue();
        }


        for (const auto& drawable : mDrawables) {
            auto passCommands = drawable->getRenderCommands();

            for (const auto& pass : passCommands){
                mRenderPasses[pass.pass].addToRenderQueue(pass.commands);
            }

        }

        return mRenderPasses;
    }

    void Scene::addLight(ObjectReference<ILight> light) {
        mLights.push_back(light);
    }

    void Scene::removeLight(ObjectReference<ILight> light) {
        mLights.erase(std::remove(mLights.begin(), mLights.end(), light), mLights.end());
    }

    void Scene::addDrawable(ObjectReference<IDrawable> drawable) {
        mDrawables.push_back(std::move(drawable));
    }

    void Scene::removeDrawable(ObjectReference<IDrawable> drawable) {
        mDrawables.erase(std::remove(mDrawables.begin(), mDrawables.end(), drawable), mDrawables.end());
    }


    std::vector<LightData> Scene::getLightData() {
        std::vector<LightData> result;

        // This is a patch between the new light system and an older Object system
        // When a propper non heap object system is created this should be replaced by that
        for (const auto& light : mLights)
        {
            result.push_back(light->getLightData());
        }

        return std::move(result);
    }

    Camera* Scene::getCamera() const {
        if (mCurrentCamera.isNoReference()) {
            std::cerr << "Warning: no camera attached to scene!" << std::endl;
        }
        return mCurrentCamera.get();
    }

    // ---------------------------------------------------------------------------
    // Object management
    // ---------------------------------------------------------------------------



    void Scene::destroyObject(IObjectReference& object) {
        ObjectID id = object.getUntyped()->getID();
        destroyObjectByID(id);
    }

    void Scene::destroyObjectByID(ObjectID id) {
        std::vector<ObjectID> destroyed = mHierarchy.removeObject(id);

        // FIXME: this should be done layer per layer instead of all at once
        // IDEA: use a on destroy lambda that has the id
        for(auto objID : destroyed){
            Object* object = mObjects.get(objID);
            object->setLoaded(false);
            object->onDestroy();
        }

        for(auto objID : destroyed){
            ObjectReference<Object> ref = mObjects.makeReference<Object>(objID);

            ObjectReference<IDrawable> asDrawable = ref.template as<IDrawable>();
            if(!asDrawable.isNoReference()){
                removeDrawable(asDrawable);
            }

            ObjectReference<ILight> asLight = ref.template as<ILight>();
            if(!asLight.isNoReference()){
                removeLight(asLight);
            }

            ObjectReference<Camera> asCamera = ref.template as<Camera>();
            if(!asCamera.isNoReference()){
                if(mCurrentCamera == asCamera){
                    mCurrentCamera = {};
                }
            }

            mObjects.remove(objID);
        }
    }



    // ---------------------------------------------------------------------------
    // Public hierarchy API
    // ---------------------------------------------------------------------------

    void Scene::setParent(IObjectReference& obj, IObjectReference& parent) {
        ObjectID objID    = obj.getUntyped()->getID();
        ObjectID parentID = parent.getUntyped()->getID();

        setParentByID(objID, parentID);
    }

    void Scene::setParentByID(ObjectID objID, ObjectID parentID) {
        mHierarchy.setParent(objID, parentID);
    }

    /*
    * Assets
    *
    */

    void Scene::setAssetManager(AssetManager* assetManager){
        mAssetManager = assetManager;
    }




    Result<AssetManager&, SceneError> Scene::getAssetManager() {
        if(mAssetManager){
            return Result<AssetManager&, SceneError>::createValue(*mAssetManager);
        }else{
            return Error(SceneError::NotLoaded);
        }
    }

    Result<const AssetManager&, SceneError> Scene::getAssetManager() const{
        if(mAssetManager){
            return Result<const AssetManager&, SceneError>::createValue(*mAssetManager);
        }else{
            return Error(SceneError::NotLoaded);
        }
    }


    void Scene::loadAssets(AssetManager& assetManager) const{
        for (const auto& toLoad : mAssets){
            toLoad->load(assetManager);
        }
    }

    void Scene::createRenderPass() {
        mRenderPasses.emplace_back();
    }

    size_t Scene::renderPassCount(){
        return mRenderPasses.size();
    }

    Option<RenderPass&> Scene::getRenderPass(size_t idx){
        if(idx >= renderPassCount()){
            return None{};
        }

        return Option<RenderPass&>::createValue(mRenderPasses[idx]);
    }

    void Scene::setDrawn(bool drawn){
        mDrawn = drawn;
    }

    bool Scene::isDrawn(){
        return mDrawn;
    }
}
