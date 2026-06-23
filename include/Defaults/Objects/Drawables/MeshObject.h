//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_MESHOBJECT_H
#define CGVCPROJECT_MESHOBJECT_H
#include "Mesh/Mesh.h"
#include "../TransformableObject.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/IDrawable.h"
#include "Renderer/RenderCommand.h"

class MeshObject : virtual public TransformableObject, public IDrawable{
private:
    MeshReference mMesh;
    std::shared_ptr<Material> mMaterial;
    std::string mShader = "litShader";

public:
    explicit MeshObject(const std::string& name, MeshReference mesh, const std::shared_ptr<Material>&  material);
    ~MeshObject() override = default;
    std::vector<RenderCommand> getRenderCommands() override;

    void setShader(std::string shader);

protected:
    std::shared_ptr<Material> getMaterial();
    std::string getShader();

private:
};

#endif //CGVCPROJECT_MESHOBJECT_H
