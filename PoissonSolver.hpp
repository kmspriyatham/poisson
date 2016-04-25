//
//  PoissonSolver.hpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#ifndef PoissonSolver_hpp
#define PoissonSolver_hpp

#include <set>
#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
#include "Point.hpp"
#include "Size.hpp"
#include <opencv2/core/core.hpp>

class PoissonSolver {
public:
    std::set<Point> domain;
    Size sizeImage;
    cv::Mat domainMask;
    Eigen::MatrixXd solve(std::function<double(Point)>, std::function<double(Point, Point)>);
    void compute();
    PoissonSolver(std::set<Point>, Size, cv::Mat);
private:
    unsigned int numNeighbors(Point);
    std::set<Point> neighbors(Point);
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
};

#endif /* PoissonSolver_hpp */
