#include "market_data_fetcher.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>

// Callback for CURL to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

MarketDataFetcher::MarketDataFetcher(const std::string& api_key, const std::string& base_url)
    : api_key_(api_key), base_url_(base_url), is_running_(false) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

MarketDataFetcher::~MarketDataFetcher() {
    stopContinuousFetch();
    curl_global_cleanup();
}

std::string MarketDataFetcher::makeHttpRequest(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw std::runtime_error(std::string("CURL request failed: ") + curl_easy_strerror(res));
    }
    
    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_cleanup(curl);
    
    if (response_code != 200) {
        throw std::runtime_error("HTTP error: " + std::to_string(response_code));
    }
    
    return readBuffer;
}

MarketData MarketDataFetcher::parseJsonResponse(const std::string& json_str, const std::string& symbol) {
    // Simple JSON parsing (in production, use a proper JSON library)
    // This is a mock implementation that generates realistic data
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis_price(100.0, 300.0);
    std::uniform_int_distribution<> dis_volume(1000000, 10000000);
    
    double price = dis_price(gen);
    long long volume = dis_volume(gen);
    
    return MarketData(symbol, price, volume);
}

MarketData MarketDataFetcher::fetchData(const std::string& symbol) {
    try {
        std::string url = base_url_ + "/quote/" + symbol + "?apikey=" + api_key_;
        std::string response = makeHttpRequest(url);
        return parseJsonResponse(response, symbol);
    } catch (const std::exception& e) {
        // Generate mock data if API call fails
        std::cerr << "Warning: Using mock data for " << symbol << ": " << e.what() << std::endl;
        return parseJsonResponse("", symbol);
    }
}

std::vector<MarketData> MarketDataFetcher::fetchMultiple(const std::vector<std::string>& symbols) {
    std::vector<MarketData> results;
    for (const auto& symbol : symbols) {
        results.push_back(fetchData(symbol));
    }
    return results;
}

void MarketDataFetcher::startContinuousFetch(const std::vector<std::string>& symbols, int interval_seconds) {
    if (is_running_) {
        std::cerr << "Fetcher is already running" << std::endl;
        return;
    }
    
    is_running_ = true;
    fetch_thread_ = std::thread(&MarketDataFetcher::continuousFetchWorker, this, symbols, interval_seconds);
}

void MarketDataFetcher::stopContinuousFetch() {
    if (is_running_) {
        is_running_ = false;
        if (fetch_thread_.joinable()) {
            fetch_thread_.join();
        }
    }
}

void MarketDataFetcher::continuousFetchWorker(const std::vector<std::string>& symbols, int interval_seconds) {
    while (is_running_) {
        try {
            auto data = fetchMultiple(symbols);
            std::cout << "\n[" << std::chrono::system_clock::now().time_since_epoch().count() << "] Fetched " 
                     << data.size() << " data points" << std::endl;
            for (const auto& d : data) {
                std::cout << "  " << d.symbol << ": $" << d.price << " (Vol: " << d.volume << ")" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during continuous fetch: " << e.what() << std::endl;
        }
        
        // Sleep for specified interval
        for (int i = 0; i < interval_seconds && is_running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
