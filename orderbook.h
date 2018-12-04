
#ifndef ORDERBOOK
#define ORDERBOOK

#include<iostream>
#include<map>
#include<unordered_map>
#include<mutex>
#include<set>
//#include<shared_mutex>

/*Simple Order book
Singleton orderbook class 
two multimap for 
a)Buy (highest bid first)
b)Sell (lowest ask first)
For faster lookup (O(1)) used  un_orderd map to keep orderid and lookup iterator value in the multimap(lookup O(log(n).
Mutex mechanism can be further improved by shared_mutex C++17
*/

namespace Order
{
    typedef struct LimitOrder
    {
	 typedef int orderid;
	 typedef char loside;
	 typedef double loprice;
	 typedef int	losize;

	 orderid order_id;
	 loside side;
	 loprice price;
	 losize size;

	 LimitOrder(orderid f_orderid,loside f_side,loprice f_price,losize f_size)
		:order_id(f_orderid),side(f_side),price(f_price),size(f_size) {}
     ~LimitOrder(){} 

     }LMTOrder;


    class OrderBook {
	public :
        //add a new order with uniqueid  
	void add(int order_id, char side, double price, int size);
	void modify(int order_id, int new_size);
	void remove(int order_id);
	double get_price(char side, int	level);
	int get_size(char side, int level);

	OrderBook(){}
	~OrderBook() {}
	OrderBook(OrderBook const &) = delete ;
	void operator=(OrderBook const &) = delete;
		
	//singleton
	static OrderBook& getInstance();

	private:
	char getside(int);
	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::greater<LimitOrder::loprice> > BidsMap;
	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::less<LimitOrder::loprice> > AsksMap;
		
	typedef std::multimap<LimitOrder::loprice,LMTOrder>::iterator MapIter;
	typedef std::unordered_map<LimitOrder::orderid,MapIter>::iterator HashmapIter;
	
	typedef std::unordered_map<LimitOrder::orderid,MapIter> BidsHash;
	typedef std::unordered_map<LimitOrder::orderid,MapIter> AsksHash;

        typedef std::set<LimitOrder::loprice,std::greater<LimitOrder::loprice>> BidsLevelSet;  
        typedef std::set<LimitOrder::loprice> AsksLevelSet;  

        typedef std::set<LimitOrder::loprice,std::greater<LimitOrder::loprice>> ::iterator BidsLevIter;  
        typedef std::set<LimitOrder::loprice> ::iterator AsksLevIter;  

        // to store order
	BidsMap bidsMap; 
	AsksMap asksMap;

        BidsLevelSet bidsLevelSet; 
        AsksLevelSet asksLevelSet;

        //Map to store iterator for faster lookup
        //Order map and Hashmap should be in sync.

	BidsHash bidsHash; 
	AsksHash asksHash;

	std::mutex BidsMap_mutex;
	std::mutex AsksMap_mutex;

	std::mutex BidsHash_mutex;
	std::mutex AsksHash_mutex;

	};
}
#endif
