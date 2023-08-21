#ifndef I_BLOCK_HPP
# define I_BLOCK_HPP
# include "../../Tools/ParseTool.hpp"
class IBlock
{
public:
		virtual void	makeBlock(std::string line, std::ifstream& input, int& line_len) = 0;
		virtual std::map<std::string, std::string>& getDirStore() = 0;
};

#endif
