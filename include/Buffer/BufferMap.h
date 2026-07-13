#pragma once

#include <iostream>
#include <Buffer/Buffer.h>


namespace Spelt {
    template <typename T>
    class BufferMap {
    private:
        bool mValid = true;
        T* mData;
        Buffer* mOwner;

    public:
        // Define the constructor directly in the header
        BufferMap(T* data, Buffer* owner) : mData(data), mOwner(owner) {}

        // Define the destructor here too
        ~BufferMap() {
            free();
        }

        // Explicitly disable copying (as discussed before)
        BufferMap(const BufferMap&) = delete;
        BufferMap& operator=(const BufferMap&) = delete;


        // --- Move Constructor ---
        BufferMap(BufferMap&& other) noexcept
            : mValid(other.mValid), mData(other.mData), mOwner(other.mOwner) {
            // Invalidate the source object so its destructor does nothing
            other.mValid = false;
            other.mData = nullptr;
            other.mOwner = nullptr;
        }

        // --- Move Assignment Operator ---
        BufferMap& operator=(BufferMap&& other) noexcept {
            if (this != &other) {
                // 1. Clean up our own existing resource first
                free();

                // 2. Copy the state from the other object
                mValid = other.mValid;
                mData = other.mData;
                mOwner = other.mOwner;

                // 3. Invalidate the source object
                other.mValid = false;
                other.mData = nullptr;
                other.mOwner = nullptr;
            }
            return *this;
        }

        T* get() { return mValid ? mData : nullptr; }

        void free() {
            if (mValid && mOwner) {
                mOwner->unmap().panicOnError("Tried to unmap a map from a buffer that is no longer mapped!");
                mValid = false;
                mData = nullptr;
            }
        }
    };

}
