#pragma once

template <typename T>
class AssetReference {
    private:
        unsigned int mID;
    public:
        AssetReference() : mID(0) {}
        AssetReference(unsigned int id) : mID(id) {}
        unsigned int getID(){
            return mID;
        }

        bool isNoReference() const {
            return mID == 0;
        }
    private:
};
