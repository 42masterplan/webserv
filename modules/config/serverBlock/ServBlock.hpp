#ifndef SERVBLOCK_HPP
# define SERVBLOCK_HPP
# include "../LocationBlock/LocBlock.hpp"
# include "../interface/IBlock.hpp"
# include "../httpBlock/HttpBase.hpp"
/**
 * @brief Server Block
 * location block을 담고있다.
 * @details [location 블록을 찾는 규칙]
 * 1. 로케이션 블럭 중 가장 구체적인 블럭 부터 탐색합니다.
 *  - 만약 이때 밖에서 index가 있었다면 index를 붙힌 상태로 탐색을 진행합니다.
 *  - 모든 index에 대해서 진행합니다.
 * 2. 해당하는 블록을 찾았다면 그 블록안에 index가 있다면 다시 붙혀서 탐색합니다.
 *  - 그렇게 index를 붙혀서 탐색하다보면 결국 완전 일치하는 것을 찾습니다.
 * 3. 발견했다면 root를 붙혀서 파일을 접근해봅니다.
 *  - 만약 이때 파일이 존재하지 않는다면 다음 인자에 해당하는 index를 붙혀서 다시 탐색을 시작합니다.
 * @note [Server 블록 주요 인자] listen, server_name, upload_store (위쪽 블록에서 인자는 당연하게 상속됨)
 * @details [아래 listen과 server_name은 server block에만 들어갈 수 있습니다. (상속 안된다.)]
 * listen : default_server가 될수 있고 port번호를 이용해서 listen을 합니다.
    -(저희가 port번호를 기준으로 서버를 찾기 때문에 꼭 필요한 인자입니다.)
 *  -default_server : 이건 해당하는 포트로 처음 찾는 서버를 의미합니다.(다만, 우리 과제에서는 같은 포트를 사용하면 fail이기 때문에 큰 의미는 없다만 이대로 하겠습니다)
 * server_name : port번호가 일치한다면  http헤더에 host와 server_name과 비교합니다.
 * upload_store : 출력 파일을 저장할 디렉토리를 지정합니다. (server과 location)
 */
class ServBlock : public IBlock, public HttpBase{
public:
	ServBlock();
	virtual ~ServBlock();

	const std::string& getUploadStore()const;
	const std::vector<std::string>&  getServerName()const;
	const int& getListen()const;

	/*실제 사용할 경로를 찾아줄 getter*/
	std::string getCombineUploadStorePath()const;

	void	refineAll();
	LocBlock	findLocBlock(std::string path);
	void	printInfo()const;
	bool isErrorBlock()const;

	//사용자가 직접 호출할 일 없는 함수들 (for template)
	void	makeBlock(std::string line, std::ifstream& input, int& line_len_);
	std::map<std::string, std::string>& getDirStore();
private :
	int		untilFindLoc(const std::string& path, const std::string& root, const std::string& index);
	int 	locBlockSetUp(std::string &left_path, int ret_idx, std::string & root);
	void	makeLocBlock(std::ifstream& input, int& line_len_, std::string &block_name);
	void	makeOtherBlock(std::ifstream& input, int& line_lne_);
	void	parseServDirective();

	std::vector<LocBlock> loc_store_;
	std::vector<OtherBlock> other_store_;
	std::map<std::string, std::string> serv_directives_;

	std::string upload_store_;

	//only server member
	std::vector<std::string> server_name_;
	int listen_;
};

#endif

