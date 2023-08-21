#ifndef FILE_TOOL_HPP
# define FILE_TOOL_HPP
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include <cmath>
# include <cstdio>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <signal.h>
# include <sys/stat.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

# define YELLOW "\x1B[33m"
# define GREEN "\x1B[32m"
# define RED "\x1B[31m"
# define CLOSE "\x1B[0m"

# define BLUE "\x1B[34m"
# define MAGENTA "\x1B[35m"
# define CYAN "\x1B[36m"
# define WHITE "\x1B[37m"

# define BOLD "\x1B[1m"  // 굵게 표시
# define UNDERLINE "\x1B[4m"  // 밑줄

#define LIGHT_BLACK "\x1B[90m"
#define LIGHT_RED "\x1B[91m"
#define LIGHT_GREEN "\x1B[92m"
#define LIGHT_YELLOW "\x1B[93m"
#define LIGHT_BLUE "\x1B[94m"
#define LIGHT_MAGENTA "\x1B[95m"
#define LIGHT_CYAN "\x1B[96m"
#define LIGHT_WHITE "\x1B[97m"

bool	isExistFile(std::string filePath);
bool	isFolder(const std::string& file_path);

#endif
