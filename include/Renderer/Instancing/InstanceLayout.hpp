#pragma once

/*
 * Keeps track of all possible types that can be an instance attribute
 */
#include <cstddef>
#include <string>
#include <vector>
enum class InstanceAttribType {
    Float,
    Int,
    Mat4,
    Vec3
};


/**
 * Has size data for each attribute
 */
struct InstanceAttribDesc {
    std::string name;
    InstanceAttribType type;
    size_t byteSize;
    size_t offset;
};

/**
 * Has the layout info for one full instance
 * Contains the InstanceAttribDesc
 */
struct InstanceLayout {
    std::vector<InstanceAttribDesc> attribs;
    size_t stride = 0;
};
