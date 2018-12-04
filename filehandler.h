#ifndef FILEHANDLER_H
#define FILEHANDLER_H
 
#include<iostream>
#include "orderbook.h"
#include <fstream>
#include <string.h>
#include "utility.h"
#include <memory>
#include <algorithm>

/*
Acts as utility class to get the filename  and 
process the statements.

TODO: validation of the each input data received from text file.
atoi() function never throws exception 
boost<lexical_cast> better option .   
  
*/

class FileHandler
{ 
    std::ifstream infile;
    public:
    FileHandler(char *filename):infile(filename){
    } 
    bool isopen();   
    void process(Order::OrderBook &);
};
#endif
