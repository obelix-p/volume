#include "data_storage.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>
#include <filesystem>

namespace fs = std::filesystem;

DataStorage::DataStorage(const std::string& output_dir, Format format)
    : output_dir_(output_dir), format_(format) {
    ensureDirectoryExists();
}

DataStorage::~DataStorage() {
    flush();
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

void DataStorage::ensureDirectoryExists() {
    try {
        fs::create_directories(output_dir_);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not create directory " << output_dir_ << ": " << e.what() << std::endl;
    }
}

std::string DataStorage::getFileName(const std::string& symbol) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << output_dir_ << "/" << symbol << "_" 
        << std::put_time(tm_info, "%Y%m%d");
    
    switch (format_) {
        case Format::CSV:
            oss << ".csv";
            break;
        case Format::JSON:
            oss << ".json";
            break;
        case Format::TEXT:
            oss << ".txt";
            break;
    }
    
    return oss.str();
}

std::string DataStorage::formatDataAsCSV(const MarketData& data) const {
    auto time_t_now = std::chrono::system_clock::to_time_t(data.timestamp);
    std::tm* tm_info = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S") << "," 
        << data.symbol << "," 
        << std::fixed << std::setprecision(2) << data.price << "," 
        << data.volume << std::endl;
    
    return oss.str();
}

std::string DataStorage::formatDataAsJSON(const MarketData& data) const {
    auto time_t_now = std::chrono::system_clock::to_time_t(data.timestamp);
    std::tm* tm_info = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << "{\n"
        << "  \"timestamp\": \"" << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S") << "\",\n"
        << "  \"symbol\": \"" << data.symbol << "\",\n"
        << "  \"price\": " << std::fixed << std::setprecision(2) << data.price << ",\n"
        << "  \"volume\": " << data.volume << "\n"
        << "}" << std::endl;
    
    return oss.str();
}

std::string DataStorage::formatDataAsText(const MarketData& data) const {
    auto time_t_now = std::chrono::system_clock::to_time_t(data.timestamp);
    std::tm* tm_info = std::localtime(&time_t_now);
    
    std::ostringstream oss;
    oss << "[" << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S") << "] "
        << data.symbol << " - Price: $" 
        << std::fixed << std::setprecision(2) << data.price 
        << " | Volume: " << data.volume << std::endl;
    
    return oss.str();
}

void DataStorage::save(const MarketData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(data);
    
    if (buffer_.size() >= BUFFER_SIZE) {
        flush();
    }
}

void DataStorage::saveMultiple(const std::vector<MarketData>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& item : data) {
        buffer_.push_back(item);
    }
    
    if (buffer_.size() >= BUFFER_SIZE) {
        flush();
    }
}

void DataStorage::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (buffer_.empty()) {
        return;
    }
    
    // Group data by symbol and date
    std::map<std::string, std::vector<MarketData>> grouped_data;
    for (const auto& data : buffer_) {
        grouped_data[data.symbol].push_back(data);
    }
    
    // Write each symbol's data to its file
    for (auto& [symbol, data_list] : grouped_data) {
        std::string filename = getFileName(symbol);
        std::ofstream file(filename, std::ios::app);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
            continue;
        }
        
        // Write header if file is empty
        if (file.tellp() == 0) {
            switch (format_) {
                case Format::CSV:
                    file << "Timestamp,Symbol,Price,Volume" << std::endl;
                    break;
                case Format::JSON:
                    file << "[" << std::endl;
                    break;
                case Format::TEXT:
                    file << "=== Market Data for " << symbol << " ===" << std::endl;
                    break;
            }
        }
        
        // Write data
        for (const auto& item : data_list) {
            switch (format_) {
                case Format::CSV:
                    file << formatDataAsCSV(item);
                    break;
                case Format::JSON:
                    file << formatDataAsJSON(item) << "," << std::endl;
                    break;
                case Format::TEXT:
                    file << formatDataAsText(item);
                    break;
            }
        }
        
        file.close();
        std::cout << "Data saved to: " << filename << std::endl;
    }
    
    buffer_.clear();
}

void DataStorage::setOutputDir(const std::string& dir) {
    output_dir_ = dir;
    ensureDirectoryExists();
}
