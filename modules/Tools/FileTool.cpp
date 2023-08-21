#include "FileTool.hpp"

/**
 * @brief 존재하는 파일인 지 확인하는 함수입니다.
 * 
 * @param filePath 
 * @return true
 * @return false 
 */
bool	isExistFile(std::string filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}

/**
 * @brief 해당 경로가 파일인 지 폴더인 지 확인하는 함수입니다.
 * 
 * @param file_path 
 * @return true 
 * @return false 
 */
bool	isFolder(const std::string& file_path){
  struct stat path_info;
  if (stat(file_path.c_str(), &path_info) != 0) //오토인덱스이면서 없는 폴더 혹은 파일일 때.
    return false;
  if (S_ISDIR(path_info.st_mode))
    return true;
  return false;
}
