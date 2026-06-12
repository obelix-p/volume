#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>

Config::Config()
    : api_key_("demo"),
      base_url_("https://api.example.com"),
      output_dir_("./market_data"),
      fetch_interval_(60),
      storage_format_("csv") {
    symbols_ = {"AAPL", "GOOGL", "MSFT"};
}

Config::~Config() {}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Trim whitespace
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            
            if (key == "api_key") {
                api_key_ = value;
            } else if (key == "base_url") {
                base_url_ = value;
            } else if (key == "output_dir") {
                output_dir_ = value;
            } else if (key == "fetch_interval") {
                try {
                    fetch_interval_ = std::stoi(value);
                } catch (...) {}
            } else if (key == "storage_format") {
                storage_format_ = value;
            } else if (key == "symbols") {
                // Parse comma-separated symbols
                symbols_.clear();
                std::istringstream symbol_stream(value);
                std::string symbol;
                while (std::getline(symbol_stream, symbol, ',')) {
                    symbol.erase(0, symbol.find_first_not_of(" \t"));
                    symbol.erase(symbol.find_last_not_of(" \t") + 1);
                    if (!symbol.empty()) {
                        symbols_.push_back(symbol);
                    }
                }
            }
        }
    }
    
    file.close();
    return true;
}

void Config::print() const {
    std::cout << "\n=== Configuration ==="  << std::endl;
    std::cout << "API Key: " << api_key_ << std::endl;
    std::cout << "Base URL: " << base_url_ << std::endl;
    std::cout << "Output Directory: " << output_dir_ << std::endl;
    std::cout << "Fetch Interval: " << fetch_interval_ << " seconds" << std::endl;
    std::cout << "Storage Format: " << storage_format_ << std::endl;
    std::cout << "Symbols: ";
    for (size_t i = 0; i < symbols_.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << symbols_[i];
    }
    std::cout << std::endl;
}
