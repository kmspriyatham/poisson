//
//  CannyDetector.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 22/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#include "CannyDetector.hpp"
#include "opencv2/imgproc/imgproc.hpp"

cv::Mat cannyDetector(cv::Mat src) {
    cv::Mat srcGray, srcEdge;
    cv::cvtColor(src, srcGray, CV_BGR2GRAY);
    cv::blur(srcGray, srcEdge, cv::Size(3, 3));
    cv::Canny(srcEdge, srcEdge, 20, 50);
    return srcEdge;
}