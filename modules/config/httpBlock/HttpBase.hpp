#ifndef HTTPBASE_HPP
# define HTTPBASE_HPP
# include "../../Tools/ParseTool.hpp"
class HttpBase
{
public :
	HttpBase();
	virtual ~HttpBase();

	/*getter*/
	const std::string& getRoot()const;
	const std::vector<std::string>& getIndex()const;
	const bool& isAutoIndex()const;
	const int& getClientMaxBodySize()const;
	const std::vector<int>& getErrorCode()const;
	const std::string& getErrorPage()const;

	/*실제 사용할 경로를 찾아줄 getter*/
	virtual std::string getCombineErrorPath()const;

	void	printHttpInfo()const;
	void	setInherit(HttpBase &base);
protected :
	void	parseHttpDirective(std::map<std::string, std::string>& dir_store);
	void	setAutoIndex(const std::string& value);
	void	setErrorPage(const std::string& line);

	std::string root_;
	std::vector<std::string> index_;
	bool autoindex_;//on 일때 true
	int client_max_body_size_;//없으면 0
	std::vector<int> error_code_;
	std::string error_page_;//에러페이지의 경로 (root를 안 붙힌상태)
};

#endif
