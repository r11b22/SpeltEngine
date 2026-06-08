//
// Created by joost on 5/2/26.
//

#ifndef CGVCPROJECT_CLICKABLEMESH_H
#define CGVCPROJECT_CLICKABLEMESH_H
#include "../../../../Renderer/IDrawable.h"
#include "InputManager.h"
#include "Mesh.h"


class ClickableMesh : public IDrawable {
private:
    Mesh mMesh;
    std::function<void()> mClickCallback;
public:
    ClickableMesh(Mesh mesh);
    ~ClickableMesh() override = default;

    void draw(ShaderProgram &shaderProgram) override;
    std::string getShaderProgramName() override;

    void setClickCallback(std::function<void()> callback);
    void onClick() const;
private:
};

#endif //CGVCPROJECT_CLICKABLEMESH_H