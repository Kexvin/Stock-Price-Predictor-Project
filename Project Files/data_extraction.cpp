// Kevin Do
#include "data_extraction.h"
#include <ctime>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace std;
using namespace Eigen;

// Function to calculate R-squared
double calculateRSquared(const VectorXd& y, const VectorXd& y_pred) {
    double mean = y.mean();
    double ss_tot = (y.array() - mean).square().sum();
    double ss_res = (y - y_pred).squaredNorm();
    return 1 - (ss_res / ss_tot);
}

// Function to perform Ridge regression
VectorXd ridgeRegression(const MatrixXd& X, const VectorXd& y, double alpha) {
    int n = X.cols();
    MatrixXd I = MatrixXd::Identity(n, n);
    return (X.transpose() * X + alpha * I).ldlt().solve(X.transpose() * y);
}

// Function to add polynomial features
MatrixXd addPolynomialFeatures(const MatrixXd& X, int degree) {
    int rows = X.rows();
    int cols = X.cols();
    MatrixXd polyX(rows, cols * degree);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            for (int d = 1; d <= degree; ++d) {
                polyX(i, j * degree + d - 1) = pow(X(i, j), d);
            }
        }
    }
    return polyX;
}

// Function to format a date as YYYY-MM-DD
string formatDate(const tm& date) {
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date);
    return string(buffer);
}

// Function to parse a date string in the format YYYY-MM-DD
tm parseDate(const string& dateStr) {
    tm date = {};
    strptime(dateStr.c_str(), "%Y-%m-%d", &date);
    return date;
}

// Function to add days to a date
tm addDays(const tm& date, int days) {
    tm newDate = date;
    newDate.tm_mday += days;
    mktime(&newDate); // Normalize the date
    return newDate;
}

double calculateSMA(const vector<double>& prices, int period) {
    if (period > prices.size()) {
        return 0;
    }
    double sum = 0;
    for (int i = prices.size() - period; i < prices.size(); i++) {
        sum += prices[i];
    }
    return sum / period;
}

double calculateEMA(const vector<double>& prices, int period) {
    if (period > prices.size()) {
        return 0;
    }
    double multiplier = 2.0 / (period + 1);
    double ema = calculateSMA(prices, period); // Initialize EMA with SMA of the first period
    for (int i = prices.size() - period + 1; i < prices.size(); i++) {
        ema = ((prices[i] - ema) * multiplier) + ema;
    }
    return ema;
}

double calculateRSI(const vector<double>& prices, int period) {
    if (period > prices.size()) {
        return 0;
    }
    double gain = 0, loss = 0;

    // Calculate the initial average gain and loss
    for (int i = 1; i <= period; i++) {
        double change = prices[i] - prices[i - 1];
        if (change > 0) {
            gain += change;
        } else {
            loss -= change;
        }
    }
    double avgGain = gain / period;
    double avgLoss = loss / period;
    // Calculate the RSI
    for (int i = period + 1; i < prices.size(); i++) {
        double change = prices[i] - prices[i - 1];
        if (change > 0) {
            avgGain = ((avgGain * (period - 1)) + change) / period;
            avgLoss = (avgLoss * (period - 1)) / period;
        } else {
            avgLoss = ((avgLoss * (period - 1)) - change) / period;
            avgGain = (avgGain * (period - 1)) / period;
        }
    }
    if (avgLoss == 0) {
        return 100;
    }
    double rs = avgGain / avgLoss;
    return 100 - (100 / (1 + rs));
}

// Function to calculate the average of a vector
double calculateAverage(const vector<double>& vec) {
    if (vec.empty()) return 0.0;
    double sum = accumulate(vec.begin(), vec.end(), 0.0);
    return sum / vec.size();
}