#include<iostream>
#include "orderbook.h"

int main()
{

	Order::OrderBook &orderbook= Order::OrderBook::getInstance();
	orderbook.add(1,'B',45.2,100);
	orderbook.modify(1,50);
	std::cout<<orderbook.get_price('B',1);
	orderbook.add(2,'S',51.4,200);
	orderbook.add(3,'B',45.1,100);
	std::cout<<orderbook.get_size('S',1);
}
