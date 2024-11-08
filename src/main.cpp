#include "main.hpp"

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <project_name>" << std::endl;
        return 1;
    }

    std::string project_name = argv[1];
    created_project_diretory_structure(project_name);
    return 0;
}
