cmake_minimum_required(VERSION 3.10)
project(stock_predictor)

set(CMAKE_CXX_STANDARD 17)

# Include directories for dependencies
include_directories(/usr/local/include)
include_directories(/usr/local/opt/eigen/include/eigen3)

# Find required packages
find_package(Boost REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add executables
add_executable(stock_predictor
        main.cpp
        stock_price_downloader.h
        stock_price_downloader.cpp
        data_extraction.cpp
        data_extraction.h
)

# Link libraries
target_link_libraries(stock_predictor
        ${Boost_LIBRARIES}
        ${CURL_LIBRARIES}
        nlohmann_json::nlohmann_json
)
