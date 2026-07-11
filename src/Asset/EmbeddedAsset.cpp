

#include "Asset/EmbeddedAsset.hpp"

std::string embeddedAssetToString(EmbeddedAsset asset){
    return std::string{reinterpret_cast<const char*>(asset.data), asset.size};
}
