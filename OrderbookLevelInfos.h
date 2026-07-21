#pragma once

#include "Usings.h"

class OrderbookLevelInfos
{
public:
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
        : bids_{ bids }
        , asks_{ asks }
    { }

    const LevelInfos& GetBids() const { return bids_; }
    const LevelInfos& GetAsks() const { return asks_; }
    //names class preferred over pair so its self documenting and easier to read in code

private:
    LevelInfos bids_;
    LevelInfos asks_;
};
