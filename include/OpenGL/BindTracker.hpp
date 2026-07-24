#pragma once

#include <array>
#include <cstddef>
namespace Spelt {
    enum class BindType {
        Buffer,
        Texture,
        FrameBufferDraw,
        FrameBufferRead,
        // Any new members must be added BEFORE Count

        Count // Automatically tracks the number of elements
    };

    class BindTracker {
    public:
        static BindTracker& getInstance() {
            static BindTracker instance;
            return instance;
        }

        BindTracker(const BindTracker&) = delete;
        BindTracker& operator=(const BindTracker&) = delete;
        BindTracker(BindTracker&&) = delete;
        BindTracker& operator=(BindTracker&&) = delete;

        unsigned int getBound(BindType type) const;

        bool isBound(BindType type, unsigned int id) const;

        void bind(BindType type, unsigned int id);

        void unbind(BindType type, unsigned int id);

    private:
        BindTracker();

        constexpr static unsigned int NoneID = 0;
        std::array<unsigned int, static_cast<size_t>(BindType::Count)> mCurrentlyBound;
    };
}
