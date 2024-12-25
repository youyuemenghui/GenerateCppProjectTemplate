#include "genproj.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;


void ProjectGenerate::createDiretory()
{
try
  {
    fs::create_directory(this->_Projectname);
    for(auto dir:_Subdir)
    {
      fs::create_directory(_Projectname+"/"+dir);
    }
    fs::create_directory(_SrcDir);
    fs::create_directory(_IncludeDir);
  }
  catch(fs::filesystem_error& Ecode)
  {
    std::cerr<<"Error creating directories: "<<Ecode.what()<<std::endl;
    throw;
  }
}

void ProjectGenerate::createfiles()
{
  try
  {
    this->createfile(_SrcDir+"/"+_Projectname+".cpp",generateProjectCpp());
    this->createfile(_SrcDir+"/"+"main.cpp",generateMainCpp());
    this->createfile(_IncludeDir+"/"+_Projectname+".hpp",generateProjectHpp());
    this->createfile(_IncludeDir+"/"+"main.hpp",generateMainHpp());
    this->createfile(_IncludeDir+"/"+"type.hpp",generateTypeHpp());
  }
  catch(const std::ios_base::failure& Ecode)
  {
    std::cerr<<"Error creating files:"<<Ecode.what()<<std::endl;
    throw ;
  }
}

void ProjectGenerate::createMakefile()
{
  try
  {
    std::ofstream makefile(_Projectname+"/"+"Makefile");
    makefile<<generateMakefileContent();
  }
  catch(const std::ios_base::failure& Ecode)
  {
    std::cerr<<"Error creating Makefile"<<Ecode.what()<<std::endl;
    throw;
  }
}
void ProjectGenerate::createfile(const std::string& filepath,const std::string& content)
{
    std::ofstream file(filepath);
    if(!file)
    {
      throw std::ios_base::failure("Failed to creating file"+filepath);
    }
    file<<content;
}

std::string ProjectGenerate::generateProjectCpp() const
{
  return "#include \""+ this->_Projectname + ".hpp\"\n";
}
std::string ProjectGenerate::generateMainCpp() const 
{
  return "#include \"main.hpp\"\n\nint main() {\n\n\n   return 0;\n}\n";
}

    // 生成 main.hpp 文件内容
std::string ProjectGenerate::generateMainHpp() const 
{
  return "#pragma once\n#include \"type.hpp\"\n#include \"" + this->_Projectname + ".hpp\"\n";
}

// 生成 project_name.hpp 文件内容
std::string ProjectGenerate::generateProjectHpp() const 
{
  return "#pragma once\n#include <iostream>\n";
}

// 生成 type.hpp 文件内容
std::string ProjectGenerate::generateTypeHpp() const 
{
  return "#pragma once\n";
}

// 生成 Makefile 内容
std::string ProjectGenerate::generateMakefileContent() const 
{
  return R"(
# 编译器
CC := g++
# 编译选项
CFLAGS := -std=c++20 -Wall -Wextra -pedantic-errors -Wunused-parameter -Wuninitialized -Wfloat-equal -Wconversion -g

# 项目名称
PROJECT_NAME := )" + _Projectname + R"(
# 目录
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin

# 源文件和目标文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# 目标可执行文件
TARGET := $(BIN_DIR)/$(PROJECT_NAME)

# 默认目标
.PHONY: all
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# 依赖关系
-include $(DEPS)

# 编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

# 清理
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 运行
.PHONY: run
run: $(TARGET)
	$(TARGET)

.PHONY: debug
debug:
	cgdb -- -x )" + _Projectname +R"(.gdbinit" $(TARGET)
)";
}

std::string ProjectGenerate::generateGDBinitContent() const 
{
  return R"(
# 设置自动加载符号信息
set auto-load safe-path /

# 启用颜色输出
set pagination off
set confirm off
#set color enable

# 设置反汇编显示格式为 Intel 风格
set disassembly-flavor intel

# 设置源代码路径，确保 GDB 能找到源代码
directory ./src
directory ./include/

# 设置调试程序的路径
#file ./bin/)"+ _Projectname + R"(

break main

# 默认显示所有局部变量
define hook-stop
  echo "Current stack frame:\n"
  info variables
  info locals   # 打印当前栈帧的所有局部变量
  info args     # 打印当前栈帧的所有函数参数
  echo "\n"
end

# 自动显示结构体成员（对于C++类的成员变量很有帮助）
set print pretty on
# 设置断点时自动打印特定变量
# 例如：每次触发断点时自动打印 "myVar" 和 "myOtherVar" 的值
#define hook-stop
#  echo "Stopping at breakpoint...\n"
#  print DLList
#  print DLLNode
#  echo "\n"
#end

# 显示更多的栈帧信息
#backtrace full

# 让 GDB 在断点时自动显示函数名和行号
define hook-stop
  echo "Stopped at: "
  info line
  info locals
  echo "\n"
end


# 设置默认断点行为（默认打印局部变量等信息）
define hook-stop
  info locals
  info args
  echo "\n"
end

run 

)";
}

void ProjectGenerate::generate()
{
  this->createDiretory();
  this->createfiles();
  this->createMakefile();
  this->createCompileCommands();
  this->GitInit();
  std::cout << "Project structure for '" << _Projectname << "' has been created successfully." << std::endl;
}
void ProjectGenerate::createCompileCommands()
{
  fs::current_path(_Projectname);
  std::string command = "bear -- make > /dev/null && make clean > /dev/null";
  try
  {
    if (std::system(command.c_str()) != 0) 
    {
      throw std::runtime_error("Failed to generate compile_commands.json or clean build directory");
    }
  } 
  catch (const std::runtime_error& e) 
  {
    // 处理异常，例如打印错误信息
    std::cerr << "Error: " << e.what() << std::endl;
    fs::current_path("..");
    // 这里可以选择其他的补救措施，或者直接返回
    return; // 直接返回，表示执行失败
  }
  fs::current_path("..");
  std::cout << "Successfully generated compile_commands.json and cleaned build directory." << std::endl;
}

void ProjectGenerate::GitInit()
{
  fs::current_path(_Projectname);
  std::string command = "git init >>/dev/null";
  if (std::system(command.c_str()) != 0) 
  try
  {
    if(std::system(command.c_str()) != 0)
    {
      throw std::runtime_error("Failed to git init");
    }
  }
  catch(const std::runtime_error& Ecode)
  {
    std::cerr << "Error:" << Ecode.what() << std::endl;
    fs::current_path("..");
    return;
  }
 fs::current_path("..");
 std::cout <<"git init Success"<<std::endl;
}

