#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include "../../config/LocationBlock/LocBlock.hpp"
# include "../UData.hpp"

class HttpResponse{

	public :
		static std::map<int, std::string> status_store_;
		
		/* constructor */
		HttpResponse();
		void		makeResponse(HttpRequest &request);

		/* methods */
		void 		initStatusStore(void);
		void 		processDefaultErrorRes(HttpResponse &res, int status_code);
		void 		processRedirectRes(HttpResponse &res, int status_code);
		const std::vector<char>& getJoinedData()const;
	private :
		std::string 			http_version_;
		int								status_code_;
		std::string				status_;
		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;

		std::vector<char>	body_;
		std::vector<char>	joined_data_;
		LocBlock					loc_block_;
};

#endif
