#pragma once


#include "Asset/AssetRepository.hpp"
#include "Mesh/Mesh.h"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshReference.hpp"

class AssetManager {
    private:
        AssetRepository<Mesh> mMeshRepo;
    public:
        MeshReference loadMesh(MeshAsset asset);
        MeshReference getMeshByName(const std::string& name) const;
        Mesh* getMesh(MeshReference ref);
        const Mesh* getMesh(MeshReference ref) const;
    private:
};
