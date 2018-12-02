
#ifndef ORDERBOOK
#define ORDERBOOK

#include<iostream>
#include<map>
#include<mutex>

namespace Order
{

	typedef struct LimitOrder
	{
		typedef int orderid;
		typedef char loside;
		typedef double loprice;
		typedef int	losize;

	private:
	
		orderid order_id;
		loside side;
		loprice price;
		losize size;

	public:

		LimitOrder(orderid f_orderid,loside f_side,loprice f_price,losize f_size)
			:order_id(f_orderid),side(f_side),price(f_price),size(f_size) {}

	}LMTOrder;


	class OrderBook {
		public :
		void add(int order_id, char side, double price, int	size);
		void modify(int order_id, int new_size);
		void remove(int order_id);
		double get_price(char side, int	level);
		int get_size(char side, int level);

		OrderBook(){}
		~OrderBook() {}
		OrderBook(OrderBook const &) = delete ;
		void operator=(OrderBook const &) = delete;
		
		//Meyers singleton
		static OrderBook& getInstance();


		private:

			char getside(int);

			typedef std::multimap<LimitOrder::loprice,Order,std::greater<LimitOrder::loprice> > BidsMap;
			typedef std::multimap<LimitOrder::loprice,Order,std::less<LimitOrder::loprice> > AsksMap;
		
			typedef std::multimap<LimitOrder::loprice,Order>::iterator MapIter;
			typedef std::multimap<LimitOrder::loprice,Order>::reverse_iterator MapReverseIter;
			typedef std::unordered_map<LimitOrder::orderid,MapIter>::iterator HashmapIter;
			
			typedef std::unordered_map<LimitOrder::orderid,MapIter> BidsHash;
			typedef std::unordered_map<LimitOrder::orderid,MapIter> AsksHash;

			BidsMap bidsMap; 
			AsksMap asksMap;

			BidsHash bidsHash; 
			AsksHash asksHash;

			trading::MarketOrder::Size openBids; 
			trading::MarketOrder::Size openAsks; 

			std::mutex BidsMap_mutex;
			std::mutex AsksMap_mutex;

			std::mutex BidsHash_mutex;
			std::mutex AsksHash_mutex;

			std::mutex BidsMap_sharedmutex;
			std::mutex AsksMap_sharedmutex;

			std::mutex BidsHash_sharedmutex;
			std::mutex AsksHash_sharedmutex;
	};
}
#endif
