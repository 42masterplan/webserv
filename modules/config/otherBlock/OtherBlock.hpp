#ifndef OTHERBLOCK_HPP
# define OTHERBLOCK_HPP
# include "../../Tools/ParseTool.hpp"
# include "../interface/IBlock.hpp"
/**
 * @brief 저희는 http, server location 블록을 제외하면 다른 블록도 블록화 해서 담아줍니다.
 *
 * @note otherblock안에는 otherblock만 올 수 있습니다.
 */
class OtherBlock : public IBlock{
	public :
		OtherBlock();
		virtual ~OtherBlock();

		std::map<std::string, std::string>& getDirStore();
		void	makeBlock(std::string line, std::ifstream& input, int& line_len_);

	private :
		void	makeOtherBlock(std::ifstream& input, int& line_len_);
		std::vector<OtherBlock> other_store_;
		std::map<std::string, std::string> other_directives_;
		std::string block_name_;
};

#endif
