#include "HttpRequest.hpp"

int main(){
	HttpRequest h2;
	std::vector<std::string> svec;
	std::string a1= "GET / HTTP/1.1\r\nHost: localhost:80\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n";
	svec.push_back(a1);
	a1 = "POST / HTTP/1.1\r\nHost: localhost:80\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n";
	svec.push_back(a1);
	a1 = "\r\n5\r\nhihel\r\n8\r\n12345678\r\n";
	svec.push_back(a1);
	a1 = "0\r\n\r\n";
	svec.push_back(a1);
	a1 =  "POST / HTTP/1.1\r\nHost: localhost:80\r\nUser-Agent: Go-http-client/1.1\r\nContent-Length: 8\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\nabcdefgh\r\n";
	svec.push_back(a1);
	std::vector<char> store;
	for (size_t i = 0; i < 5;i++){
		std::cout << i <<":::------------------------------------------" << std::endl;
		store.insert(store.end(), svec[i].c_str(), svec[i].c_str() + svec[i].size());
		// print_vec(store);
		h2.parse(store);
		h2.printBodyInfo();
	}
}
