#ifndef CGVCPROJECT_BUFFERMAP_H
#define CGVCPROJECT_BUFFERMAP_H
#include <iostream>
#include <Buffer/Buffer.h>

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

    T* get() { return mValid ? mData : nullptr; }

    void free() {
        if (mValid && mOwner) {
            mOwner->unmap();
            mValid = false;
            mData = nullptr;
        }
    }
};

#endif //CGVCPROJECT_BUFFERMAP_H