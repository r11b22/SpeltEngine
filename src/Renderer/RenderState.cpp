//
// Created by joost on 6/13/26.
//

#include "Renderer/RenderState.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const RenderState& state) {
    os << "RenderState {\n"
       << "  srcBlendFactor: " << state.srcBlendFactor << "\n"
       << "  dstBlendFactor: " << state.dstBlendFactor << "\n"
       << "  cullMode: " << static_cast<int>(state.cullMode) << "\n"
       << "  depthFunc: " << static_cast<int>(state.depthFunc) << "\n"
       << "  stencilFunc: " << static_cast<int>(state.stencilFunc) << "\n"
       << "  stencilRef: " << static_cast<int>(state.stencilRef) << "\n"
       << "  stencilReadMask: 0x" << std::hex << static_cast<int>(state.stencilReadMask) << std::dec << "\n"
       << "  stencilWriteMask: 0x" << std::hex << static_cast<int>(state.stencilWriteMask) << std::dec << "\n"
       << "  stencilFailOp: " << static_cast<int>(state.stencilFailOp) << "\n"
       << "  stencilZFailOp: " << static_cast<int>(state.stencilZFailOp) << "\n"
       << "  stencilZPassOp: " << static_cast<int>(state.stencilZPassOp) << "\n"
       // Bitfields must be cast to standard types to pass to streams
       << "  depthTest: " << std::boolalpha << static_cast<bool>(state.depthTest) << "\n"
       << "  depthWrite: " << static_cast<bool>(state.depthWrite) << "\n"
       << "  blend: " << static_cast<bool>(state.blend) << "\n"
       << "  cullFace: " << static_cast<bool>(state.cullFace) << "\n"
       << "  stencilTest: " << static_cast<bool>(state.stencilTest) << "\n"
       << "}";
    return os;
}
