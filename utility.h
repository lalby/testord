#ifndef UTILH_
#define UTILH_

#include<iostream>
#include<vector>
#include <sstream>
#include<string>
#include <memory>

namespace Order
{
  void split(const std::string &s,std::vector<char *> &tokens,const char delim=' ');
}

#endif
