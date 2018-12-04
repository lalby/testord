#include <iostream>
#include "utility.h"
#include <cstring>


void Order::split(const std::string &s,std::vector<char *> &tokens,const char delim)
{
    std::stringstream ss(s);
    std::string token;
    while(getline(ss,token,delim))
    {
       char *temp=new char(token.size()+1);
       strncpy(temp,token.c_str(),token.length()+1);
       tokens.push_back(temp);
    }

}		

