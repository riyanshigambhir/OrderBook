#pragma once

#include "Usings.h"
#include "OrderType.h"
#include "Order.h"

class OrderModify
{
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
        : orderId_{ orderId }
        , side_{ side }
        , price_{ price }
        , quantity_{ quantity }
    { }
    //"modify" is implemented as cancel the old order, 
    //then add a brand-new order with the new price/quantity. 

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    Quantity GetQuantity() const { return quantity_; }

    OrderPointer ToOrderPointer(OrderType orderType) const
    {
        return std::make_shared<Order>(orderType, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};

//consequence of the current design:
//cancel-and-replace loses queue priority
//Since a "modify" is really cancel-then-re-add, a modified order goes to the back of the queue at its price level, 
//even if only the quantity changed and the price stayed the same.
// Real exchanges often treat this differently 
//e.g., some allow a quantity-decrease to keep your place in line, but any price change or quantity-increase loses priority. 
//improvement scope
//special-case ModifyOrder so a pure quantity decrease mutates remainingQuantity_ in place instead of cancel/re-add.