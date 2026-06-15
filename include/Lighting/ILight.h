
#pragma once

#include "LightData.h"

class ILight {
    protected:
        ILight() = default;
    public:
        virtual ~ILight() = default;

        virtual LightData getLightData() const = 0;
    private:
};
