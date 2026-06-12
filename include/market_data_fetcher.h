#ifndef MARKET_DATA_FETCHER_H
#define MARKET_DATA_FETCHER_H

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

struct MarketData {
    std::string symbol;
    double price;
    long long volume;
    std::chrono::system_clock::time_point timestamp;
    
    MarketData(const std::string& sym, double p, long long v)
        : symbol(sym), price(p), volume(v), 
          timestamp(std::chrono::system_clock::now()) {}
};

class MarketDataFetcher {
public:
    MarketDataFetcher(const std::string& api_key, const std::string& base_url);
    ~MarketDataFetcher();
    
    // Fetch market data for a symbol
    MarketData fetchData(const std::string& symbol);
    
    // Fetch data for multiple symbols
    std::vector<MarketData> fetchMultiple(const std::vector<std::string>& symbols);
    
    // Start background fetching thread
    void startContinuousFetch(const std::vector<std::string>& symbols, int interval_seconds);
    
    // Stop background fetching
    void stopContinuousFetch();
    
    // Check if fetcher is running
    bool isRunning() const { return is_running_; }
    
private:
    std::string api_key_;
    std::string base_url_;
    std::thread fetch_thread_;
    bool is_running_;
    mutable std::mutex mutex_;
    
    // Helper method for HTTP requests
    std::string makeHttpRequest(const std::string& url);
    
    // Parse JSON response
    MarketData parseJsonResponse(const std::string& json_str, const std::string& symbol);
    
    // Continuous fetch worker function
    void continuousFetchWorker(const std::vector<std::string>& symbols, int interval_seconds);
};

#endif // MARKET_DATA_FETCHER_H
