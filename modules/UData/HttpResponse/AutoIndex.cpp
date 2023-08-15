#include "AutoIndex.hpp"
# include "HttpResponse.hpp"
AutoIndex::AutoIndex(){}

AutoIndex::~AutoIndex(){}

/**
 * @brief 오토인덱싱 html 상 템플릿입니다.
 * 자체로는 쓸모가 없고 파일 명세를 추가하며 완성해야 합니다.
 */
const std::string AutoIndex::autoindex_template_ =
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<head>\n"
  "    <title>디렉토리 인덱싱</title>\n"
  "    <style>\n"
  "        body {\n"
  "            font-family: Arial, sans-serif;\n"
  "        }\n"
  "        table {\n"
  "            border-collapse: collapse;\n"
  "            width: 100%;\n"
  "        }\n"
  "        th, td {\n"
  "            border: 1px solid #dddddd;\n"
  "            text-align: left;\n"
  "            padding: 8px;\n"
  "        }\n"
  "        tr:nth-child(even) {\n"
  "            background-color: #f2f2f2;\n"
  "        }\n"
  "    </style>\n"
  "</head>\n"
  "<body>\n"
  "    <h1>디렉토리 목록</h1>\n"
  "    <table>\n"
  "        <tr>\n"
  "            <th>파일명</th>\n"
  "            <th>크기(byte)</th>\n"
  "            <th>수정 시간</th>\n"
  "        </tr>\n";

/**
 * @brief 오토인덱싱 html 하 템플릿입니다.
 * 이 템플릿 내부의 _FILE_, _LINK_, _SIZE_, _MODIFIED_TIME_ 키워드가 파일에 맞는 값으로 대체됩니다.
 */
const std::string AutoIndex::f_info_template_ =
  "        <tr>\n"
  "            <td><a href=\"_LINK_\">_FILE_</a></td>\n"
  "            <td>_SIZE_</td>\n"
  "            <td>_MODIFIED_TIME_</td>\n"
  "        </tr>\n";

/**
 * @brief 오토인덱싱 html 하 템플릿을 가공하는 함수입니다.
 * 각 키워드를 파일의 해당 값으로 변경합니다.
 * @param file 파일의 이름입니다.
 * @param size 파일의 크기입니다. 바이트 단위입니다.
 * @param modified_time 파일이 최근 수정된 시간입니다.
 * @return 가공된 템플릿이 담긴 string입니다.
 */
std::string  AutoIndex::getFileTemplate(const char* link, const char* file, const char* size, const char* modified_time){
  std::string buff = f_info_template_;
  size_t pos = 0;

  pos = buff.find("_LINK_", pos);
  buff.replace(pos, 6, link);
  pos = buff.find("_FILE_", pos);
  buff.replace(pos, 6, file);
  pos = buff.find("_SIZE_", pos);
  buff.replace(pos, 6, size);
  pos = buff.find("_MODIFIED_TIME_", pos);
  buff.replace(pos, 15, modified_time);
  return (buff);
}

/**
 * @brief 디렉토리 리스팅, 즉 오토인덱스 페이지를 생성하는 함수입니다.
 * @param input_dir 페이지를 생성할 파일 경로입니다.
 * @return 생성된 오토인덱스 페이지가 담긴 string입니다.
 * @exception opendir(), readdir(), stat() 시스템콜에서 에러 발생 시 runtime_error를 throw합니다.
 */
std::vector<char>  AutoIndex::getDirectoryListing(HttpResponse& cur_response){
  const char* input_dir = cur_response.file_path_.c_str();
  DIR*          dir;
  std::string   form = autoindex_template_;
  struct dirent *ent;
  struct stat   file_stat;
  std::vector<char> ret;
  std::cout << "디렉토리::" << input_dir << std::endl;
  dir = opendir(input_dir);
  if (!dir)
    return ret;
  ent = readdir(dir);
  if (!ent)
    return ret;
  while (ent){
    std::string file_path = input_dir;
    std::string temp = cur_response.loc_block_.getRoot();
    std::cout << "루트: " << temp << std::endl;
    if (stat(file_path.c_str(), &file_stat) == -1)
      return ret;
    file_path.replace(file_path.find(temp), temp.size(), "");
    file_path += ent->d_name;
    // std::cout << "2nd디렉토리::" << file_path << std::endl;
    std::string file_size = std::to_string(file_stat.st_size);
    form += getFileTemplate(file_path.c_str(), ent->d_name, file_size.c_str(), ctime(&file_stat.st_mtime));
    ent = readdir(dir);
  }
  form += "    </table>\n</body>\n</html>";
  ret = std::vector<char>(form.begin(), form.end());
  return (ret);
}
