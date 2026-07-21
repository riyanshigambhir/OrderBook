#pragma once

#include <memory>
#include <list>
#include <format>
#include <stdexcept>

#include "OrderType.h"
#include "Usings.h"

class Order
{
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
        : orderType_{ orderType }
        , orderId_{ orderId }
        , side_{ side }
        , price_{ price }
        , initialQuantity_{ quantity }
        , remainingQuantity_{ quantity } //as order gets filled remaining quantity shrinks while initial quantity stays fixed as a historical record
    { }

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    OrderType GetOrderType() const { return orderType_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }
    Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }
    bool IsFilled() const { return GetRemainingQuantity() == 0; }

    void Fill(Quantity quantity)
    {
        if (quantity > GetRemainingQuantity()) //if invariant remaining quantity goes negative, it would silently wrap around to a large positive number since remaining quantity is unsigned, so we throw an exception to avoid bug
            throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity.", GetOrderId()));

        remainingQuantity_ -= quantity;
    }

private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>; 
//Orders must be passes around by pointed as the smae order needs to be references from multiple paces simultaneously. 
using OrderPointers = std::list<OrderPointer>;
//std::list is a doubly linked list, orders can be inserted and removed from the middle of the list with O(1) complexity. 
//iterators to untouched nodes stay valid forever
//std::vector is most cache friendly for iterators (insertion can reallocate and invalidate every iterator), but inserting and removing from the middle of a vector is O(n) complexity.
//iterator could silently become garbage the next time any order got added anywhere
//future scope - a production book might use an intrusive linked list or a flat array with stable indices instead, to get O(1) cancel and cache locality. 
