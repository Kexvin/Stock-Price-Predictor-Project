# Stock Price Predictor

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Overview
Stock Predictor is a C++ project that predicts stock prices using polynomial regression models. It utilizes various technical indicators like SMA, EMA, and RSI for more accurate predictions.

## Features
- Download historical stock data via an API using the `StockPriceDownloader` class.
- Calculate technical indicators (SMA, EMA, RSI).
- View and analyze historical stock data over a specific date range, displaying key metrics such as open, close, high, low prices, and trading volume.
- Perform polynomial regression to predict future stock prices in any range (Most accurate within 30-days).

## Installation
### Prerequisites
- C++17 or later
- CMake 3.10 or later
- Boost Libraries
- CURL
- Eigen
- nlohmann_json

### Building the Project
1. Clone the repository:
   ```bash
   git clone https://github.com/Kexvin/stock-predictor.git
   cd stock-predictor
