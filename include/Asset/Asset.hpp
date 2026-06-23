#pragma once

#include <string>
class Asset {
    private:
        std::string mName;
    public:
        Asset(std::string name);

        const std::string& getName() const;
    private:
};
