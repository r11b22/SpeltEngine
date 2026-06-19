#pragma once

#include <stdexcept>
#include <format>
#include <utility>
#include <vector>


template <typename T>
class SparseSet {
private:
    std::vector<int> mSparse = {};
    std::vector<int> mDenseToSparse = {};
    std::vector<T> mDense = {};

    static constexpr int null_id = -1;

public:
    bool contains(int id) const {
        if (id >= mSparse.size()) return false;
        return mSparse[id] != null_id;
    }

    void insert(int id, T object) {
        if (contains(id)) {
            mDense[mSparse[id]] = std::move(object);
        } else {
            mDense.push_back(std::move(object));
            mDenseToSparse.push_back(id);
            int location = mDense.size() - 1;
            expandSparseToFit(id);
            mSparse[id] = location;
        }
    }

    template <typename... Args>
    void emplace(int id, Args&&... args) {
        // Construct T directly inside the vector without copying or moving it
        mDense.emplace_back(std::forward<Args>(args)...);

        int newIndex = static_cast<int>(mDense.size() - 1);
        mSparse[id] = newIndex;

        // Ensure mDenseToSparse is resized and updated accordingly
        if (newIndex >= mDenseToSparse.size()) {
            mDenseToSparse.resize(newIndex + 1);
        }
        mDenseToSparse[newIndex] = id;
    }

    T* get(int id) {
        if (!contains(id)) {
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        }
        return &mDense[mSparse[id]];
    }

    const T* get(int id) const {
        if (!contains(id)) {
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        }
        return &mDense[mSparse[id]];
    }

    void remove(int id) {
        if (!contains(id)) {
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        }

        int indexToRemove = mSparse[id];
        int lastIndex = static_cast<int>(mDense.size() - 1);

        if (indexToRemove != lastIndex) {
            // Automatically moves if it won't throw (is moveable), otherwise copies
            mDense[indexToRemove] = std::move_if_noexcept(mDense[lastIndex]);

            int movedEntity = mDenseToSparse[lastIndex];
            mSparse[movedEntity] = indexToRemove;
            mDenseToSparse[indexToRemove] = movedEntity;
        }

        mDense.pop_back();
        mDenseToSparse.pop_back();
        mSparse[id] = null_id;
    }

private:
    void expandSparseToFit(int id) {
        if (mSparse.size() <= id) {
            mSparse.resize(id + 1, null_id);
        }
    }
};
