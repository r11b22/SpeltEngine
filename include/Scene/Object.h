//
// Created by joost on 5/29/26.
//

#ifndef CGVCPROJECT_OBJECT_H
#define CGVCPROJECT_OBJECT_H
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "ObjectID.h"

class Scene;

/**
 * An abstract object interface for use in scenes
 */
class Object : public std::enable_shared_from_this<Object>{
private:
    ObjectID mID;
    std::string mName;
    Scene* mScene;

public:
    Object(std::string name);
    virtual ~Object() = default;

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] uint64_t getID() const;

    virtual void onLoad() {}
    virtual void onUpdate(float deltaT) {}
    virtual void onDestroy() {}

    void destroy();

    void setParent(const std::shared_ptr<Object>& obj);

    void setScene(Scene* scene);
    Scene& getScene() const;
};
#endif //CGVCPROJECT_OBJECT_H