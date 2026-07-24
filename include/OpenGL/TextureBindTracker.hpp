#pragma once

#include <cstddef>
#include <vector>

namespace Spelt {
    // Textures don't have a single global bind point like buffers do -
    // they're bound per *texture unit* (GL_TEXTURE0, GL_TEXTURE1, ...),
    // and the number of units is a runtime GPU limit rather than a
    // compile-time enum, so this can't reuse BindTracker's
    // std::array<BindType::Count> approach directly.
    class TextureBindTracker {
    public:
        static TextureBindTracker& getInstance() {
            static TextureBindTracker instance;
            return instance;
        }

        TextureBindTracker(const TextureBindTracker&) = delete;
        TextureBindTracker& operator=(const TextureBindTracker&) = delete;
        TextureBindTracker(TextureBindTracker&&) = delete;
        TextureBindTracker& operator=(TextureBindTracker&&) = delete;

        // Number of texture units this tracker knows about.
        size_t unitCount() const;

        unsigned int getBound(unsigned int textureUnit) const;

        bool isBound(unsigned int textureUnit, unsigned int id) const;

        void bind(unsigned int textureUnit, unsigned int id);

        void unbind(unsigned int textureUnit, unsigned int id);

    private:
        TextureBindTracker();

        constexpr static unsigned int NoneID = 0;

        // Queries GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS so the tracker
        // covers every unit the driver actually exposes.
        static unsigned int queryMaxTextureUnits();

        std::vector<unsigned int> mCurrentlyBound;
    };
}
