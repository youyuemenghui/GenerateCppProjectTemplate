#include "getopt.hpp"
#include <string>
#include <vector>

namespace fs = std::filesystem;
void created_project_diretory_structure(const std::string& project_name)
{
  std::vector<std::string> DirArray={"bin", "build", "doc", "include", "lib", "src", "test"};

  fs::create_directory(project_name);
  for(std::string dir:DirArray)
  {
      fs::create_directory(project_name+"/"+dir);
  }

  // 创建源文件路径
  std::string SrcDir= project_name+"/"+DirArray[5];
  std::string HppDir= project_name+"/"+DirArray[3];
  std::cout<<SrcDir<<std::endl;
  std::cout<<HppDir<<std::endl;
  
  fs::create_directory(SrcDir);
  fs::create_directory(HppDir);
  
  std::fstream main_cpp(SrcDir+"/main.cpp",std::ios::out);
  std::fstream main_hpp(HppDir+"/main.hpp",std::ios::out);
  std::fstream project_hpp(HppDir+"/"+project_name+".hpp",std::ios::out);
  std::fstream type_hpp(HppDir+"/type.hpp",std::ios::out);
  std::fstream makefile(project_name+"/Makefile",std::ios::out);
  std::fstream gdbinit(project_name+"/"+project_name+".gdbinit",std::ios::out);

  // 写 Makefile
  makefile << "# 编译器\n";
  makefile << "CC := g++\n";
  makefile << "# 编译选项\n";
  makefile << "CFLAGS := -std=c++20 -Wall -Wextra -pedantic-errors -Wunused-parameter -Wuninitialized  -Wfloat-equal -Wconversion -g\n\n";
  makefile << "# 项目名称\n";
  makefile << "PROJECT_NAME := " << project_name << "\n\n";
  makefile << "# 目录\n";
  makefile << "SRC_DIR := src\n";
  makefile << "INC_DIR := include\n";
  makefile << "BUILD_DIR := build\n";
  makefile << "BIN_DIR := bin\n\n";
  makefile << "# 源文件和目标文件\n";
  makefile << "SRCS := $(wildcard $(SRC_DIR)/*.cpp)\n";
  makefile << "OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))\n";
  makefile << "DEPS := $(OBJS:.o=.d)\n\n";
  makefile << "# 目标可执行文件\n";
  makefile << "TARGET := $(BIN_DIR)/$(PROJECT_NAME)\n\n";
  makefile << "# 默认目标\n";
  makefile << ".PHONY: all\n";
  makefile << "all: $(TARGET)\n\n";
  makefile << "# 编译目标\n";
  makefile << "$(TARGET): $(OBJS)\n";
  makefile << "\t@mkdir -p $(BIN_DIR)\n";
  makefile << "\t$(CC) $(CFLAGS) $^ -o $@\n\n";
  makefile << "# 依赖关系\n";
  makefile << "-include $(DEPS)\n\n";
  makefile << "# 编译规则\n";
  makefile << "$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp\n";
  makefile << "\t@mkdir -p $(BUILD_DIR)\n";
  makefile << "\t$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@\n\n";
  makefile << "# 清理\n";
  makefile << ".PHONY: clean\n";
  makefile << "clean:\n";
  makefile << "\trm -rf $(BUILD_DIR) $(BIN_DIR)\n\n";
  makefile << "# 运行\n";
  makefile << ".PHONY: run\n";
  makefile << "run: $(TARGET)\n";
  makefile << "\t$(TARGET)\n";
  makefile << ".PHONY: debug\n";
  makefile << "debug:\n";
  makefile << "\tcgdb -- -x DLList.gdbinit $(TARGET)\n";
  makefile.close();
  
  // 写头文件
  main_hpp << "#include <iostream>\n";
  project_hpp << "#include \"type.hpp\"\n";
  project_hpp << "#include \"" << project_name << ".hpp\"\n";
  project_hpp.close();

  // 写源文件
  main_cpp << "#include \"main.hpp\"\n\n";
  main_cpp << "int main()\n";
  main_cpp << "{\n\n";
  main_cpp << "\treturn 0;\n";
  main_cpp << "}\n";
  main_cpp.close();

  gdbinit << "#\t设置自动加载符号信息\n";
  gdbinit << "set auto-load safe-path /\n\n";
  gdbinit << "# 启用颜色输出\n";
  gdbinit << "set pagination off\n";
  gdbinit << "set confirm off\n";
  gdbinit << "#set color enable\n\n";
  gdbinit << "# 设置反汇编显示格式为 Intel 风格\n";
  gdbinit << "set disassembly-flavor intel\n\n";
  gdbinit << "# 设置源代码路径，确保 GDB 能找到源代码\n";
  gdbinit << "directory ./src\n";
  gdbinit << "directory ./include/\n\n";
  gdbinit << "# 设置调试程序的路径\n";
  gdbinit << "#file ./bin/DLList\n\n";
  gdbinit << "break main\n\n";
  gdbinit << "# 默认显示所有局部变量\n";
  gdbinit << "define hook-stop\n";
  gdbinit << "\techo \"Current stack frame:\"\n\n";
  gdbinit << "\tinfo variables\n";
  gdbinit << "\tinfo locals   # 打印当前栈帧的所有局部变量\n";
  gdbinit << "\tinfo args     # 打印当前栈帧的所有函数参数\n";
  gdbinit << "\techo \"\n\"\n";
  gdbinit << "end\n\n";
  gdbinit << "# 自动显示结构体成员（对于C++类的成员变量很有帮助）\n";
  gdbinit << "set print pretty on\n";
  gdbinit << "# 设置断点时自动打印特定变量\n";
  gdbinit << "# 例如：每次触发断点时自动打印 \"myVar\" 和 \"myOtherVar\" 的值\n";
  gdbinit << "#define hook-stop\n";
  gdbinit << "#\techo \"Stopping at breakpoint...\n\"\n";
  gdbinit << "#\tprint DLList\n";
  gdbinit << "#\tprint DLLNode\n";
  gdbinit << "#\techo \"\n\"\n";
  gdbinit << "#end\n\n";
  gdbinit << "# 显示更多的栈帧信息\n";
  gdbinit << "#backtrace full\n\n";
  gdbinit << "# 让 GDB 在断点时自动显示函数名和行号\n";
  gdbinit << "define hook-stop\n";
  gdbinit << "\techo \"Stopped at: \"\n";
  gdbinit << "\tinfo line\n";
  gdbinit << "\tinfo locals\n";
  gdbinit << "\techo \"\n\"\n";
  gdbinit << "end\n\n\n";
  gdbinit << "# 设置默认断点行为（默认打印局部变量等信息）\n";
  gdbinit << "define hook-stop\n";
  gdbinit << "\tinfo locals\n";
  gdbinit << "\tinfo args\n";
  gdbinit << "\techo \"\n\"\n";
  gdbinit << "end\n";
  gdbinit << "\n";
  gdbinit << "run \n";
  gdbinit.close();

  // 生成 compile_commands.json
  // 假设你安装了 bear 并且可以从 C++ 程序中执行 shell 命令
  std::string command = "bear -- make > /dev/null && make clean > /dev/null";
  if (std::system(command.c_str()) != 0) 
  {
    std::cerr << "Failed to generate compile_commands.json or clean build directory" << std::endl;
    exit(1);
  }
  command.clear();
  command.append("git init");
  if (std::system(command.c_str()) != 0) 
  {
    std::cerr << "Failed to git init" << std::endl;
    exit(1);
  }
}

