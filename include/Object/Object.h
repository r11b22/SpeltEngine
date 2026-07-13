//
// Created by joost on 5/29/26.
//

#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Asset/AssetManager.hpp"
#include "ObjectID.h"

namespace Spelt {
    class IObjectReference;

    class Scene;

    enum class ObjectError {
        NoScene
    };

    /**
    * An abstract object interface for use in scenes
    */
    class Object {
    private:
        ObjectID mID;
        std::string mName;
        Scene* mScene;

        bool mLoaded;
    public:
        Object(std::string name);
        virtual ~Object() = default;

        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] uint64_t getID() const;

        virtual void onLoad() {}
        virtual void onUpdate(float deltaT) {}
        virtual void onDestroy() {}

        void setLoaded(bool loaded);
        bool isLoaded() const;

        Result<void, ObjectError> destroy();

        void setParent(IObjectReference& obj);

        void setScene(Scene* scene);
        Option<Scene&> getScene() const;

        Result<AssetManager&, ObjectError> getAssetManager();
        Result<const AssetManager&, ObjectError> getAssetManager() const;
    };
}
