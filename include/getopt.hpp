#include <getopt.h>
#include <string>
#include <vector>

class ProjectGenerate
{
public:
  ProjectGenerate(const std::string& project_name):_Projectname(project_name),_SrcDir(project_name+"/"+"src"),_IncludeDir(project_name+"/"+"include"){};
  void generate();
private:
  void createfile(const std::string& filepath,const std::string& content); //辅助写入文件
  std::string generateMainCpp() const ;     // 生成 main.hpp 文件内容
  std::string generateProjectCpp() const ;  // 生成 project_name.hpp 文件内容
  std::string generateMainHpp() const ;     // 生成 project_name.hpp 文件内容
  std::string generateProjectHpp() const ;  // 生成 type.hpp 文件内容
  std::string generateTypeHpp() const ;     // 生成 Makefile 内容
  std::string generateMakefileContent() const ;
  std::string generateGDBinitContent() const ;
  void createCompileCommands(); // 生成 compile_commands.json
  void GitInit();

  void createDiretory();
  void createfiles();
  void createMakefile();

  std::string _Projectname;
  std::string _SrcDir;
  std::string _IncludeDir;
  std::vector<std::string> _Subdir{"bin","build","doc","lib","test"};

};
