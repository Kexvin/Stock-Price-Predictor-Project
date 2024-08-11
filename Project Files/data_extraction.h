// Kevin Do
#ifndef DATA_EXTRACTION_H
#define DATA_EXTRACTION_H

#include <vector>
#include <eigen3/Eigen/Dense>
#include <ctime>
#include <string>

int dateToDays(const std::string& date);
double calculateSMA(const std::vector<double>& prices, int period);
double calculateEMA(const std::vector<double>& prices, int period);
double calculateRSI(const std::vector<double>& prices, int period);
std::string epochToDate(time_t epochTime);
double calculateRSquared(const Eigen::VectorXd& y, const Eigen::VectorXd& y_pred);
Eigen::VectorXd ridgeRegression(const Eigen::MatrixXd& X, const Eigen::VectorXd& y, double alpha);
Eigen::MatrixXd addPolynomialFeatures(const Eigen::MatrixXd& X, int degree);
double calculateAverage(const std::vector<double>& vec);
tm parseDate(const std::string& dateStr);
tm addDays(const std::tm& date, int days);
std::string formatDate(const std::tm& date);

#endif // DATA_EXTRACTION_H
