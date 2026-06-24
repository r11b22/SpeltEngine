//
// Created by joost on 5/25/26.
//

#include "Scene/Scene.hpp"
#include "Asset/AssetManager.hpp"
#include "Lighting/ILight.h"
#include "Lighting/LightData.h"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshReference.hpp"
#include "Object/Object.h"
#include "Object/ObjectID.h"
#include "Object/ObjectRepository.h"
#include "Texture/TextureAsset.hpp"
#include "Texture/TextureReference.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

// ---------------------------------------------------------------------------
// Constructor — seed the sentinel root node
// ---------------------------------------------------------------------------

Scene::Scene() {
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

const RenderQueue& Scene::getRenderQueue() {
    // TODO move this to somewhere else
    mRenderQueue.clear();

    for (const auto& drawable : mDrawables) {
        auto commands = drawable->getRenderCommands();

        mRenderQueue.submitRenderCommands(commands);
    }

    return mRenderQueue;
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

void Scene::addMeshAsset(MeshAsset asset){
    mMeshAssets.push_back(asset);
}

MeshReference Scene::getMeshByName(const std::string& name){
    if (mAssetManager){
        return mAssetManager->getMeshByName(name);
    }else{
        throw std::runtime_error("The Scene was not yet loaded! Getting assets can only be done after the scene is loaded!");
    }
}

const std::vector<MeshAsset>& Scene::getMeshAssets() const {
    return mMeshAssets;
}

void Scene::addTextureAsset(TextureAsset asset){
    mTextureAssets.push_back(asset);
}

TextureReference Scene::getTextureByName(const std::string& name){
    if (mAssetManager){
        return mAssetManager->getTextureByName(name);
    }else{
        throw std::runtime_error("The Scene was not yet loaded! Getting assets can only be done after the scene is loaded!");
    }
}

const std::vector<TextureAsset>& Scene::getTextureAssets() const {
    return mTextureAssets;
}
