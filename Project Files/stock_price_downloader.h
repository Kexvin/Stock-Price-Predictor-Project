// Kevin Do
#ifndef STOCK_PRICE_DOWNLOADER_H
#define STOCK_PRICE_DOWNLOADER_H

#include <string>
#include <vector>
#include <dlib/statistics.h>

// The StockPrice structure holds information about the stock price on a specific date.
struct StockPrice {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    int64_t volume;
};

// The StockPriceDownloader class handles downloading stock price data.
class StockPriceDownloader {
public:
    // Constructor to initialize the downloader with the stock symbol and date range.
    StockPriceDownloader(const std::string& symbol, const std::string& start_date, const std::string& end_date);

    // Function to get the downloaded stock prices.
    const std::vector<StockPrice>& getStockPrices() const;

private:
    // Function to fetch the stock prices from the API.
    void fetchStockPrices();
    // Callback function used by libcurl to write the data received from the server.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    // Function to convert Unix timestamps to date strings.
    std::string convertUnixTimestampToDateString(int64_t timestamp);

    // Member variables to store the stock symbol and date range.
    std::string symbol;
    std::string start_date;
    std::string end_date;
    // Vector to store the fetched stock prices.
    std::vector<StockPrice> stock_prices;
};

#endif // STOCK_PRICE_DOWNLOADER_H
