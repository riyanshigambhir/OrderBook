#pragma once

enum class OrderType
{
    GoodTillCancel, //sits in order till fully filled or explicitly cancelled. Most limit orders placed will be GTC.
    FillAndKill, //aka ImmediateOrCancel. If it can be filled immediately, it will be filled. If not, it will be cancelled. No resting order is left in the book.
};

//Other types for future scope: 
//Good-For-Day (GFD) - sits in order till end of day or fully filled or explicitly cancelled.
//Post-Only (PO) - will only post to the book, and will not match with any existing orders. If it would match, it is cancelled instead.
//How to add new order type - type specific branching logic lives in AddOrder / MatchOrder / CancelOrder. Add a new branch for the new type in those functions, and add a new enum value here.

enum class Side
{
    Buy,
    Sell,
}; //used throughtout as the branch condiiton determinign whether to add to bids or asks, and which side of the trade is the maker/taker.
