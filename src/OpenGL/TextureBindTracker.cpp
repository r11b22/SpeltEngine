#include "OpenGL/TextureBindTracker.hpp"

#include <cassert>

// Swap this include for whatever GL loader Spelt actually uses
// (glad/glad.h, GL/glew.h, gl3w.h, etc.) - only glGetIntegerv and
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS are needed here.
#include <glad/glad.h>

namespace Spelt {

    unsigned int TextureBindTracker::queryMaxTextureUnits() {
        GLint maxUnits = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        return static_cast<unsigned int>(maxUnits);
    }

    TextureBindTracker::TextureBindTracker()
        : mCurrentlyBound(queryMaxTextureUnits(), NoneID) {
    }

    size_t TextureBindTracker::unitCount() const {
        return mCurrentlyBound.size();
    }

    unsigned int TextureBindTracker::getBound(unsigned int textureUnit) const {
        assert(textureUnit < mCurrentlyBound.size() && "Texture unit out of range");
        return mCurrentlyBound[textureUnit];
    }

    bool TextureBindTracker::isBound(unsigned int textureUnit, unsigned int id) const {
        return getBound(textureUnit) == id;
    }

    void TextureBindTracker::bind(unsigned int textureUnit, unsigned int id) {
        assert(textureUnit < mCurrentlyBound.size() && "Texture unit out of range");
        mCurrentlyBound[textureUnit] = id;
    }

    void TextureBindTracker::unbind(unsigned int textureUnit, unsigned int id) {
        assert(textureUnit < mCurrentlyBound.size() && "Texture unit out of range");
        if (mCurrentlyBound[textureUnit] == id) {
            mCurrentlyBound[textureUnit] = NoneID;
        }
    }
}
