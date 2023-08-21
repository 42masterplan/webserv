#ifndef AUTO_INDEX_HPP
# define AUTO_INDEX_HPP
# include <string>
# include <vector>
class HttpResponse;

/**
 * @brief 오토인덱싱 기능을 제공하는 정적 클래스입니다.
 */
class AutoIndex {
  public:
    static std::vector<char>  getDirectoryListing(HttpResponse& res);
  private:
    AutoIndex();
    ~AutoIndex();
    static std::string        getFileTemplate(const char* link, const char* file, const char* size, const char* modified_time);
    static const std::string  autoindex_template_;
    static const std::string  f_info_template_;
};

#endif
