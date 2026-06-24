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
    // ── Iterator ────────────────────────────────────────────────────────────

    template <bool IsConst>
    class Iterator {
    public:
        using SetType    = std::conditional_t<IsConst, const SparseSet, SparseSet>;
        using ValueType  = std::conditional_t<IsConst, const T, T>;
        using PairType   = std::pair<int, ValueType&>;

        using iterator_category = std::random_access_iterator_tag;
        using value_type        = PairType;
        using difference_type   = std::ptrdiff_t;
        // No pointer/reference typedefs: pair is synthesized on dereference.

        Iterator(SetType* set, std::ptrdiff_t index) : mSet(set), mIndex(index) {}

        // Allow implicit conversion from non-const to const iterator
        Iterator(const Iterator<false>& other) requires IsConst
            : mSet(other.mSet), mIndex(other.mIndex) {}

        PairType operator*() const {
            return { mSet->mDenseToSparse[mIndex], mSet->mDense[mIndex] };
        }

        // Increment / decrement
        Iterator& operator++()    { ++mIndex; return *this; }
        Iterator  operator++(int) { auto tmp = *this; ++mIndex; return tmp; }
        Iterator& operator--()    { --mIndex; return *this; }
        Iterator  operator--(int) { auto tmp = *this; --mIndex; return tmp; }

        // Arithmetic
        Iterator  operator+(difference_type n) const { return { mSet, mIndex + n }; }
        Iterator  operator-(difference_type n) const { return { mSet, mIndex - n }; }
        Iterator& operator+=(difference_type n) { mIndex += n; return *this; }
        Iterator& operator-=(difference_type n) { mIndex -= n; return *this; }

        difference_type operator-(const Iterator& other) const { return mIndex - other.mIndex; }

        PairType operator[](difference_type n) const {
            return { mSet->mDenseToSparse[mIndex + n], mSet->mDense[mIndex + n] };
        }

        // Comparisons
        bool operator==(const Iterator&) const = default;
        auto operator<=>(const Iterator& other) const { return mIndex <=> other.mIndex; }

    private:
        friend class Iterator<!IsConst>;  // let const iterator access non-const fields
        SetType*      mSet;
        std::ptrdiff_t mIndex;
    };

    using iterator       = Iterator<false>;
    using const_iterator = Iterator<true>;

    iterator       begin()        { return { this, 0 }; }
    iterator       end()          { return { this, static_cast<std::ptrdiff_t>(mDense.size()) }; }
    const_iterator begin()  const { return { this, 0 }; }
    const_iterator end()    const { return { this, static_cast<std::ptrdiff_t>(mDense.size()) }; }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    void clear() {
        for (int id : mDenseToSparse) {
            mSparse[id] = null_id;
        }
        mDense.clear();
        mDenseToSparse.clear();
    }

    bool contains(int id) const {
        if (id >= static_cast<int>(mSparse.size())) return false;
        return mSparse[id] != null_id;
    }

    void insert(int id, T object) {
        if (contains(id)) {
            mDense[mSparse[id]] = std::move(object);
        } else {
            mDense.push_back(std::move(object));
            mDenseToSparse.push_back(id);
            int location = static_cast<int>(mDense.size() - 1);
            expandSparseToFit(id);
            mSparse[id] = location;
        }
    }

    template <typename... Args>
    void emplace(int id, Args&&... args) {
        mDense.emplace_back(std::forward<Args>(args)...);
        int newIndex = static_cast<int>(mDense.size() - 1);
        expandSparseToFit(id);
        mSparse[id] = newIndex;
        if (newIndex >= static_cast<int>(mDenseToSparse.size())) {
            mDenseToSparse.resize(newIndex + 1);
        }
        mDenseToSparse[newIndex] = id;
    }

    T* get(int id) {
        if (!contains(id))
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        return &mDense[mSparse[id]];
    }

    const T* get(int id) const {
        if (!contains(id))
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        return &mDense[mSparse[id]];
    }

    void remove(int id) {
        if (!contains(id))
            throw std::runtime_error(std::format("{} is not contained in SparseSet", id));
        int indexToRemove = mSparse[id];
        int lastIndex     = static_cast<int>(mDense.size() - 1);
        if (indexToRemove != lastIndex) {
            mDense[indexToRemove] = std::move_if_noexcept(mDense[lastIndex]);
            int movedEntity = mDenseToSparse[lastIndex];
            mSparse[movedEntity]          = indexToRemove;
            mDenseToSparse[indexToRemove] = movedEntity;
        }
        mDense.pop_back();
        mDenseToSparse.pop_back();
        mSparse[id] = null_id;
    }

private:
    void expandSparseToFit(int id) {
        if (static_cast<int>(mSparse.size()) <= id)
            mSparse.resize(id + 1, null_id);
    }
};
