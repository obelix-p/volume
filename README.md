# Real-Time Market Data Reader

A C++ application that reads real-time market data (price and volume) from external APIs and saves it to the filesystem.

## Features

- **Real-time Data Fetching**: Continuously fetch market price and volume data for multiple stock symbols
- **Multi-threaded Architecture**: Background worker thread for continuous data collection
- **Flexible Storage Formats**: Save data in CSV, JSON, or text format
- **Configurable**: Easy configuration via JSON config file
- **Error Handling**: Robust error handling and retry mechanisms
- **Buffered I/O**: Efficient buffered file writing for performance

## Prerequisites

- C++17 or later
- CMake 3.10+
- libcurl (for HTTP requests)
- nlohmann_json (optional, for JSON parsing)

### Installation

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake libcurl4-openssl-dev nlohmann-json3-dev
```

**macOS (Homebrew):**
```bash
brew install cmake curl nlohmann-json
```

**Windows (vcpkg):**
```bash
vcpkg install curl nlohmann-json
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Configuration

Edit `config.json` with your settings:

```json
api_key=YOUR_API_KEY_HERE
base_url=https://api.example.com
output_dir=./market_data
fetch_interval=60
storage_format=csv
symbols=AAPL, GOOGL, MSFT
```

### Configuration Options

- **api_key**: Your market data API key (e.g., Alpha Vantage, IEX Cloud)
- **base_url**: API endpoint URL
- **output_dir**: Directory where data files will be saved
- **fetch_interval**: Seconds between data fetches
- **storage_format**: Output format (csv, json, text)
- **symbols**: Comma-separated list of stock symbols to track

## Usage

### Basic Usage

```bash
./market_data_reader
```

### With Custom Config

```bash
./market_data_reader /path/to/config.json
```

## Output Files

Data is saved to the output directory with filenames like:
- `AAPL_20240612.csv` (for CSV format)
- `AAPL_20240612.json` (for JSON format)
- `AAPL_20240612.txt` (for text format)

### CSV Format Example
```
Timestamp,Symbol,Price,Volume
2024-06-12 14:30:45,AAPL,189.45,2500000
2024-06-12 14:31:45,AAPL,189.50,2600000
```

### JSON Format Example
```json
{
  "timestamp": "2024-06-12 14:30:45",
  "symbol": "AAPL",
  "price": 189.45,
  "volume": 2500000
}
```

## Architecture

### Key Components

1. **MarketDataFetcher**: Handles HTTP requests to market data APIs
2. **DataStorage**: Manages file I/O and data persistence
3. **Config**: Configuration management and loading
4. **MarketData**: Data structure for price/volume information

## Thread Safety

The application uses:
- `std::mutex` for thread-safe file operations
- `std::thread` for background data fetching
- Lock guards to prevent data races

## Performance Considerations

- **Buffering**: Data is buffered before writing (default: 100 items) to reduce I/O operations
- **Asynchronous Fetching**: Background thread prevents blocking the main application
- **Connection Reuse**: HTTP connections are reused when possible

## API Integration Examples

### Alpha Vantage
```
base_url=https://www.alphavantage.co/query
api_key=YOUR_ALPHA_VANTAGE_KEY
```

### IEX Cloud
```
base_url=https://cloud.iexapis.com/stable/stock
api_key=YOUR_IEX_TOKEN
```

## Troubleshooting

### "Could not open file" Error
- Ensure output directory exists and is writable
- Check file permissions

### "CURL request failed" Error
- Verify network connectivity
- Check API endpoint URL
- Validate API key

### Memory Issues
- Reduce `BUFFER_SIZE` in `data_storage.h`
- Reduce `fetch_interval` in config
- Limit number of symbols

## License

MIT License

## Contributing

Contributions are welcome! Please submit pull requests with:
- New features or improvements
- Bug fixes
- Documentation updates

## Future Enhancements

- [ ] Database integration (SQLite, PostgreSQL)
- [ ] Real-time data visualization
- [ ] Technical analysis indicators
- [ ] Alert system for price thresholds
- [ ] Web dashboard for monitoring
- [ ] Support for multiple data sources
- [ ] Data compression for archival
