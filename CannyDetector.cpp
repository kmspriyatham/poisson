//
//  CannyDetector.cpp
//  poisson
//

#include "CannyDetector.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// edge detector
cv::Mat cannyDetector(cv::Mat src) {
    cv::Mat srcGray, srcEdge;
    cv::cvtColor(src, srcGray, CV_BGR2GRAY);
    cv::blur(srcGray, srcEdge, cv::Size(3, 3));
    cv::Canny(srcEdge, srcEdge, 20, 50);
    return srcEdge;
}