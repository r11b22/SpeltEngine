//
// Created by joost on 6/12/26.
//

#include "Renderer/RenderCommand.h"
#include "Material/Material.h"

namespace Spelt {
    bool DrawCommand::isInstanceOf(const DrawCommand& other) const {
        return shaderName == other.shaderName &&
                mesh == other.mesh && // Assumes MeshReference has == defined
                material == other.material &&
                staticUniforms == other.staticUniforms && // std::vector == compares elements in order
                textureUniforms == other.textureUniforms &&
                (instances.size() == 0|| other.instances.size() == 0 || instances[0].isInstanceOf(other.instances[0]));
    }
}
