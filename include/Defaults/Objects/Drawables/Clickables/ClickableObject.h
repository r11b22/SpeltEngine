//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_CLICKABLEOBJECT_H
#define CGVCPROJECT_CLICKABLEOBJECT_H
#include "Mesh.h"
#include "../../TransformableObject.h"
#include "ClickableMesh.h"
#include "Renderer/IDrawable.h"

class ClickableObject : public TransformableObject, public IDrawable{
private:
    ClickableMesh mClickableMesh;
public:
    explicit ClickableObject(const std::string& name, Mesh mesh);
    ~ClickableObject() override = default;

    void draw(ShaderProgram& shaderProgram) override;
    std::string getShaderProgramName() override;
    void setClickCallback(std::function<void()> callback);

    void onClick() const;
private:
};


#endif //CGVCPROJECT_CLICKABLEOBJECT_H