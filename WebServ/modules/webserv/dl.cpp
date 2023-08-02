#include "ServManager.hpp"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

std::string dl_template =
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
  "            <th>크기</th>\n"
  "            <th>수정 시간</th>\n"
  "        </tr>\n";

std::string f_info_template =
  "        <tr>\n"
  "            <td><a href=\"_FILE_\">_FILE_</a></td>\n"
  "            <td>_SIZE_</td>\n"
  "            <td>_AMEND_TIME_</td>\n"
  "        </tr>\n";

std::string  concatTemplate(const char* file, const char* size, const char* amend_time){
  std::string buff = f_info_template;
  size_t pos = 0;

  pos = buff.find("_FILE_", pos);
  buff.replace(pos, 6, file);
  pos = buff.find("_FILE_", pos);
  buff.replace(pos, 6, file);
  pos = buff.find("_SIZE_", pos);
  buff.replace(pos, 6, size);
  pos = buff.find("_AMEND_TIME_", pos);
  buff.replace(pos, 12, amend_time);
  return buff;
}

void  getDirectoryListing(char* input_dir){
  DIR*          dir;
  std::string   ret;
  struct dirent *ent;
  struct stat   file_stat;
  char  l_buff[20]; //long의 최대 자릿수 19

  std::string content;
  std::string line;

  dir = opendir(input_dir);
  if (!dir)
    throw std::runtime_error("opendir() ERROR");
  while (ent = readdir(dir)){
    char  filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "%s/%s", input_dir, ent->d_name);
    stat(filepath, &file_stat);
    sprintf(l_buff, "%ld", file_stat.st_size);
    dl_template += concatTemplate(ent->d_name, l_buff, ctime(&file_stat.st_mtime));
  }
  dl_template = dl_template + "    </table>\n" + "</body>\n" + "</html>";
  std::cout << dl_template << "\n";
}

int main(){
  getDirectoryListing("../../var/www");
  return 0;
}