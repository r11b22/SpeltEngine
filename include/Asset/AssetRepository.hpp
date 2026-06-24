#pragma once

#include "Asset/Asset.hpp"
#include "Asset/AssetReference.hpp"
#include "DataStructures/SparseSet.hpp"
#include <concepts>
#include <stdexcept>

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

        T* getAsset(AssetReference<T> reference){
            int id = reference.getID();

            return mData.get(id);
        }

        const T* getAsset(AssetReference<T> reference) const {
            int id = reference.getID();

            return mData.get(id);
        }
    private:
};
