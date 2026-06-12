#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "config.h"
#include "market_data_fetcher.h"
#include "data_storage.h"

int main(int argc, char* argv[]) {
    std::cout << "=== Market Data Reader ==="  << std::endl;
    
    // Load configuration
    Config& config = Config::getInstance();
    
    std::string config_file = (argc > 1) ? argv[1] : "config.json";
    if (!config.loadFromFile(config_file)) {
        std::cerr << "Warning: Could not load config file '" << config_file << "'" << std::endl;
        std::cout << "Using default configuration..." << std::endl;
        
        // Set default values
        config.setApiKey("demo");
        config.setBaseUrl("https://api.example.com");
        config.setOutputDir("./market_data");
        config.setSymbols({"AAPL", "GOOGL", "MSFT"});
        config.setFetchInterval(60);
        config.setStorageFormat("csv");
    }
    
    config.print();
    
    try {
        // Initialize data storage
        DataStorage::Format format = DataStorage::Format::CSV;
        if (config.getStorageFormat() == "json") {
            format = DataStorage::Format::JSON;
        } else if (config.getStorageFormat() == "text") {
            format = DataStorage::Format::TEXT;
        }
        
        DataStorage storage(config.getOutputDir(), format);
        
        // Initialize market data fetcher
        MarketDataFetcher fetcher(config.getApiKey(), config.getBaseUrl());
        
        std::cout << "\nStarting market data collection..." << std::endl;
        std::cout << "Press Ctrl+C to stop." << std::endl;
        
        // Fetch initial data
        std::cout << "\nFetching initial market data for symbols: ";
        for (const auto& sym : config.getSymbols()) {
            std::cout << sym << " ";
        }
        std::cout << std::endl;
        
        try {
            auto market_data = fetcher.fetchMultiple(config.getSymbols());
            
            std::cout << "\nReceived " << market_data.size() << " data points:" << std::endl;
            for (const auto& data : market_data) {
                std::cout << "  " << data.symbol << ": $" << data.price 
                         << " (Volume: " << data.volume << ")" << std::endl;
                storage.save(data);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching initial data: " << e.what() << std::endl;
            std::cout << "\nNote: Ensure you have a valid API key and network connectivity." << std::endl;
            std::cout << "Or configure a local mock data source." << std::endl;
            return 1;
        }
        
        // Start continuous fetching in background
        fetcher.startContinuousFetch(config.getSymbols(), config.getFetchInterval());
        
        // Keep application running
        while (fetcher.isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        // Flush remaining data
        storage.flush();
        
        std::cout << "\nMarket data collection stopped." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
