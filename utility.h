#ifndef UTILH_
#define UTILH_

#include<iostream>
#include<vector>
#include <sstream>
#include<string>
#include <memory>


void split(const std::string &s,std::vector<char *> &tokens,const char delim=' ')
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

#endif
