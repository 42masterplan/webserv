#ifndef IBLOCK_HPP
# define IBLOCK_HPP
# include "ParseTool.hpp"
class IBlock
{
public:
		virtual void	makeBlock(std::string line, std::ifstream& input, int& line_len) = 0;
		virtual std::map<std::string, std::string>& getDirStore() = 0;
};

#endif
