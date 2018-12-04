#ifndef FILEHANDLER_H
#define FILEHANDLER_H
 
#include<iostream>
#include "orderbook.h"
#include <fstream>
#include <string.h>
#include "utility.h"
#include <memory>
#include <algorithm>

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
