//
// Created by joost on 5/29/26.
//


#include "Object/Object.h"

#include <atomic>
#include <iostream>
#include <stdexcept>

#include "Asset/AssetManager.hpp"
#include "Object/ObjectRepository.h"
#include "Scene/Scene.hpp"

Object::Object(std::string name)
    : mID(0), mName(std::move(name)), mScene(nullptr)
{
    static std::atomic_uint64_t objectCount{1};
    mID = objectCount.fetch_add(1);
}

uint64_t Object::getID() const{
    return mID;
}

const std::string& Object::getName() const{
    return mName;
}

void Object::setParent(IObjectReference& obj) {
    getScene().setParentByID(getID(), obj.getUntyped()->getID());
}

void Object::setScene(Scene *scene) {
    mScene = scene;
}

Scene &Object::getScene() const{
    if (mScene == nullptr) {
        throw std::runtime_error("Tried to get the scene for an object that is not part of a scene!");
    }

    return *mScene;
}

void Object::setLoaded(bool loaded){
    mLoaded = loaded;
}

bool Object::isLoaded() const {
    return mLoaded;
}

void Object::destroy() {
    if (mScene == nullptr) {
        throw std::runtime_error("You can not destroy an object that is not yet part of a scene!");
    }

    mScene->destroyObjectByID(getID());
}

AssetManager& Object::getAssetManager() {
    if (mScene == nullptr) {
        throw std::runtime_error("You can not manage assets inside an object that is not yet part of a scene!");
    }

    return mScene->getAssetManager();
}

const AssetManager& Object::getAssetManager() const{
    if (mScene == nullptr) {
        throw std::runtime_error("You can not manage assets inside an object that is not yet part of a scene!");
    }

    return mScene->getAssetManager();
}
