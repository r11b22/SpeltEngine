#pragma once

#include "Renderer/RenderCommand.h"
#include "Renderer/RenderQueue.h"

namespace Spelt {

    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    enum class CameraType {
        UI,
        ThreeD,
    };

    class RenderPass {
        private:
            ProjectionType mProjectionType = ProjectionType::Perspective;
            CameraType mCamerType = CameraType::ThreeD;

            RenderQueue mQueue;
        public:
            void addToRenderQueue(std::vector<RenderCommand> command);
            const RenderQueue& getRenderQueue() const;

            void setProjectionType(ProjectionType type);
            ProjectionType getProjectionType() const;

            void setCameraType(CameraType type);
            CameraType getCameraType() const;

            void clearRenderQueue();
        private:
    };
}
