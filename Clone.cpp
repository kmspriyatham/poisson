//
//  Clone.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#include "Clone.hpp"
#include "PoissonSolver.hpp"
#include "CannyDetector.hpp"
#include <opencv2/core/eigen.hpp>

cv::Mat cloneImportedGrad(cv::Mat src, cv::Mat dest, std::set<Point> domain, cv::Mat domainMask, Point translation) {
    cv::Mat channel;
    cv::Mat srcChannels[3], destChannels[3], outputChannels[3];
    cv::split(src, srcChannels);
    cv::split(dest, destChannels);
    Eigen::MatrixXd source, destination;
    PoissonSolver poissonSolver = PoissonSolver();
    poissonSolver.domain = domain;
    poissonSolver.domainMask = domainMask;
    poissonSolver.sizeImage.width = (unsigned int) src.cols;
    poissonSolver.sizeImage.height = (unsigned int) src.rows;
    poissonSolver.compute();
    for (int i = 0; i < 3; i++) {
        srcChannels[i].convertTo(channel, CV_64FC1);
        cv::cv2eigen(channel, source);
        destChannels[i].convertTo(channel, CV_64FC1);
        cv::cv2eigen(channel, destination);
        std::function<double(Point)> dirichlet = [destination] (Point p)->double {
            return destination(p.y, p.x);
        };
        std::function<double(Point, Point)> guidance = [source, translation] (Point p, Point q)->double {
            p.x += translation.x;
            p.y += translation.y;
            q.x += translation.x;
            q.y += translation.y;
            return source(p.y, p.x) - source(q.y, q.x);
        };
        Eigen::MatrixXd x = poissonSolver.solve(dirichlet, guidance);
        {
            int i = 0;
            for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
                channel.at<double>(p->y, p->x) = x(i, 0);
            }
        }
        channel.convertTo(outputChannels[i], CV_8UC1);
    }
    cv::Mat output;
    cv::merge(outputChannels, 3, output);
    return output;
}

cv::Mat cloneMixedGrad(cv::Mat src, cv::Mat dest, std::set<Point> domain, cv::Mat domainMask, Point translation) {
    cv::Mat channel;
    cv::Mat srcChannels[3], destChannels[3], outputChannels[3];
    cv::split(src, srcChannels);
    cv::split(dest, destChannels);
    Eigen::MatrixXd source, destination;
    PoissonSolver poissonSolver = PoissonSolver();
    poissonSolver.domain = domain;
    poissonSolver.domainMask = domainMask;
    poissonSolver.sizeImage.width = (unsigned int) src.cols;
    poissonSolver.sizeImage.height = (unsigned int) src.rows;
    poissonSolver.compute();
    for (int i = 0; i < 3; i++) {
        srcChannels[i].convertTo(channel, CV_64FC1);
        cv::cv2eigen(channel, source);
        destChannels[i].convertTo(channel, CV_64FC1);
        cv::cv2eigen(channel, destination);
        std::function<double(Point)> dirichlet = [destination] (Point p)->double {
            return destination(p.y, p.x);
        };
        std::function<double(Point, Point)> guidance = [source, destination, translation] (Point p, Point q)->double {
            double destinationGrad = destination(p.y, p.x) - destination(q.y, q.x);
            p.x += translation.x;
            p.y += translation.y;
            q.x += translation.x;
            q.y += translation.y;
            double sourceGrad = source(p.y, p.x) - source(q.y, q.x);
            return (destinationGrad > sourceGrad)? destinationGrad: sourceGrad;
        };
        Eigen::MatrixXd x = poissonSolver.solve(dirichlet, guidance);
        {
            int i = 0;
            for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
                channel.at<double>(p->y, p->x) = x(i, 0);
            }
        }
        channel.convertTo(outputChannels[i], CV_8UC1);
    }
    cv::Mat output;
    cv::merge(outputChannels, 3, output);
    return output;
}

cv::Mat flatten(cv::Mat src, std::set<Point> domain, cv::Mat domainMask) {
    cv::Mat channel;
    cv::Mat srcChannels[3], outputChannels[3];
    cv::split(src, srcChannels);
    Eigen::MatrixXd source;
    cv::Mat srcEdge = cannyDetector(src);
    PoissonSolver poissonSolver = PoissonSolver();
    poissonSolver.domain = domain;
    poissonSolver.domainMask = domainMask;
    poissonSolver.sizeImage.width = (unsigned int) src.cols;
    poissonSolver.sizeImage.height = (unsigned int) src.rows;
    poissonSolver.compute();
    for (int i = 0; i < 3; i++) {
        srcChannels[i].convertTo(channel, CV_64FC1);
        cv::cv2eigen(channel, source);
        std::function<double(Point, Point)> guidance = [source, srcEdge] (Point p, Point q)->double {
            double sourceGrad = source(p.y, p.x) - source(q.y, q.x);
            return (srcEdge.at<uint8_t>(p.y, p.x) != 0 || srcEdge.at<uint8_t>(q.y, q.x) != 0)? sourceGrad: 0;
        };
        std::function<double(Point)> dirichlet = [source] (Point p)->double {
            return source(p.y, p.x);
        };
        Eigen::MatrixXd x = poissonSolver.solve(dirichlet, guidance);
        {
            int i = 0;
            for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
                channel.at<double>(p->y, p->x) = x(i, 0);
            }
        }
        channel.convertTo(outputChannels[i], CV_8UC1);
    }
    cv::Mat output;
    cv::merge(outputChannels, 3, output);
    return output;
}
