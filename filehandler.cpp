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
                
           while(getline(infile,line))
           {
              memset(action,0,sizeof(action));
             
              Order::split(line,tokens);

              strncpy(action,tokens[0],sizeof(action)-1);
            
              if(strcmp(action,"add")==0)
              {
                  if(tokens.size() >=5 && (*tokens[2]=='B' || *tokens[2]=='S')) 
                  { 
                     orderbook.add(std::stoi(tokens[1]),(*tokens[2]),std::stod(tokens[3]),std::stoi(tokens[4]));
                  }
              }				
              else if(strcmp(action,"modify")==0)
              {
                  if(tokens.size()>=3 )  
                  orderbook.modify(std::stoi(tokens[1]),std::stoi(tokens[2]));
              }
              else if(strcmp(action,"remove")==0)
              {
                  if(tokens.size()>=2)
                  orderbook.remove(std::stoi(tokens[1]));	
              }
              else if(strcmp(action,"get")==0)
              {
                  if(tokens.size()>=4) {
                  if(strcmp(tokens[1],"price")==0)
                  {
                      std::cout<<orderbook.get_price(*tokens[2],std::stoi(tokens[3]))<<"\n";
                  }
                  else if(strcmp(tokens[1],"size")==0)
                  {
                       std::cout<<orderbook.get_size(*tokens[2],std::stoi(tokens[3]))<<"\n";
                  }}
               }
          
              for_each(tokens.begin(),tokens.end(),[](char *p) {
                      if(*p) delete p;} 
              );			
              tokens.clear();

           }
        infile.close();
        } catch(...)
        {
              std::cout<<"some operation failed/ignored -may be incorrect input data ";
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

