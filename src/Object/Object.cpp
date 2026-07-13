//
// Created by joost on 5/29/26.
//


#include "Object/Object.h"

#include <atomic>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "Asset/AssetManager.hpp"
#include "Error/Option.hpp"
#include "Error/Result.hpp"
#include "Object/ObjectRepository.h"
#include "Scene/Scene.hpp"

namespace Spelt {
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
        getScene().value("Object was not added to a scene! Could not get parent.").setParentByID(getID(), obj.getUntyped()->getID());
    }

    void Object::setScene(Scene *scene) {
        mScene = scene;
    }

    Option<Scene&> Object::getScene() const{
        if (mScene == nullptr) {
            return Option<Scene&>::createNone();
        }

        return Option<Scene&>::createValue(*mScene);
    }

    void Object::setLoaded(bool loaded){
        mLoaded = loaded;
    }

    bool Object::isLoaded() const {
        return mLoaded;
    }

    Result<void, ObjectError> Object::destroy() {
        if (mScene == nullptr) {
            return Result<void, ObjectError>::createError(ObjectError::NoScene);
        }

        mScene->destroyObjectByID(getID());

        return Result<void, ObjectError>::createValue();
    }

    Result<AssetManager&, ObjectError> Object::getAssetManager() {
        if (mScene == nullptr) {
            return Result<AssetManager&, ObjectError>::createError(ObjectError::NoScene);
        }

        return Result<AssetManager&, ObjectError>::createValue(mScene->getAssetManager());
    }

    Result<const AssetManager&, ObjectError> Object::getAssetManager() const{
        if (mScene == nullptr) {
            return Result<const AssetManager&, ObjectError>::createError(ObjectError::NoScene);
        }

        return Result<const AssetManager&, ObjectError>::createValue(mScene->getAssetManager());
    }
}
