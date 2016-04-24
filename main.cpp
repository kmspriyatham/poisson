//
//  main.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#include <iostream>
#include "GUI.hpp"
#include "Clone.hpp"
#include <opencv2/core/eigen.hpp>
#include <ctime>
#include "CannyDetector.hpp"

#define IMPORT "import"
#define MIXED "mixed"
#define FLATTEN "flatten"

int main(int argc, const char * argv[]) {
    if (argc == 1) {
        std::cout << "Mode not recognised" << std::endl;
        std::cout << "Usage: " << argv[0] << " <mode> <source> <destination>" << std::endl;
        return -1;
    }
    std::string mode = argv[1];
    cv::Mat outputChannels[3];
    clock_t t1, t2, t = 0;
    if (mode == FLATTEN) {
        if (argc < 3) {
            std::cout << "Usage: " << argv[0] << " 'flatten' <source>" << std::endl;
        }
        cv::Mat src;
        src = cv::imread(argv[2], cv::IMREAD_COLOR);
        
        if(!src.data) {
            std::cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        std::pair<std::set<Point>, cv::Mat> editInfo = imageEditor(src);
        std::set<Point> domain = editInfo.first;
        cv::Mat domainMask = editInfo.second;
        if (domain.size() == 0) {
            return 0;
        }
        std::cout << "Processing Image..." << std::endl;
        cv::Mat channel;
        cv::Mat srcChannels[3];
        cv::split(src, srcChannels);
        Eigen::MatrixXd source;
        cv::Mat sourceEdge = cannyDetector(src);
        for (int i = 0; i < 3; i++) {
            std::cout << "Cloning channel: " << i + 1 << std::endl;
            t1 = clock();
            srcChannels[i].convertTo(channel, CV_64FC1);
            cv::cv2eigen(channel, source);
            cv::eigen2cv(flatten(source, sourceEdge, domain, domainMask), channel);
            channel.convertTo(outputChannels[i], CV_8UC1);
            t2 = clock();
            t += (t2 - t1);
            std::cout << "Time taken: " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;
        }
    } else {
        if (mode == IMPORT || mode == MIXED) {
            if (argc < 4) {
                std::cout << "Usage: " << argv[0] << "<'import' or 'mixed'> <source> <destination>" << std::endl;
                return -1;
            }
            cv::Mat src, dest, output;
            src = cv::imread(argv[2], cv::IMREAD_COLOR);
            dest = cv::imread(argv[3], cv::IMREAD_COLOR);
            
            if(!src.data || !dest.data) {
                std::cout <<  "Could not open or find the image" << std::endl ;
                return -1;
            }
            std::tuple<std::set<Point>, cv::Mat, Point> editInfo = imageEditor(src, dest);
            std::set<Point> domain = std::get<0>(editInfo);
            if (domain.size() == 0) {
                return 0;
            }
            cv::Mat domainMask = std::get<1>(editInfo);
            Point translation = std::get<2>(editInfo);
            std::cout << "Processing Image..." << std::endl;
            cv::Mat channel;
            cv::Mat srcChannels[3], destChannels[3];
            cv::split(src, srcChannels);
            cv::split(dest, destChannels);
            Eigen::MatrixXd source, destination;
            for (int i = 0; i < 3; i++) {
                std::cout << "Cloning channel: " << i + 1 << std::endl;
                t1 = clock();
                srcChannels[i].convertTo(channel, CV_64FC1);
                cv::cv2eigen(channel, source);
                destChannels[i].convertTo(channel, CV_64FC1);
                cv::cv2eigen(channel, destination);
                if (mode == MIXED) {
                    cv::eigen2cv(cloneMixedGrad(source, destination, domain, domainMask, translation), channel);
                } else {
                    cv::eigen2cv(cloneImportedGrad(source, destination, domain, domainMask, translation), channel);
                }
                channel.convertTo(outputChannels[i], CV_8UC1);
                t2 = clock();
                t += (t2 - t1);
                std::cout << "Time taken: " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;
            }
        } else {
            std::cout << "Mode not recognised" << std::endl;
            return -1;
        }
    }
    cv::Mat output;
    cv::merge(outputChannels, 3, output);
    cv::namedWindow("output", cv::WINDOW_NORMAL);
    cv::imshow("output", output);
    std::cout << "Done. Total Time: " << (float)t / CLOCKS_PER_SEC << " seconds" << std::endl;
    char key = cv::waitKey(0);
    if (key == 's') {
        cv::imwrite("output.jpg", output);
    }
    return 0;
}
