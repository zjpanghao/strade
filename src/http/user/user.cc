
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

// #include "client/linux/handler/exception_handler.h"

//#if defined (FCGI_STD)
//#include <fcgi_stdio.h>
//#elif defined(FCGI_PLUS)
//#include <fcgio.h>
//#include <fcgi_config.h>
//#endif

#include "base/fcgimodule.h"
#include "logic/comm_head.h"
// 设置请求类型



int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  std::string core_sock_file = "/var/www/tmp/strade_user";
  fcgi_client.Init(core_sock_file, USER_API, 5);
  fcgi_client.Run();
  fcgi_client.Close();
  return 0;
}
