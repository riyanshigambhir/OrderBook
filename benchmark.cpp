#include "Orderbook.h"
#include <chrono>
#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>

// Synthetic order flow generator + throughput/latency benchmark for the
// limit order book. Measures per-order latency (not just aggregate
// throughput) since tail latency matters as much as average speed for
// a matching engine.

int main()
{
    Orderbook orderbook;

    constexpr int NUM_ORDERS = 100'000;
    constexpr Price MID_PRICE = 10000;   // e.g. represents 100.00 in ticks
    constexpr Price SPREAD_RANGE = 50;   // orders generated within +/- this of mid
    constexpr Quantity MAX_QTY = 100;

    std::mt19937 rng(42); // fixed seed for reproducibility
    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<Price> priceDist(MID_PRICE - SPREAD_RANGE, MID_PRICE + SPREAD_RANGE);
    std::uniform_int_distribution<Quantity> qtyDist(1, MAX_QTY);

    std::vector<OrderPointer> orders;
    orders.reserve(NUM_ORDERS);

    for (int i = 0; i < NUM_ORDERS; ++i)
    {
        Side side = sideDist(rng) == 0 ? Side::Buy : Side::Sell;
        Price price = priceDist(rng);
        Quantity qty = qtyDist(rng);

        orders.push_back(std::make_shared<Order>(
            OrderType::GoodTillCancel,
            static_cast<OrderId>(i + 1), // order IDs start at 1 (0 reserved/unused)
            side,
            price,
            qty
        ));
    }

    std::cout << "Generated " << NUM_ORDERS << " synthetic orders.\n";
    std::cout << "Running throughput + latency benchmark...\n";

    std::vector<double> latenciesNs;
    latenciesNs.reserve(NUM_ORDERS);

    std::size_t totalTrades = 0;

    const auto overallStart = std::chrono::high_resolution_clock::now();

    for (auto& order : orders)
    {
        const auto orderStart = std::chrono::high_resolution_clock::now();
        auto trades = orderbook.AddOrder(order);
        const auto orderEnd = std::chrono::high_resolution_clock::now();

        latenciesNs.push_back(
            std::chrono::duration<double, std::nano>(orderEnd - orderStart).count());

        totalTrades += trades.size();
    }

    const auto overallEnd = std::chrono::high_resolution_clock::now();

    // ---- Throughput ----
    const auto elapsedMs = std::chrono::duration<double, std::milli>(overallEnd - overallStart).count();
    const double ordersPerSec = NUM_ORDERS / (elapsedMs / 1000.0);

    // ---- Latency percentiles ----
    std::sort(latenciesNs.begin(), latenciesNs.end());

    auto percentile = [&](double p) -> double
    {
        std::size_t idx = static_cast<std::size_t>(p * (latenciesNs.size() - 1));
        return latenciesNs[idx];
    };

    const double minLatency = latenciesNs.front();
    const double maxLatency = latenciesNs.back();
    const double avgLatency = std::accumulate(latenciesNs.begin(), latenciesNs.end(), 0.0) / latenciesNs.size();
    const double p50 = percentile(0.50);
    const double p99 = percentile(0.99);
    const double p999 = percentile(0.999);

    std::cout << std::fixed << std::setprecision(0);
    std::cout << "----------------------------------------\n";
    std::cout << "Orders submitted:   " << NUM_ORDERS << "\n";
    std::cout << "Trades executed:    " << totalTrades << "\n";
    std::cout << "Remaining in book:  " << orderbook.Size() << "\n";
    std::cout << "Elapsed time:       " << elapsedMs << " ms\n";
    std::cout << "Throughput:         " << static_cast<long long>(ordersPerSec) << " orders/sec\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Per-order latency (nanoseconds):\n";
    std::cout << "  min:   " << minLatency << " ns\n";
    std::cout << "  avg:   " << avgLatency << " ns\n";
    std::cout << "  p50:   " << p50 << " ns\n";
    std::cout << "  p99:   " << p99 << " ns\n";
    std::cout << "  p99.9: " << p999 << " ns\n";
    std::cout << "  max:   " << maxLatency << " ns\n";
    std::cout << "----------------------------------------\n";

    return 0;
}