#pragma once

#include <string>
namespace Spelt {
    class Asset {
        private:
            std::string mName;
        public:
            Asset(std::string name);

            const std::string& getName() const;
        private:
    };
}
