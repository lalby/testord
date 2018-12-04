#include "orderbook.h"
std::pair<std::map<char,int>::iterator,bool> ret;

//use thread safe logger class instead of below
std::mutex outmutex;

void Order::OrderBook::add(int order_id, char side, double price, int size)
{
    LMTOrder Order(order_id,side,price,size);

    //map key is price and value is Order
    typedef std::pair<LimitOrder::loprice,LMTOrder> pairPriceOrder;
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
          bidsLevelSet.insert(price);
           
          PairOrderidIterator ordiditpair(order_id,mapIter);
          std::lock_guard<std::mutex> lg_BidsHash_mutex(BidsHash_mutex);
          hashRet = bidsHash.insert(ordiditpair); 
			
          if (!hashRet.second) {
		  //thread safe logger class need to call instaed of below
		  std::lock_guard<std::mutex> lkg(outmutex);
		  std::cout<<"duplicate order id :Crtical"<<order_id;
          }

            break;
        }
        case 'S':
        {			
	     std::lock_guard<std::mutex> lg_AsksMap_mutex(AsksMap_mutex);
       	     mapIter = asksMap.insert(priceorderpair); 
             asksLevelSet.insert(price);

	     PairOrderidIterator ordiditpair(order_id,mapIter);
	     std::lock_guard<std::mutex> lg_AsksHash_mutex(AsksHash_mutex);
             hashRet = bidsHash.insert(ordiditpair); 
			
	     if (!hashRet.second)
	     {
		 //thread safe logger class need to call instaed of below
		 std::lock_guard<std::mutex> lkg(outmutex);
		 std::cout<<"duplicate order id"<<order_id;
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
           //LMTOrder order((;
           bidsLevelSet.erase(mapIter->second.price);
           BidsHash_mutex.unlock();

           BidsMap_mutex.lock();
           bidsMap.erase(mapIter);
           BidsMap_mutex.unlock();
        }
	BidsHash_mutex.unlock();
     }

    if(side=='S')
    {
        AsksHash_mutex.lock();
        if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
        { 
            mapIter = hmIter->second;
            asksHash.erase(hmIter);
            asksLevelSet.erase(mapIter->second.price);
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
           BidsLevIter bidsleviter;
           
           bidsleviter=bidsLevelSet.begin();
           while(level > currlevel && bidsleviter !=bidsLevelSet.end())
           {
               ++currlevel;
               ++bidsleviter;
           }
           if(level==currlevel && bidsleviter !=bidsLevelSet.end()){
               return(*bidsleviter);}
 	
           break;
        }
        case 'S':
        {
           std::lock_guard<std::mutex> lg_AsksMap_mutex(AsksMap_mutex);
           int currlevel=1;
           //generally use upper levels
           AsksLevIter asksleviter; 
           asksleviter=asksLevelSet.begin();
           while(level > currlevel &&  asksleviter!=asksLevelSet.end())
           {
               ++currlevel;
               ++asksleviter;
           }
           if(level==currlevel &&  asksleviter!=asksLevelSet.end()){
               return(*asksleviter);}

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

              BidsLevIter bidsleviter;
              bidsleviter=bidsLevelSet.begin();

              while(level !=currlevel && bidsleviter !=bidsLevelSet.end())
              {
                  ++currlevel;
                  ++bidsleviter;
              }
              
              if(level==currlevel && bidsleviter !=bidsLevelSet.end())
              {
                  int totalsize=0;
                  rangePair=bidsMap.equal_range(*bidsleviter); 
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
              
               AsksLevIter asksleviter;
               asksleviter=asksLevelSet.begin();

               while(level > currlevel && asksleviter !=asksLevelSet.end())
               {
                   std::cout<<"Level"<<currlevel <<" "<<*asksleviter;  
                   ++currlevel;
                   ++asksleviter;
               }

               if(level==currlevel && asksleviter !=asksLevelSet.end())
               {
                   int totalsize=0;
                   rangePair=asksMap.equal_range(*asksleviter); 
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
