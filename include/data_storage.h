#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "market_data_fetcher.h"
#include <string>
#include <vector>
#include <fstream>
#include <mutex>

class DataStorage {
public:
    // Storage format types
    enum class Format {
        CSV,
        JSON,
        TEXT
    };
    
    DataStorage(const std::string& output_dir, Format format = Format::CSV);
    ~DataStorage();
    
    // Save single market data point
    void save(const MarketData& data);
    
    // Save multiple market data points
    void saveMultiple(const std::vector<MarketData>& data);
    
    // Flush buffered data to disk
    void flush();
    
    // Set output directory
    void setOutputDir(const std::string& dir);
    
    // Get output directory
    std::string getOutputDir() const { return output_dir_; }
    
private:
    std::string output_dir_;
    Format format_;
    std::ofstream file_stream_;
    std::vector<MarketData> buffer_;
    mutable std::mutex mutex_;
    
    static const size_t BUFFER_SIZE = 100;
    
    // Helper methods
    std::string getFileName(const std::string& symbol);
    std::string formatDataAsCSV(const MarketData& data) const;
    std::string formatDataAsJSON(const MarketData& data) const;
    std::string formatDataAsText(const MarketData& data) const;
    void ensureDirectoryExists();
};

#endif // DATA_STORAGE_H
