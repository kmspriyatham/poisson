//
//  GUI.cpp
//  poisson
//

#include "GUI.hpp"
#include <iostream>
#include "Size.hpp"

bool drag = false;
Point sourceOrigin = Point(-1, -1);
Point destinationOrigin = Point(-1, -1);
cv::Mat src, dest;
Size srcSize, destSize;
std::string sourceWindow = "Source";
std::string destinationWindow = "Destination";
int brushSize = 2;

Point translation = Point(0, 0);
std::set<Point> domain;
cv::Mat domainMask;

bool isValid(Point p, Size size) {
    return (p.x >= 0 && p.y >= 0 && p.x < size.width && p.y < size.height);
}

void sourceOnMouseClone(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN && sourceOrigin == Point(-1, -1)) {
        sourceOrigin.x = x;
        sourceOrigin.y = y;
    }
}

// callback to draw on destination image to select domain with an arbitrary shape.
void destinationOnMouseClone(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        drag = true;
        if (destinationOrigin == Point(-1, -1) && sourceOrigin != Point(-1, -1)) {
            destinationOrigin.x = x;
            destinationOrigin.y = y;
            translation.x = sourceOrigin.x - x;
            translation.y = sourceOrigin.y - y;
        }
    }
    if (event == cv::EVENT_MOUSEMOVE && drag && sourceOrigin != Point(-1, -1)) {
        // when user draws onto the destination image, all points in the <brushSize> neighborhood are added to the domain
        for (int i = -brushSize; i <= brushSize; i++) {
            for (int j = -brushSize; j <= brushSize; j++) {
                Point srcPoint = Point(translation.x + x + i, translation.y + y + j);
                Point destPoint = Point(x + i, y + j);
                if (isValid(srcPoint, srcSize) && isValid(destPoint, destSize)) {
                    domain.insert(destPoint);
                    dest.at<cv::Vec3b>(destPoint.y, destPoint.x) = src.at<cv::Vec3b>(srcPoint.y, srcPoint.x);
                }
            }
        }
        cv::imshow(destinationWindow, dest);
    }
    if (event == cv::EVENT_LBUTTONUP) {
        drag = false;
    }
}

// callback to draw a rectangle on destination image to select a rectangular domain.
void destinationOnMouseCloneRec(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (destinationOrigin == Point(-1, -1) && sourceOrigin != Point(-1, -1)) {
            destinationOrigin.x = x;
            destinationOrigin.y = y;
            translation.x = sourceOrigin.x - x;
            translation.y = sourceOrigin.y - y;
        }
    }
    if (event == cv::EVENT_LBUTTONUP) {
        for (int i = sourceOrigin.x; i <= translation.x + x; i++) {
            for (int j = sourceOrigin.y; j <= translation.y + y; j++) {
                Point srcPoint = Point(i, j);
                Point destPoint = Point(i - translation.x, j - translation.y);
                if (isValid(srcPoint, srcSize)) {
                    domain.insert(destPoint);
                    dest.at<cv::Vec3b>(destPoint.y, destPoint.x) = src.at<cv::Vec3b>(srcPoint.y, srcPoint.x);
                }
            }
        }
        cv::imshow(destinationWindow, dest);
    }
}

std::tuple<std::set<Point>, cv::Mat, Point> imageEditor(cv::Mat source, cv::Mat destination) {
    src = source;
    dest = destination;
    srcSize = Size(source.rows, source.cols);
    destSize = Size(dest.rows, dest.cols);
    
    cv::namedWindow(sourceWindow, cv::WINDOW_NORMAL);
    cv::setMouseCallback(sourceWindow, sourceOnMouseClone);
    cv::imshow(sourceWindow, source);
    
    cv::namedWindow(destinationWindow, cv::WINDOW_NORMAL);
    cv::imshow(destinationWindow, destination);
    domainMask = cv::Mat(destination.rows, destination.cols, CV_32S, cv::Scalar(-1));
    while (true) {
        char key = cv::waitKey(0);
        if (key == 'q') {
            break;
        }
        if (key == 'r') { // draw a rectangle
            cv::setMouseCallback(destinationWindow, destinationOnMouseCloneRec);
        }
        if (key == 'd') { // draw an arbitrary shape
            cv::setMouseCallback(destinationWindow, destinationOnMouseClone);
        }
        if (key == 'e') { // solve the poisson equation and edit the image
            {
                int i = 0;
                for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
                    domainMask.at<int>(p->y, p->x) = i;
                }
            }
            return std::tuple<std::set<Point>, cv::Mat, Point> (domain, domainMask, translation);
            break;
        }
    }
    return std::tuple<std::set<Point>, cv::Mat, Point> ();
}

void sourceOnMouseFlatten(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        drag = true;
    }
    if (event == cv::EVENT_MOUSEMOVE && drag) {
        for (int i = -brushSize; i <= brushSize; i++) {
            for (int j = -brushSize; j <= brushSize; j++) {
                Point srcPoint = Point(x + i, y + j);
                if (isValid(srcPoint, srcSize)) {
                    domain.insert(srcPoint);
                    src.at<cv::Vec3b>(srcPoint.y, srcPoint.x) = cv::Vec3b(255, 255, 255);
                }
            }
        }
        cv::imshow(sourceWindow, src);
    }
    if (event == cv::EVENT_LBUTTONUP) {
        drag = false;
    }
}

void sourceOnMouseFlattenRec(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        sourceOrigin.x = x;
        sourceOrigin.y = y;
    }
    if (event == cv::EVENT_LBUTTONUP) {
        for (int i = sourceOrigin.x; i <= x; i++) {
            for (int j = sourceOrigin.y; j <= y; j++) {
                Point srcPoint = Point(i, j);
                domain.insert(srcPoint);
                src.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255); // color the domain rectangle white
            }
        }
        cv::imshow(sourceWindow, src);
    }
}

std::pair<std::set<Point>, cv::Mat> imageEditor(cv::Mat source) {
    src = source.clone();
    srcSize = Size(source.rows, source.cols);
    domainMask = cv::Mat(source.rows, source.cols, CV_32S, cv::Scalar(-1));
    cv::namedWindow(sourceWindow, cv::WINDOW_NORMAL);
    cv::imshow(sourceWindow, src);
    while (true) {
        char key = cv::waitKey(0);
        if (key == 'r') { // draw rectangle
            cv::setMouseCallback(sourceWindow, sourceOnMouseFlattenRec);
        }
        if (key == 'd') { // draw arbitrary shape
            cv::setMouseCallback(sourceWindow, sourceOnMouseFlatten);
        }
        if (key == 'q') {
            break;
        }
        if (key == 'e') { // solve the poisson equation and edit the image
            {
                int i = 0;
                for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
                    domainMask.at<int>(p->y, p->x) = i; // set the indices of points in domain as the elements of domainMask
                }
            }
            return std::pair<std::set<Point>, cv::Mat>(domain, domainMask);
            break;
        }
    }
    return std::pair<std::set<Point>, cv::Mat>();
}