#include<iostream>
#include "orderbook.h"
#include <fstream>
#include <string.h>
#include "utility.h"


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
		std::ifstream infile(argv[1]);
        if(!infile.is_open())
		{
	        std::cout<<"could not open file "<<argv[1];
            return 0;				
		}			                
 
		std::string line;
           
		char action[30]={0};
		std::vector<char *> tokens;

		while(getline(infile,line))
		{
            //std::cout<<line<<"\n";
            memset(action,0,sizeof(action));
            tokens.clear();
             
            split(line,tokens);
            strncpy(action,tokens[0],sizeof(action)-1);

			for(auto s : tokens)
			std::cout<<"  "<<s;
                        std::cout<<"\n"; 
             
			if(strcmp(action,"add")==0)
			{
				orderbook.add(atoi(tokens[1]),(*tokens[2]),atof(tokens[3]),atoi(tokens[4]));
			}				
		    else if(strcmp(action,"modify")==0)
			{
				orderbook.modify(atoi(tokens[1]),atoi(tokens[2]));
			}
			else if(strcmp(action,"remove")==0)
			{
				orderbook.remove(atoi(tokens[1]));	
			}
			else if(strcmp(action,"get")==0)
			{
				if(strcmp(tokens[1],"price")==0)
				{
					std::cout<<orderbook.get_price(*tokens[2],atoi(tokens[3]))<<"\n";
				}
				else if(strcmp(tokens[1],"size")==0)
				{
					std::cout<<orderbook.get_size(*tokens[2],atoi(tokens[3]))<<"\n";
				}
			}

		  }
		infile.close();
        } catch(...)
         {
			std::cout<<"file operation failed:";
         }                       

          
    }			
	/*	
	orderbook.add(1,'B',45.2,100);
	orderbook.modify(1,50);
	std::cout<<orderbook.get_price('B',1);
	orderbook.add(2,'S',51.4,200);
	orderbook.add(3,'B',45.1,100);
	std::cout<<orderbook.get_size('S',1);
    */
}
