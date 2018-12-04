#include<iostream>
#include "orderbook.h"
#include "filehandler.h"

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
            FileHandler fh(argv[1]);
            fh.process(orderbook);
        }
        catch(...)
        {
            std::cout<<"error canot process!!";
        } 
    } 
}
