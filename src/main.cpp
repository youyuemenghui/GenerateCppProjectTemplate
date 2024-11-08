#include "main.hpp"
#include <iostream>

int main(int argc, char *argv[]) 
{
   if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <project_name>" << std::endl;
        return 1;
    }

    try {
        ProjectGenerate generator(argv[1]);
        generator.generate();
    } catch (const std::exception& e) {
        std::cerr << "Error generating project: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
