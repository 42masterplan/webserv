#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP
# include <iostream>
# include <dirent.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/stat.h>
# include <vector>

/**
 * @brief 오토인덱싱 기능을 제공하는 정적 클래스입니다.
 */
class AutoIndex {
  public:
    static std::vector<char>  getDirectoryListing(const char* input_dir);
  private:
    AutoIndex();
    ~AutoIndex();
    static std::string        getFileTemplate(const char* file, const char* size, const char* modified_time);
    static const std::string  autoindex_template_;
    static const std::string  f_info_template_;
};

#endif
