#include <iostream>
#include "Orderbook.h"

int main()
{
    Orderbook orderbook;

    // basic smoke test: add a resting sell, then a crossing buy
    const OrderId order1Id = 1;
    orderbook.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, order1Id, Side::Sell, 100, 10));
    std::cout << "Book size after resting sell: " << orderbook.Size() << "\n"; // expect 1

    const OrderId order2Id = 2;
    Trades trades = orderbook.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, order2Id, Side::Buy, 100, 5));
    std::cout << "Trades executed: " << trades.size() << "\n"; // expect 1
    std::cout << "Book size after partial match: " << orderbook.Size() << "\n"; // expect 1 (sell has 5 remaining)

    orderbook.CancelOrder(order1Id);
    std::cout << "Book size after cancel: " << orderbook.Size() << "\n"; // expect 0

    return 0;
}
