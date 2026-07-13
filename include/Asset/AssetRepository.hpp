#pragma once

#include "Asset/Asset.hpp"
#include "Asset/AssetReference.hpp"
#include "DataStructures/SparseSet.hpp"
#include "Error/Option.hpp"
#include "Error/Result.hpp"
#include <concepts>
#include <stdexcept>

namespace Spelt {
    enum class AssetRepositoryError {
        NotFound
    };

    template <typename   T>
    requires std::derived_from<T, Asset>
    class AssetRepository {
        private:
            SparseSet<T> mData;
            int mCounter = 1;
        public:
            void clear(){
                mData.clear();
                mCounter = 1;
            }


            AssetReference<T> pushAsset(T asset){
                mData.insert(mCounter, std::move(asset));
                mCounter++;

                return AssetReference<T>(mCounter-1);
            }

            template <typename... Args>
            AssetReference<T> createAsset(Args&&... args){
                mData.emplace(mCounter, std::forward<Args>(args)...);
                mCounter++;

                return AssetReference<T>(mCounter-1);
            }

            [[nodiscard]] AssetReference<T> getAssetByName(const std::string& name) const{
                const auto it = std::find_if(mData.begin(), mData.end(), [&name](const auto& pair) {
                    return pair.second.getName() == name;
                });

                if (it == mData.end()){
                    throw std::runtime_error("Could not find asset with name!");
                }

                return AssetReference<T>{static_cast<unsigned int>((*it).first)};
            }

            [[nodiscard]] bool hasAssetOfName(const std::string& name) const {
                const auto it = std::find_if(mData.begin(), mData.end(), [&name](const auto& pair) {
                    return pair.second.getName() == name;
                });
                return it != mData.end();
            }

            T* getAsset(AssetReference<T> reference){
                int id = reference.getID();

                Option<T*> result = mData.get(id);

                return result.match(
                    [](T* asset){ return asset; },
                    []() { return static_cast<T*>(nullptr); }
                );
            }

            const T* getAsset(AssetReference<T> reference) const {
                int id = reference.getID();

                Option<const T*> result = mData.get(id);

                return result.match(
                    [](const T* asset){ return asset; },
                    []() { return nullptr; }
                );
            }

            Result<void, AssetRepositoryError> removeAsset(AssetReference<T> reference){
                int id = reference.getID();

                return mData.remove(id).replaceError(AssetRepositoryError::NotFound);
            }
        private:
    };
}
