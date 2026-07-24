

#include "OpenGL/BindTracker.hpp"
namespace Spelt {


    BindTracker::BindTracker() {
        mCurrentlyBound.fill(NoneID);
    }

    unsigned int BindTracker::getBound(BindType type) const {
        return mCurrentlyBound[static_cast<size_t>(type)];
    }

    bool BindTracker::isBound(BindType type, unsigned int id) const {
        return getBound(type) == id;
    }

    void BindTracker::bind(BindType type, unsigned int id) {
        mCurrentlyBound[static_cast<size_t>(type)] = id;
    }

    void BindTracker::unbind(BindType type, unsigned int id) {
        auto index = static_cast<size_t>(type);
        if (mCurrentlyBound[index] == id) {
            mCurrentlyBound[index] = NoneID;
        }
    }
}
