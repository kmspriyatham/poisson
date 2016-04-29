//
//  GUI.hpp
//  poisson
//

#ifndef GUI_hpp
#define GUI_hpp

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Point.hpp"
#include <set>
#include <utility>

std::tuple<std::set<Point>, cv::Mat, Point> imageEditor(cv::Mat, cv::Mat);
std::pair<std::set<Point>, cv::Mat> imageEditor(cv::Mat);

#endif /* GUI_hpp */
