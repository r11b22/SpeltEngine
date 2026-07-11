#ifndef EMBEDDED_ASSET_STRUCT
#define EMBEDDED_ASSET_STRUCT
struct EmbeddedAsset {
    const char* version;
    const unsigned char* data;
    const unsigned int size;
};
#endif // EMBEDDED_ASSET_STRUCT
