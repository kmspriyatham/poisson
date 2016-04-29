//
//  main.cpp
//  poisson
//

#include <iostream>
#include "GUI.hpp"
#include "Clone.hpp"
#include <opencv2/core/eigen.hpp>
#include <ctime>

// acceptable modes, one for each method in the paper.
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
    cv::Mat output;
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
        // get the domain, domainMask
        std::pair<std::set<Point>, cv::Mat> editInfo = imageEditor(src);
        std::set<Point> domain = editInfo.first;
        cv::Mat domainMask = editInfo.second;
        if (domain.size() == 0) {
            return 0;
        }
        std::cout << "Processing Image..." << std::endl;
        t1 = clock();
        output = flatten(src, domain, domainMask);
        t2 = clock();
        t = (t2 - t1);

    } else {
        if (mode == IMPORT || mode == MIXED) {
            if (argc < 4) {
                std::cout << "Usage: " << argv[0] << "<'import' or 'mixed'> <source> <destination>" << std::endl;
                return -1;
            }
            cv::Mat src, dest;
            src = cv::imread(argv[2], cv::IMREAD_COLOR);
            dest = cv::imread(argv[3], cv::IMREAD_COLOR);
            
            if(!src.data || !dest.data) {
                std::cout <<  "Could not open or find the image" << std::endl ;
                return -1;
            }
            // get the domain, domainMask, translation
            std::tuple<std::set<Point>, cv::Mat, Point> editInfo = imageEditor(src, dest);
            std::set<Point> domain = std::get<0>(editInfo);
            if (domain.size() == 0) {
                return 0;
            }
            cv::Mat domainMask = std::get<1>(editInfo);
            Point translation = std::get<2>(editInfo);
            std::cout << "Processing Image..." << std::endl;
            t1 = clock();
            if (mode == MIXED) {
                output = cloneMixedGrad(src, dest, domain, domainMask, translation);
            } else {
                output = cloneImportedGrad(src, dest, domain, domainMask, translation);
            }
            t2 = clock();
            t = (t2 - t1);
        } else {
            std::cout << "Mode not recognised" << std::endl;
            return -1;
        }
    }
    cv::namedWindow("output", cv::WINDOW_NORMAL);
    cv::imshow("output", output);
    std::cout << "Done. Total Time: " << (float)t / CLOCKS_PER_SEC << " seconds" << std::endl;
    char key = cv::waitKey(0);
    if (key == 's') {
        cv::imwrite("output.jpg", output);
    }
    return 0;
}
