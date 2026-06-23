#include "Asset/Asset.hpp"
#include <string>


Asset::Asset(std::string name)
    : mName(name)
{

}


const std::string& Asset::getName() const {
    return mName;
}
