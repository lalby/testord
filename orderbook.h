
#ifndef ORDERBOOK
#define ORDERBOOK

#include<iostream>
#include<map>
#include<unordered_map>
#include<mutex>
#include<set>
//#include<shared_mutex>

/*Simple Order book
Singleton orderbook class is used to maintain  
a)Buy (highest bid first)
b)Sell (lowest ask first)
Buy,Sell separately monitored in two differnt ordered map <price,order> as price is the key.
This two map differentiated by its map compare method(greater/lesser order).
Map is implemented internaly as redblack tree so this will take care of the order.  
Map lookup is O(log n) to achieve faster lookup used the below approach.

For faster lookup (O(1)) used an un_orderd map to keep orderid and lookup iterator value in the multimap.

LevelMap:

To maintain the Level , used map<price,count variable to keep number of elements in the particular level>
while insert element in the levelmap ,need to check already elements in the same price lvel just increase the counter.
Before delete any element , decrease the count .if  count==0 remove the element from the levelmap 
Price value of the particular level get from levelmap and find the elements in the mainmap. 

These all map should be in sync.

TODO:improve the locking mechanism using -shared_mutx c++17 and multithread testing is pending.

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
	//char getside(int);

        typedef std::pair<LimitOrder::loprice,LMTOrder> pairPriceOrder;
        typedef std::pair<LimitOrder::loprice,int> pairPriceLevel;

	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::greater<LimitOrder::loprice> > BidsMap;
	typedef std::multimap<LimitOrder::loprice,LMTOrder,std::less<LimitOrder::loprice> > AsksMap;
		
	typedef std::multimap<LimitOrder::loprice,LMTOrder>::iterator MapIter;
	typedef std::unordered_map<LimitOrder::orderid,MapIter>::iterator HashmapIter;
	
	typedef std::unordered_map<LimitOrder::orderid,MapIter> BidsHash;
	typedef std::unordered_map<LimitOrder::orderid,MapIter> AsksHash;


        typedef std::map<LimitOrder::loprice,int,std::greater<LimitOrder::loprice>> BidsLevelMap;  
        typedef std::map<LimitOrder::loprice,int,std::less<LimitOrder::loprice>> AsksLevelMap;  

        typedef std::map<LimitOrder::loprice,int>::iterator  LevelIter;  



        // to store order
	BidsMap bidsMap; 
	AsksMap asksMap;

        BidsLevelMap bidsLevelMap; 
        AsksLevelMap asksLevelMap;

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
