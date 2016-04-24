//
//  Clone.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#include "Clone.hpp"
#include "PoissonSolver.hpp"


Eigen::MatrixXd seamlessClone(Eigen::MatrixXd source, Eigen::MatrixXd destination, std::set<Point> domain, cv::Mat domainMask, std::function<double(Point, Point)> guidance) {
    PoissonSolver poissonSolver = PoissonSolver();
    poissonSolver.domain = domain;
    poissonSolver.domainMask = domainMask;
    poissonSolver.sizeImage.width = (unsigned int) destination.cols();
    poissonSolver.sizeImage.height = (unsigned int) destination.rows();
    std::function<double(Point)> dirichlet = [destination] (Point p)->double {
        return destination(p.y, p.x);
    };
    Eigen::MatrixXd x = poissonSolver.solve(dirichlet, guidance);
    {
        int i = 0;
        for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
            destination(p->y, p->x) = x(i, 0);
        }
    }
    return destination;
}

Eigen::MatrixXd cloneImportedGrad(Eigen::MatrixXd source, Eigen::MatrixXd destination, std::set<Point> domain, cv::Mat domainMask, Point translation) {
    std::function<double(Point, Point)> guidance = [source, translation] (Point p, Point q)->double {
        p.x += translation.x;
        p.y += translation.y;
        q.x += translation.x;
        q.y += translation.y;
        return source(p.y, p.x) - source(q.y, q.x);
    };
    return seamlessClone(source, destination, domain, domainMask, guidance);
}

Eigen::MatrixXd cloneMixedGrad(Eigen::MatrixXd source, Eigen::MatrixXd destination, std::set<Point> domain, cv::Mat domainMask, Point translation) {
    std::function<double(Point, Point)> guidance = [source, destination, translation] (Point p, Point q)->double {
        double destinationGrad = destination(p.y, p.x) - destination(q.y, q.x);
        p.x += translation.x;
        p.y += translation.y;
        q.x += translation.x;
        q.y += translation.y;
        double sourceGrad = source(p.y, p.x) - source(q.y, q.x);
        return (destinationGrad > sourceGrad)? destinationGrad: sourceGrad;
    };
    return seamlessClone(source, destination, domain, domainMask, guidance);
}

Eigen::MatrixXd flatten(Eigen::MatrixXd source, cv::Mat sourceEdge, std::set<Point> domain, cv::Mat domainMask) {
    std::function<double(Point, Point)> guidance = [source, sourceEdge] (Point p, Point q)->double {
        double sourceGrad = source(p.y, p.x) - source(q.y, q.x);
        return (sourceEdge.at<uint8_t>(p.y, p.x) != 0 || sourceEdge.at<uint8_t>(q.y, q.x) != 0)? sourceGrad: 0;
    };
    return seamlessClone(source, source, domain, domainMask, guidance);
}