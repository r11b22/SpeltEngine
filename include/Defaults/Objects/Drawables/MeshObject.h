//
// Created by joost on 6/2/26.
//

#pragma once

#include "../TransformableObject.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/IDrawable.h"
#include "Renderer/RenderCommand.h"
#include <cstddef>

namespace Spelt::Defaults {
    class MeshObject : virtual public TransformableObject, public IDrawable{
    private:
        MeshReference mMesh;
        Material mMaterial;
        std::string mShader = "litShader";

        size_t mRenderPass = 0;

    public:
        explicit MeshObject(const std::string& name, MeshReference mesh, Material  material);
        ~MeshObject() override = default;
        std::vector<RenderPassCommands> getRenderCommands() override;

        void setShader(std::string shader);
        void setMesh(MeshReference mesh);
        void setMaterial(Material material);
        void setRenderPass(size_t pass);

    protected:
        Material getMaterial();
        MeshReference getMesh() const;
        std::string getShader() const;
        size_t getRenderPass() const;

    private:
    };

}
