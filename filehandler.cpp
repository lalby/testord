#include<iostream>
#include "filehandler.h"
#include "utility.h"
#include <chrono>

bool FileHandler::isopen()   
{
   if(!infile.is_open())
       return false;
    else
      return true; 
}   
    
void FileHandler::process(Order::OrderBook &orderbook)
{
    //std::chrono::high_resolution_clock::time_point t1=std::chrono::high_resolution_clock::now(); 
       
    if(isopen())
    {  
        std::vector<char *> tokens;
         
        try
        {	
           std::string line;
           char action[30]={0};
		
           //std::vector<std::unique_ptr<char>> tokens;
           //avoiding due to latency each time need to create in the loop

          //TODO - validation of line  
                
           while(getline(infile,line))
           {
              memset(action,0,sizeof(action));
             
              Order::split(line,tokens);

              strncpy(action,tokens[0],sizeof(action)-1);
            
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
          
              for_each(tokens.begin(),tokens.end(),[](char *p) {
                      if(*p) delete p;} 
              );			
              tokens.clear();

           }
        infile.close();
        } catch(...)
        {
              std::cout<<"operation failed:";
              //not using smart pointers - so clear here also incase of exception.
              for_each(tokens.begin(),tokens.end(),[](char *p) {
                      if(*p) delete p;} 
              );			
              tokens.clear();

        }                       

    }			

   
    //std::chrono::high_resolution_clock::time_point t2=std::chrono::high_resolution_clock::now(); 
    //std::chrono::duration<double> time_span =std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);  
    //std::cout<<"time taken:" <<time_span.count();
};
