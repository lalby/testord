#include "orderbook1.h"
std::pair<std::map<char,int>::iterator,bool> ret;


//use thread safe logger class instead of below
std::mutex outmutex;


void Order::OrderBook::add(int order_id, char side, double price, int	size)
{
	LMTOrder Order(order_id,side,price,size);
	
	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::greater<LimitOrder::loprice> > BidsMap;
	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::less<LimitOrder::loprice> > AsksMap;
	

	typedef std::pair<LimitOrder::loprice,LMTOrder> pairPriceOrder;
	pairPriceOrder priceorderpair(price,Order);

	// for hashmap
	typedef std::pair<LMTOrder::orderid,MapIter> PairOrderidIterator;
	
	HashmapIter hmIter;
	MapIter mapIter; 
	
	std::pair<HashmapIter,bool> hashRet; 

	switch(side)
	{
		case 'B':
			
			std::lock_guard<BidsMap_mutex> lg_BidsMap_mutex;
			mapIter = bidsMap.insert(priceorderpair); 
			PairOrderidIterator ordiditpair(order_id,mapIter);
			std::lock_guard<BidsHash_mutex> lg_BidsHash_mutex;
			hashRet = bidsHash.insert(ordiditpair); 
			
			if (!hashRet.second) {
				//thread safe logger class need to call instaed of below
				std::lock_guard<std::mutex> lkg(outmutex);
				std::cout<<"duplicate order id"<<order_id;
			}


			break;

		case 'S':
			
			std::lock_guard<AsksMap_mutex> lg_AsksMap_mutex;
			mapIter = asksMap.insert(priceorderpair); 
			PairOrderidIterator ordiditpair(order_id,mapIter);
			std::lock_guard<AsksHash_mutex> lg_AsksHash_mutex;
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


char Order::OrderBook::getside(int order_id)
{
	char side='U';
	{
		std::lock_guard<BidsHash_mutex> lg_BidsHash_mutex;
		if ((hmIter = bidsHash.find(order_id)) != bidsHash.end()) 
		{ 
			side='B';
		}
	}
	if(side == 'U' )
	{
		std::lock_guard<AsksHash_mutex> lg_AsksHash_mutex;
		if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
		{ 
			side='S';
		}

	}

	return side;
}

void Order::OrderBook::modify(int order_id, int new_size)
{
	char side='S';
	
	{
		//Side -B 
		std::lock_guard<BidsHash_mutex> lg_BidsHash_mutex;
		if ((hmIter = bidsHash.find(order_id)) != bidsHash.end()) 
		{ 
			side='B';			
			mapIter = hmIter->second;
			Order& orderFromMap = mapIter->second;
			
			if (new_size) 
			{ 
				orderFromMap.size=new_size;
			}

			//new_size==0 condition not handle here 
	
		 } 
	}
		
	if(side=='S')
	{
		std::lock_guard<AsksHash_mutex> lg_AsksHash_mutex;
		if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
		{ 
			mapIter = hmIter->second;
			Order& orderFromMap = mapIter->second;
			
			if (new_size) 
			{ 
				orderFromMap.size=new_size;
			}

			//new_size==0 condition not handle here
		}
	}
}


void Order::OrderBook::remove(int order_id)
{
	char side='S';

	{
		//Side -B
		BidsHash_mutex.lock()
		if ((hmIter = bidsHash.find(order_id)) != bidsHash.end()) 
		{ 
			side='B';			
			mapIter = hmIter->second;
			bidsHash.erase(hmIter);  
			BidsHash_mutex.unlock()

			BidsMap_mutex.lock();
			bidsMap.erase(mapIter);
			BidsMap_mutex.unlock();
		}
		BidsHash_mutex.unlock()
	}

	if(side=='S')
	{
		AsksHash_mutex.lock();
		if ((hmIter = asksHash.find(order_id)) != asksHash.end()) 
		{ 
			mapIter = hmIter->second;
			asksHash.erase(hmIter);
			AsksHash_mutex.unlock();

			AsksMap_mutex.lock();
			AsksMap.erase(mapIter);
			AsksMap_mutex.unlock();
		}
		AsksHash_mutex.unlock();
	}
}


double Order::OrderBook::get_price(char side,int level)
{
	switch(side)
	{
		case 'B':
			std::lock_guard<BidsMap_mutex> lg_BidsMap_mutex;
			if(BidsMap.size() > level)
			{
				//map index access is faster
				return(BidsMap[level].first);
			}
			break;
		case 'S':
			std::lock_guard<AsksMap_mutex> lg_AsksMap_mutex;
			if(AsksMap.size() > level)
			{
				return(AsksMap[level].first);
			}
			break;

	}
	return -1; //failure 
}
	
int Order::OrderBook::get_size(char side, int level)
{
	switch(side)
	{
		case 'B':
			{
				std::lock_guard<BidsMap_mutex> lg_BidsMap_mutex;
				if(BidsMap.size() > level)
				{
					
					//map index access is faster
					return(BidsMap[level].second.size);
				}
			}
			break;
		case 'S':
			{
				std::lock_guard<AsksMap_mutex> lg_AsksMap_mutex;
				if(AsksMap.size() > level)
				{
					return(AsksMap[level].second.size);
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