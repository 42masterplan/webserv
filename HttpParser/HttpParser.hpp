#include "../interface/UData.hpp"
#include <algorithm>

typedef enum parseStatus{
	FIRST,
	HEADER,
	BODY,
	FIN
}e_parse;

class HttpParser {
	public :
		void parse(char* buff, size_t len, UData& u_data) const ;
	private :
		size_t				findCRLF(const std::vector<char>& raw_data) const ;
		std::string&	getTarget(std::string& line) const ;
		void					parseFirstLine(std::string& line, UData& u_data) const ;
		bool					parseHeader(std::string& line, UData& u_data) const ;

};