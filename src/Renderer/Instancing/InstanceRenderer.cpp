#include "Renderer/Instancing/InstanceRenderer.hpp"
#include "Buffer/Buffer.h"
#include "Renderer/Instancing/InstanceLayout.hpp"
#include <iostream>


namespace Spelt {
    InstanceRenderer::InstanceRenderer(int uniformLocation)
        : mInstanceScratch{},
        mInstanceStreamBuffer(GL_STREAM_DRAW),
        mUniformLocation(uniformLocation)
    {

    }


    void InstanceRenderer::draw(IRenderable* toRender, ShaderProgram& program, const std::vector<InstanceData>& instances){
        const size_t instanceCount = instances.size();
        if (instanceCount == 0) return;

        // TODO check if instancing is supported

        InstanceLayout layout = instances[0].getLayout();

        // Resize the scratch buffer to fit the to draw data
        mInstanceScratch.resize(layout.stride * instanceCount);
        std::byte* ptr = mInstanceScratch.data();

        for (size_t i = 0; i < instanceCount; i++){
            instances[i].writeToBuffer(ptr + (i * layout.stride), layout);
        }


        mInstanceStreamBuffer.setDataAndOrphan(mInstanceScratch.data(), mInstanceScratch.size());
        mInstanceStreamBuffer.bindBase(mUniformLocation);

        toRender->drawInstanced(program, instanceCount);
    }
}
