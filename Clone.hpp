//
//  Clone.hpp
//  poisson
//
//  Created by Priyatham Kattakinda on 20/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#ifndef Clone_hpp
#define Clone_hpp

#include "Point.hpp"
#include <Eigen/Dense>
#include <set>
#include <opencv2/core/core.hpp>

//Eigen::MatrixXd cloneImportedGrad(Eigen::MatrixXd, Eigen::MatrixXd, std::set<Point>, cv::Mat, Point);
//Eigen::MatrixXd cloneMixedGrad(Eigen::MatrixXd, Eigen::MatrixXd, std::set<Point>, cv::Mat, Point);
//Eigen::MatrixXd flatten(Eigen::MatrixXd, cv::Mat, std::set<Point>, cv::Mat);

cv::Mat flatten(cv::Mat, std::set<Point>, cv::Mat);
cv::Mat cloneImportedGrad(cv::Mat, cv::Mat, std::set<Point>, cv::Mat, Point);
cv::Mat cloneMixedGrad(cv::Mat, cv::Mat, std::set<Point>, cv::Mat, Point);

#endif /* Clone_hpp */
