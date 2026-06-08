//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_MESHOBJECT_H
#define CGVCPROJECT_MESHOBJECT_H
#include "Mesh.h"
#include "../TransformableObject.h"
#include "Renderer/IDrawable.h"
#include "Scene/Object.h"
#include "Transform/ITransformable.h"

class MeshObject : virtual public TransformableObject, public IDrawable{
private:
    std::shared_ptr<Mesh> mMesh;
public:
    explicit MeshObject(const std::string& name, const std::shared_ptr<Mesh>& mesh);
    ~MeshObject() override = default;
    void draw(ShaderProgram& shaderProgram) override;
    std::string getShaderProgramName() override;

    void setVertices(std::vector<float> vertices);
    void setIndices(std::vector<unsigned int> indices);
    void setShader(std::string shader);
    void setTexture(const TextureData& texData);

private:
};

#endif //CGVCPROJECT_MESHOBJECT_H