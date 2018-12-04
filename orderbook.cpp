#include "orderbook.h"
std::pair<std::map<char,int>::iterator,bool> ret;

//use thread safe logger class instead of below
std::mutex outmutex;

void Order::OrderBook::add(int order_id, char side, double price, int size)
{
    LMTOrder Order(order_id,side,price,size);

    //map key is price and value is Order

    pairPriceOrder priceorderpair(price,Order);

    typedef std::pair<LMTOrder::orderid,MapIter> PairOrderidIterator;
	
    HashmapIter hmIter;
    MapIter mapIter; 
	
    std::pair<HashmapIter,bool> hashRet; 

    switch(side)
    {
       case 'B':
       {	
          std::lock_guard<std::mutex> lg_BidsMap_mutex(BidsMap_mutex);
          mapIter = bidsMap.insert(priceorderpair); 
           
          PairOrderidIterator ordiditpair(order_id,mapIter);
          std::lock_guard<std::mutex> lg_BidsHash_mutex(BidsHash_mutex);
          hashRet = bidsHash.insert(ordiditpair); 
			
          if (!hashRet.second) {
		  //thread safe logger class need to call instaed of below
		  std::lock_guard<std::mutex> lkg(outmutex);
		  std::cout<<"duplicate order id :Crtical"<<order_id<<std::endl;
                  //remove inserted 
                  bidsMap.erase(mapIter);
          }
          else
          {
               int count;
               LevelIter bidsLevIter=bidsLevelMap.find(price);
               if(bidsLevIter == bidsLevelMap.end())
               {
                 count=1;
                 pairPriceLevel pricelevelpair(price, count);
                 bidsLevelMap.insert(pricelevelpair); 

               }                
               else
               {
                 count= (*bidsLevIter).second;
                 bidsLevelMap.erase(bidsLevIter);
                  
                 pairPriceLevel pricelevelpair(price, ++count);
                 bidsLevelMap.insert(pricelevelpair); 
                          
               }

          }
            break;
        }
        case 'S':
        {			
	     std::lock_guard<std::mutex> lg_AsksMap_mutex(AsksMap_mutex);
       	     mapIter = asksMap.insert(priceorderpair); 

	     PairOrderidIterator ordiditpair(order_id,mapIter);
	     std::lock_guard<std::mutex> lg_AsksHash_mutex(AsksHash_mutex);
             hashRet = bidsHash.insert(ordiditpair); 
			
	     if (!hashRet.second)
	     {
		 //thread safe logger class need to call instaed of below
		 std::lock_guard<std::mutex> lkg(outmutex);
		 std::cout<<"duplicate order id"<<order_id;
	     }
             else
             { 
               int count;
               LevelIter asksLevIter=asksLevelMap.find(price);
               if(asksLevIter == asksLevelMap.end())
               {
                 count=1;
                 pairPriceLevel pricelevelpair(price, count);
                 asksLevelMap.insert(pricelevelpair); 

               }                
               else
               {
                 count= (*asksLevIter).second;
                 asksLevelMap.erase(asksLevIter);
                  
                 pairPriceLevel pricelevelpair(price, ++count);
                 asksLevelMap.insert(pricelevelpair); 
                          
               }

          }
             break;
	 }
     }
}


void Order::OrderBook::modify(int order_id, int new_size)
{
    char side='S';
    HashmapIter hmIter;
    MapIter mapIter; 
	
    { //for lock_guard
        //Side -B 
        std::lock_guard<std::mutex> lg_BidsHash_mutex(BidsHash_mutex);
        if ((hmIter = bidsHash.find(order_id)) != bidsHash.end()) 
	{ 
           side='B';			
           mapIter = hmIter->second;
			
           if (new_size) 
           { 
              mapIter->second.size=new_size;  
           }
           //new_size==0 condition not handle here 
	
         } 
     }
		
     if(side=='S')
     {
         std::lock_guard<std::mutex> lg_AsksHash_mutex(AsksHash_mutex);
         if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
         { 
             mapIter = hmIter->second;
			
             if (new_size) 
             { 
                mapIter->second.size=new_size;
             }
             //new_size==0 condition not handle here
         }
     }
}


void Order::OrderBook::remove(int order_id)
{
    char side='S';
    HashmapIter hmIter;
    MapIter mapIter; 

    {
       //Side -B
       BidsHash_mutex.lock();
       if ((hmIter = bidsHash.find(order_id) )!= bidsHash.end()) 
       { 
           side='B';			
           mapIter = hmIter->second;
           bidsHash.erase(hmIter);  

           int count;
           double price=(*mapIter).second.price;
           LevelIter bidsLevIter=bidsLevelMap.find(price);
           if(bidsLevIter != bidsLevelMap.end())
           {
               count= (*bidsLevIter).second;
               bidsLevelMap.erase(bidsLevIter);
               
               if(--count >0)    
               {
                 pairPriceLevel pricelevelpair(price, count);
                 bidsLevelMap.insert(pricelevelpair); 
               }
                          
           }

           BidsMap_mutex.lock();
           bidsMap.erase(mapIter);
           BidsMap_mutex.unlock();
         }
         BidsHash_mutex.unlock();
        
     }

    if(side == 'S')
    {
        AsksHash_mutex.lock();
        if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
        { 
            mapIter = hmIter->second;
            asksHash.erase(hmIter);

           int count;
           double price=(*mapIter).second.price;
           LevelIter asksLevIter=asksLevelMap.find(price);
           if(asksLevIter != asksLevelMap.end())
           {
               count= (*asksLevIter).second;
               asksLevelMap.erase(asksLevIter);
               
               if(--count >0)    
               {
                 pairPriceLevel pricelevelpair(price, count);
                 asksLevelMap.insert(pricelevelpair); 
               }
                          
           }


            AsksHash_mutex.unlock();

            AsksMap_mutex.lock();
            asksMap.erase(mapIter);
            AsksMap_mutex.unlock();
        }
        AsksHash_mutex.unlock();
     }
}

//get from the set
double Order::OrderBook::get_price(char side,int level)
{
    //MapIter mapiter; 
    
    switch(side)
    {
       case 'B':
       {			
           std::lock_guard<std::mutex> lg_BidsMap_mutex(BidsMap_mutex);
           int currlevel=1;
           //generally use upper levels
           LevelIter bidsleviter;
           
           bidsleviter=bidsLevelMap.begin();
           while(level > currlevel && bidsleviter !=bidsLevelMap.end())
           {
               ++currlevel;
               ++bidsleviter;
           }
           if(level==currlevel && bidsleviter !=bidsLevelMap.end()){
               return((*bidsleviter).first);}
 	
           break;
        }
        case 'S':
        {
           std::lock_guard<std::mutex> lg_AsksMap_mutex(AsksMap_mutex);
           int currlevel=1;
           //generally use upper levels
           LevelIter asksleviter; 
           asksleviter=asksLevelMap.begin();
           while(level > currlevel &&  asksleviter!=asksLevelMap.end())
           {
               ++currlevel;
               ++asksleviter;
           }
           if(level==currlevel &&  asksleviter!=asksLevelMap.end()){
               return((*asksleviter).first);}

           break;
         }

      }
	return -1; //failure 
}
	
int Order::OrderBook::get_size(char side, int level)
{
        
    MapIter mapiter; 
    std::pair<BidsMap::iterator,BidsMap::iterator> rangePair;

    switch(side)
    {
         case 'B':
         {
              std::lock_guard<std::mutex> lg_BidsMap_mutex(BidsMap_mutex);
              int currlevel=1;
              //generally use upper levels

              LevelIter bidsleviter;
              bidsleviter=bidsLevelMap.begin();

              while(level !=currlevel && bidsleviter !=bidsLevelMap.end())
              {
                  ++currlevel;
                  ++bidsleviter;
              }
              
              if(level==currlevel && bidsleviter !=bidsLevelMap.end())
              {
                  int totalsize=0;
                  rangePair=bidsMap.equal_range((*bidsleviter).first); 
                  MapIter mapiter; 
                  for(mapiter = rangePair.first ;mapiter !=rangePair.second;++mapiter)
                  {
                       totalsize+=(*mapiter).second.size;
                  }
                  return(totalsize);
              }	    
              break;

           }

           case 'S':
           {
               std::lock_guard<std::mutex> lg_AsksMap_mutex(AsksMap_mutex);
               int currlevel=1;
               //generally use upper levels
              
               LevelIter asksleviter;
               asksleviter=asksLevelMap.begin();

               while(level > currlevel && asksleviter !=asksLevelMap.end())
               {
                   ++currlevel;
                   ++asksleviter;
               }

               if(level==currlevel && asksleviter !=asksLevelMap.end())
               {
                   int totalsize=0;
                   rangePair=asksMap.equal_range((*asksleviter).first); 
                   for(mapiter = rangePair.first ;mapiter !=rangePair.second;++mapiter)
                   {
                       totalsize+=(*mapiter).second.size;
                   }
                   return totalsize;
               }	    
           }
           break;
	}
	return -1; //failure 
}

//singleton c++11
Order::OrderBook & Order::OrderBook:: getInstance()
{
	static OrderBook  OrderBookinst;
	return OrderBookinst;
}
