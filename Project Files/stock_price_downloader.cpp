// Kevin Do
#include "stock_price_downloader.h"
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>


using namespace std;
using json = nlohmann::json;

// Callback function used by libcurl to write the data received from the server
size_t StockPriceDownloader::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Constructor to initialize the downloader with the stock symbol and date range
StockPriceDownloader::StockPriceDownloader(const string& symbol, const string& start_date, const string& end_date)
    : symbol(symbol), start_date(start_date), end_date(end_date) {
    fetchStockPrices(); // Fetch the stock prices when the object is created
}

// Function to get the downloaded stock prices
const vector<StockPrice>& StockPriceDownloader::getStockPrices() const {
    return stock_prices;
}

// Function to convert Unix timestamps to date strings
std::string StockPriceDownloader::convertUnixTimestampToDateString(int64_t timestamp) {
    std::time_t time = timestamp / 1000;
    std::tm* tm = std::gmtime(&time);
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d");
    return ss.str();
}

// Function to fetch the stock prices from the API
void StockPriceDownloader::fetchStockPrices() {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        string url = "https://api.polygon.io/v2/aggs/ticker/" + symbol + "/range/1/day/" +
                     start_date + "/" + end_date + "?apiKey=S_6VEUEzvLtF6y_1H45nmpmFXL6lvytz";
        cout << "Fetching URL: " << url << endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            return;
        }

        cout << "API Response: " << readBuffer << endl; // Debugging print

        try {
            auto jsonResponse = json::parse(readBuffer);

            if (jsonResponse.contains("error")) {
                cerr << "API Error: " << jsonResponse["error"] << endl;
                return;
            }

            if (!jsonResponse.contains("results")) {
                cerr << "No results found in the API response." << endl;
                return;
            }

            // Const allows for read-only data that cannot be modified, auto tells the compiler to auto deduce the item being read,
            // & is a reference to the actual object in colleciton prevents copying object.
            for (const auto& item : jsonResponse["results"]) {
                // Create a StockPrice object to store the stock price data for the current item
                StockPrice sp;

                // Convert the timestamp to a human-readable date string and store it in the date field of the StockPrice object
                sp.date = convertUnixTimestampToDateString(item["t"].get<int64_t>()); // Convert timestamp to date string
                sp.open = item["o"];
                sp.high = item["h"];
                sp.low = item["l"];
                sp.close = item["c"];
                sp.volume = item["v"];
                stock_prices.push_back(sp);

            }
        } catch (const std::exception& e) {
            cerr << "Error parsing JSON: " << e.what() << endl;
        }
    }
}
