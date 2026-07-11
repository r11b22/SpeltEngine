#pragma once

#include <string>

#ifndef EMBEDDED_ASSET_STRUCT
#define EMBEDDED_ASSET_STRUCT
/*
 * Embedded asset data structure
 * This struct can not change without changing SpeltEngine-AssetHeaderGenerator
 */

struct EmbeddedAsset {
    const char* version;
    const unsigned char* data;
    const unsigned int size;
};
#endif // EMBEDDED_ASSET_STRUCT


std::string embeddedAssetToString(EmbeddedAsset asset);
