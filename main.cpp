#include<iostream>
#include "orderbook.h"
#include "filehandler.h"
#include<thread>

//usage :: ./orderbook inputfile 

int main(int argc,char *argv[])
{

    Order::OrderBook &orderbook= Order::OrderBook::getInstance();

    if(argc<2)
    {
    	std::cout<<"invalid aruments usage : ./order filename ";	
   		return 0;
    }
    else
    {
        try
        {	
            /* TODO : Test program for thread    
            std::vector<std::string> flist ={"inputfile1","inputfile2","inputfile3"};
            FileHandler fh[10];
            int i=0; 
            for(auto f : flist)
            {
               fh[i++].set(f);
               //uset static  
               std::thread(fh[i].process,(orderbook));
            }
            */ 
            FileHandler fh(argv[1]);
            fh.process(orderbook);
        }
        catch(...)
        {
            std::cout<<"error canot process!!";
        } 
    } 
}
