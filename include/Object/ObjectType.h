#pragma once

#include <concepts>

class Object;

template <typename T>
concept ObjectType = std::derived_from<T, Object>;
