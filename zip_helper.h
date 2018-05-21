#ifndef __ZIP_HELPER  
#define __ZIP_HELPER  
  
#include <map>  
#include <string>  
  
//1.暂时不支持子目录  
//注意: 因为使用了zlib库,使用时加上预编译宏 ZLIB_WINAPI  
class ZipHelper  
{  
public:  
    ZipHelper(){}  
    ~ZipHelper(){}  
  
    //path: utf8 path  
    ZipHelper& AddFile(const char* input_path,const char* inzip_path = "");
	ZipHelper& AddDir(const char* input_dir,const char* temp_dir = NULL);
	ZipHelper& ClearFile();
    //output_path :utf8 path  
    bool ToZip(const char* output_path);
  
private:  
    std::map<std::string,std::string> files_;  
};  
  
  
#endif  