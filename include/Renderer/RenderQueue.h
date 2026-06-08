//
// Created by joost on 6/4/26.
//

#ifndef CGVCPROJECT_RENDERQUEUE_H
#define CGVCPROJECT_RENDERQUEUE_H
#include <memory>
#include <string>
#include <unordered_map>

#include "IDrawable.h"

class RenderQueue {
private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<IDrawable>>> mDrawables = {};
    std::vector<std::shared_ptr<IDrawable>> mDrawablesOrdered = {};
public:

    const std::unordered_map<std::string, std::vector<std::shared_ptr<IDrawable>>>& getDrawables() const;
    const std::vector<std::shared_ptr<IDrawable>>& getDrawablesOrdered() const;

    void addDrawable(std::shared_ptr<IDrawable> drawable);
    void removeDrawable(const std::shared_ptr<IDrawable>& drawable);
private:
};

#endif //CGVCPROJECT_RENDERQUEUE_H