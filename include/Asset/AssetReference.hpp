#pragma once

#include <cstddef>
#include <functional>

namespace Spelt {
    template <typename T>
    class AssetReference {
        private:
            unsigned int mID;
        public:
            AssetReference() : mID(0) {}
            AssetReference(unsigned int id) : mID(id) {}
            unsigned int getID() const{
                return mID;
            }

            bool isNoReference() const {
                return mID == 0;
            }

            bool operator==(const AssetReference& other) const {
                    return mID == other.mID;
            }
        private:
    };


}

namespace std {
    template <typename T>
    struct hash<Spelt::AssetReference<T>> {
        std::size_t operator()(const Spelt::AssetReference<T>& ref) const noexcept {
            return std::hash<unsigned int>{}(ref.getId());
        }
    };
}
