#ifndef CGI_HPP
# define CGI_HPP
# include "../UData/HttpRequest/HttpRequest.hpp"
# include "../UData/UData.hpp"
# include "Kqueue.hpp"
# include <iostream>
# include <unistd.h>
# include <fcntl.h>

class Cgi{
  public:
    static void   forkCgi(HttpRequest& req);
  private:
    static char** getEnvs(HttpRequest& req);
};

#endif