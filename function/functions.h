/*******************************************
*author:xbfeng
*data:2020-05-12
********************************************/
#ifdef WIN32
#include <io.h>
#include <direct.h> 
#else
#include <unistd.h>
#include <sys/stat.h>
#endif
#include <stdint.h>
#include <string>

int32_t createDirectory(const std::string &directoryPath);