#include <cassert>
#include <iostream>
#include "Orderbook.h"

// Each Test_* function sets up its own fresh Orderbook and asserts on
// exact expected values — no eyeballing printed numbers.

void Test_RestingOrderSitsInBook()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));

    assert(ob.Size() == 1);
    std::cout << "PASS: Test_RestingOrderSitsInBook\n";
}

void Test_CrossingOrderPartiallyFills()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 5));

    assert(trades.size() == 1);
    assert(trades[0].GetBidTrade().quantity_ == 5);
    assert(trades[0].GetAskTrade().quantity_ == 5);
    assert(ob.Size() == 1); // sell order still resting with 5 remaining

    std::cout << "PASS: Test_CrossingOrderPartiallyFills\n";
}

void Test_ExactFillRemovesBothOrders()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 10));

    assert(trades.size() == 1);
    assert(ob.Size() == 0); // both orders fully filled, nothing left resting

    std::cout << "PASS: Test_ExactFillRemovesBothOrders\n";
}

void Test_NonCrossingOrdersBothRest()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 10));
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 10));

    assert(trades.size() == 0); // bid 100 < ask 105, no match
    assert(ob.Size() == 2);

    std::cout << "PASS: Test_NonCrossingOrdersBothRest\n";
}

void Test_CancelRemovesOrder()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));
    ob.CancelOrder(1);

    assert(ob.Size() == 0);
    std::cout << "PASS: Test_CancelRemovesOrder\n";
}

void Test_CancelNonexistentOrderIsNoOp()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));
    ob.CancelOrder(999); // doesn't exist — should not throw or crash

    assert(ob.Size() == 1);
    std::cout << "PASS: Test_CancelNonexistentOrderIsNoOp\n";
}

void Test_FillAndKillCancelsIfNoMatch()
{
    Orderbook ob;
    // no resting sell orders at all, so a FillAndKill buy can't match and should vanish
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::FillAndKill, 1, Side::Buy, 100, 10));

    assert(trades.size() == 0);
    assert(ob.Size() == 0); // FillAndKill leaves nothing resting
    std::cout << "PASS: Test_FillAndKillCancelsIfNoMatch\n";
}

void Test_FillAndKillPartialFillThenCancelled()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 5));
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::FillAndKill, 2, Side::Buy, 100, 10));

    assert(trades.size() == 1);
    assert(trades[0].GetBidTrade().quantity_ == 5); // only 5 available to fill
    assert(ob.Size() == 0); // remaining 5 of the FillAndKill buy order gets cancelled, not left resting

    std::cout << "PASS: Test_FillAndKillPartialFillThenCancelled\n";
}

void Test_DuplicateOrderIdIsRejected()
{
    Orderbook ob;
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10));
    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5));

    assert(trades.size() == 0);
    assert(ob.Size() == 1); // second order with same ID was rejected, not added
    std::cout << "PASS: Test_DuplicateOrderIdIsRejected\n";
}

void Test_PriceTimePriority()
{
    Orderbook ob;
    // two sell orders at same price — order 1 was placed first, should fill before order 2
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 5));
    ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 100, 5));

    Trades trades = ob.AddOrder(std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Buy, 100, 5));

    assert(trades.size() == 1);
    assert(trades[0].GetAskTrade().orderId_ == 1); // FIFO: order 1 (placed first) fills, not order 2
    std::cout << "PASS: Test_PriceTimePriority\n";
}

int main()
{
    Test_RestingOrderSitsInBook();
    Test_CrossingOrderPartiallyFills();
    Test_ExactFillRemovesBothOrders();
    Test_NonCrossingOrdersBothRest();
    Test_CancelRemovesOrder();
    Test_CancelNonexistentOrderIsNoOp();
    Test_FillAndKillCancelsIfNoMatch();
    Test_FillAndKillPartialFillThenCancelled();
    Test_DuplicateOrderIdIsRejected();
    Test_PriceTimePriority();

    std::cout << "\nAll tests passed.\n";
    return 0;
}