#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

class Config {
public:
    static Config& getInstance();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Getters
    std::string getApiKey() const { return api_key_; }
    std::string getBaseUrl() const { return base_url_; }
    std::string getOutputDir() const { return output_dir_; }
    std::vector<std::string> getSymbols() const { return symbols_; }
    int getFetchInterval() const { return fetch_interval_; }
    std::string getStorageFormat() const { return storage_format_; }
    
    // Setters
    void setApiKey(const std::string& key) { api_key_ = key; }
    void setBaseUrl(const std::string& url) { base_url_ = url; }
    void setOutputDir(const std::string& dir) { output_dir_ = dir; }
    void setSymbols(const std::vector<std::string>& syms) { symbols_ = syms; }
    void setFetchInterval(int interval) { fetch_interval_ = interval; }
    void setStorageFormat(const std::string& format) { storage_format_ = format; }
    
    // Print configuration
    void print() const;
    
private:
    Config();
    ~Config();
    
    std::string api_key_;
    std::string base_url_;
    std::string output_dir_;
    std::vector<std::string> symbols_;
    int fetch_interval_;
    std::string storage_format_;
};

#endif // CONFIG_H
