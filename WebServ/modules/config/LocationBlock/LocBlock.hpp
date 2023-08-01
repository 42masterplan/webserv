#ifndef LOCBLOCK_HPP
# define LOCBLOCK_HPP

# include "../Tools/ParseTool.hpp"
# include "../otherBlock/OtherBlock.hpp"
# include "../interface/IBlock.hpp"
# include "../httpBlock/HttpBase.hpp"
/**
 * @brief location 블록 이 안에는 다른 블록은 오지 못하고 오직 limit_except 블록만 받도록 하겠습니다.
 *
 * @note [location 블록 주요 인자] cgi_pass, limit_except, return
 * @details [인자별 상세 설명]
 * cgi_pass : fastcgi인자와 비슷한 우리가 만든 인자입니다.
 * limit_except : 블록 형태로 들어오게 되고 location에서 사용할 메서드들을 제한합니다.
 * return : return 코드와 return_type으로 나눠서 저장해줍니다.
 */
class LocBlock : public IBlock, public HttpBase{
public:
	LocBlock(std::string loc_info);
	virtual ~LocBlock();
	// LocBlock& operator=(const LocBlock& obj );

	/*getter*/
	const std::string& getUploadStore()const;
	const std::string& getLocInfo()const;
	const int& getReturnCode()const;
	const std::string& getReturnPath()const;
	const std::string& getCgiPath()const;
	const int& getRank()const ;
	const bool& isLimit()const;
	const std::vector<std::string>& getDenyMethod()const;

	/*실제 사용할 경로를 찾아줄 getter*/
	virtual std::string getCombineErrorPath()const;
	std::string getCombineUploadStorePath()const;
	std::string getCombineReturnPath()const;
	std::string getCombineCgiPath()const;
	const std::string& getCombineLocPath()const;

	/*setter*/
	void	setCombinePath(std::string combine_path);
	void	setHighPriorityRoot(const std::string& root);

	bool isErrorBlock()const;
	void	refineAll();
	void	printInfo()const;

	/* 사용자가 호출하지 않는 public 함수 (재귀 템플릿 때문에 public.) */
	void	makeBlock(std::string line, std::ifstream& input, int& line_len_);
	std::map<std::string, std::string>& getDirStore();
private:
	LocBlock();
	void	parseLocDirective();
	void	parseReturn(std::string ret_line);

	std::map<std::string, std::string> loc_directives_;

	int rank_; //depth가 몇번째인지 즉 location block의 순위이다.
	std::string upload_store_;
	std::string loc_info_;//location / { 예시에서 : '/'를 이곳에 저장한다.
	int return_code_;//리다이렉션 코드
	std::string return_path_;//리다이렉션 경로
	std::string cgi_pass_;
	std::vector<std::string> deny_methods_; //거부하는 메서드
	bool is_limit_except_;//거부하는 메서드가 있는가 여부

	std::string combined_path_; //여기에 조합된 경로를 넣어준다.(root와 index를 조합해서 결국 반환되는)
	std::string high_priority_root_;
};
bool cmp(const LocBlock& a, const LocBlock& b);
#endif
