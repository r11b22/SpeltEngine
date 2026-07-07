//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_MESHOBJECT_H
#define CGVCPROJECT_MESHOBJECT_H
#include "../TransformableObject.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/IDrawable.h"
#include "Renderer/RenderCommand.h"

class MeshObject : virtual public TransformableObject, public IDrawable{
private:
    MeshReference mMesh;
    Material mMaterial;
    std::string mShader = "litShader";

public:
    explicit MeshObject(const std::string& name, MeshReference mesh, Material  material);
    ~MeshObject() override = default;
    std::vector<RenderCommand> getRenderCommands() override;

    void setShader(std::string shader);
    void setMesh(MeshReference mesh);
    void setMaterial(Material material);

protected:
    Material getMaterial();
    MeshReference getMesh() const;
    std::string getShader() const;

private:
};

#endif //CGVCPROJECT_MESHOBJECT_H
