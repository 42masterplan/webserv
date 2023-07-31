#include "../interface/UData.hpp"

typedef enum parseStatus{
	START,
	FIRST,
	HEADERS,
	BODY,
	FIN
}e_parse;

class HttpParser {
	public :
		void parse(char* buff, size_t len, UData u_data);
	private :

};