// Kevin Do 2024-08-11
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include "stock_price_downloader.h"
#include "data_extraction.h"


using namespace std;
using namespace Eigen;

static void greetUser() {
    cout << "Hello, welcome to my program" << endl;
}

int main() {
    string symbol, startDate, endDate, choice;
    int number;
    bool run = true;

    while (run) {
        greetUser();
        cout << "(Choices)" << endl;
        cout << "1. Predict Stock Prices" << endl;
        cout << "2. View an Interval of the Stock Data" << endl;
        cout << "3. Exit Program" << endl;
        cin >> number;

        switch (number) {
            case 1: {
                cout << "Enter the stock you would like to track: ";
                cin >> symbol;
                transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
                cout << "Enter the start date for prediction of stock (format: YYYY-MM-DD): ";
                cin >> endDate;

                startDate = endDate;
                tm newStart = parseDate(startDate);
                tm newStartTm = addDays(newStart, -720);
                string newStartDate = formatDate(newStartTm);

                StockPriceDownloader downloader(symbol, newStartDate, endDate);
                const std::vector<StockPrice>& prices = downloader.getStockPrices();

                if (prices.size() > 1) {
                    std::vector<double> closePrices, volumes;
                    for (const auto& price : prices) {
                        closePrices.push_back(price.close);
                        volumes.push_back(price.volume);
                    }

                    // Calculate technical indicators for each day
                    std::vector<double> smaValues, emaValues, rsiValues;
                    for (size_t i = 0; i < prices.size(); ++i) {
                        smaValues.push_back(calculateSMA(closePrices, 14)); // 14-day SMA
                        emaValues.push_back(calculateEMA(closePrices, 14)); // 14-day EMA
                        rsiValues.push_back(calculateRSI(closePrices, 14)); // 14-day RSI
                    }

                    // Prepare data for polynomial regression
                    size_t num_days = prices.size();
                    MatrixXd X(num_days, 6); // Adding extra columns for SMA, EMA, RSI, and ATR
                    VectorXd y(num_days);
                    for (size_t i = 0; i < num_days; ++i) {
                        X(i, 0) = 1; // bias term
                        X(i, 1) = i;   // feature: time index
                        X(i, 2) = smaValues[i]; // feature: SMA
                        X(i, 3) = emaValues[i]; // feature: EMA
                        X(i, 4) = rsiValues[i]; // feature: RSI
                        X(i, 5) = prices[i].volume;
                        y(i) = prices[i].close;
                    }

                    // Add polynomial features for degree 2 and 3
                    MatrixXd polyX_deg2 = addPolynomialFeatures(X, 2);
                    MatrixXd polyX_deg3 = addPolynomialFeatures(X, 3);

                    // Train models with Ridge regression
                    double alpha = 0.1; // Regularization parameter
                    VectorXd theta_deg2 = ridgeRegression(polyX_deg2, y, alpha);
                    VectorXd theta_deg3 = ridgeRegression(polyX_deg3, y, alpha);

                    // Predict the close prices for the known dataset
                    VectorXd y_pred_deg2 = polyX_deg2 * theta_deg2;
                    VectorXd y_pred_deg3 = polyX_deg3 * theta_deg3;

                    // Combine the predictions with a weighted average
                    double weight_deg2 = 0.55;  // Adjust weight for degree 2
                    double weight_deg3 = 0.45;  // Adjust weight for degree 3
                    VectorXd y_pred_combined = weight_deg2 * y_pred_deg2 + weight_deg3 * y_pred_deg3;

                    // Calculate R-squared for the combined model
                    double r_squared_combined = calculateRSquared(y, y_pred_combined);
                    cout << "R-squared (Combined): " << r_squared_combined * 100 << "%" << endl;

                    int period;
                    cout << "How many days would you like to predict? ";
                    cin >> period;

                    // Extend the feature set for the next days
                    MatrixXd futureX(period, 6);
                    for (size_t i = 0; i < period; ++i) {
                        futureX(i, 0) = 1;          // bias term
                        futureX(i, 1) = num_days + i; // feature: future time index
                        futureX(i, 2) = smaValues.back(); // feature: SMA (use last known SMA)
                        futureX(i, 3) = emaValues.back(); // feature: EMA (use last known EMA)
                        futureX(i, 4) = rsiValues.back(); // feature: RSI (use last known RSI)
                        futureX(i, 5) = prices[i].volume;
                    }

                    // Add polynomial features for future dates for both degree 2 and 3
                    MatrixXd polyFutureX_deg2 = addPolynomialFeatures(futureX, 2);
                    MatrixXd polyFutureX_deg3 = addPolynomialFeatures(futureX, 3);

                    // Predict future values using the combined model
                    VectorXd futurePredictions_deg2 = polyFutureX_deg2 * theta_deg2;
                    VectorXd futurePredictions_deg3 = polyFutureX_deg3 * theta_deg3;
                    VectorXd futurePredictions = weight_deg2 * futurePredictions_deg2 + weight_deg3 * futurePredictions_deg3;

                    // Adjust future predictions by the mean error
                    futurePredictions = futurePredictions.array();

                    // Parse the end date to get the last known date
                    tm lastDate = parseDate(endDate);

                    cout << "Predictions: (" << symbol << ")" << endl;
                    // Output predictions for future dates
                    for (size_t i = 0; i < futurePredictions.size(); ++i) {
                        tm futureDate = addDays(lastDate, i + 1);
                        string futureDateStr = formatDate(futureDate);
                        cout << "Date: " << futureDateStr << " Close: $" << futurePredictions[i] << endl;
                    }
                } else {
                    cout << "Not enough data available for the given date range" << endl;
                }
                break;
            }
            case 2: {
                cout << "Enter the stock you would like to track: ";
                cin >> symbol;
                transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
                cout << "Enter the start date for the date range of stock (format: YYYY-MM-DD): ";
                cin >> startDate;
                cout << "Enter the end date for the date range of the stock (format: YYYY-MM-DD): ";
                cin >> endDate;

                StockPriceDownloader downloader(symbol, startDate, endDate);
                const std::vector<StockPrice>& prices = downloader.getStockPrices();

                for (const auto& price : prices) {
                    cout << "Date: " << price.date << " Open: $" << price.open << " Close: $" << price.close
                         << " High: $" << price.high << " Low: $" << price.low << " Volume: " << price.volume << endl;
                }

                cout << "(Indicators)" << endl;
                if (prices.size() > 1) {
                    std::vector<double> closePrices;
                    for (const auto& price : prices) {
                        closePrices.push_back(price.close);
                    }

                    std::vector<double> smaValues, emaValues, rsiValues;
                    for (size_t i = 0; i < prices.size(); ++i) {
                        smaValues.push_back(calculateSMA(closePrices, 14)); // 14-day SMA
                        emaValues.push_back(calculateEMA(closePrices, 14)); // 14-day EMA
                        rsiValues.push_back(calculateRSI(closePrices, 14)); // 14-day RSI
                    }

                    // Calculate the overall average for each indicator
                    double overallSMA = calculateAverage(smaValues);
                    double overallEMA = calculateAverage(emaValues);
                    double overallRSI = calculateAverage(rsiValues);

                    // Print the overall values
                    cout << "SMA: " << overallSMA << endl;
                    cout << "EMA: " << overallEMA << endl;
                    cout << "RSI: " << overallRSI << endl;
                }
                break;
            }
            case 3: {
                cout << "Exiting program..." << endl;
                run = false;
                break;
            }
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }

    }

    return 0;
}

