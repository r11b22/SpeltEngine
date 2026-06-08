//
// Created by joost on 5/29/26.
//


#include "Scene/Object.h"

#include <atomic>
#include <iostream>
#include <stdexcept>

#include "Scene/Scene.h"

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

void Object::setParent(const std::shared_ptr<Object> &obj) {
    getScene().setParent(shared_from_this(), obj);
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


void Object::destroy() {
    if (mScene == nullptr) {
        throw std::runtime_error("You can not destroy an object that is not yet part of a scene!");
    }

    mScene->destroyObject(shared_from_this());
}



