

#include "Error/Panic.hpp"
#include <iostream>
namespace Spelt {
    void fatalPanic(std::string_view message, std::source_location location) {
        std::cerr << "\n==================================================\n";
        std::cerr << "!! ENGINE FATAL ERROR !!\n";
        std::cerr << "==================================================\n";
        std::cerr << "Message:  " << message << "\n";
        std::cerr << "File:     " << location.file_name() << "\n";
        std::cerr << "Line:     " << location.line() << "\n";
        std::cerr << "Function: " << location.function_name() << "\n";
        std::cerr << "==================================================\n\n";

        std::flush(std::cerr);
        std::abort();
    }
}
