# testord
LALBY THOMAS
lalby2007@gmail.com
(C++,C++11/14 ,Python developer.Experience with Trading system -Risk control )
Simple OrderBook
===============
Objective : To achieve the scenarios mentioned the requirement.
Source code and test data path:
https://github.com/lalby/testord
Language used : C++ 11
Cmake version : 3.5.1
Compiler : g++ (ubuntu 5.4)
Design approach:
Singleton orderbook class is used to maintain :
a)Buy (highest bid first)
b)Sell (lowest ask first)
and corresponding functionality.
a)Maintain the order according to the price:
Buy and Sell separately monitored in two different ordered map <price,order> as price is the key
and the order is a structure having order details.
This two map differentiated by its map compare method(greater/lesser order).
Map is implemented internally as red black tree so this will take care of the order.
b)Improve the lookup to O(1) instead of map full iteration O(n)
Map lookup is O(log n) . To achieve faster lookup ,used the below approach.
Used an un_orderd map to keep orderid and lookup iterator value (received from (a)).
Un_ordered map internally implemented as hash .
Iter = multimap -BuyMap <- insert (price , Order object)
Un_ordered hash LookupMap <- insert( orderId,iter)
For search with orderid , get the iter value from the LookupMap and access directly.
c) Get the level
Each level may have multiple order and this should be keep in a sorted order .
To maintain the Level , used map<price,count variable to keep number of elements in the
particular level>
While insert element in the levelmap ,need to check if is already elements in the same price
level , in that case just increase the counter.
Before deleting any element , decrease the count .if count==0 remove the element from the
levelmap.
using the price we can get the size of same price from the ordermap (a)
These all map should be in sync.
Optimization:
Tried to optimize the code at different revisions.
Tried to use reference/pointer instead of copying objects .
Avoid using some good features of C++11 ( uniqe_ptr etc ) to improve the latency (specific to
the code used in this application) .
RAII features are looks more cleaner code but some scenarios its affects the latency.
Used C++11 chrono to check latency ( code is commented now, This latency check code
needs to move to under some TEST macro).
Working further to improve the latency.
Improvements:
a)Improve the locking mechanism using -shared_mutx c++17 .
b)Test using thread
c) Further optimization
d)Exception handling -optimized way
