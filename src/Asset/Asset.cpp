#include "Asset/Asset.hpp"
#include <string>

namespace Spelt {
    Asset::Asset(std::string name)
        : mName(name)
    {

    }


    const std::string& Asset::getName() const {
        return mName;
    }
}
