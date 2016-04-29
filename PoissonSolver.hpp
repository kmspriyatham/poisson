//
//  PoissonSolver.hpp
//  poisson
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
    std::set<Point> domain; // set of points in domain.
    Size sizeImage; // size of the image
    cv::Mat domainMask; // this is a matrix that has the same size as the image and
                        // each element is the index of the point in domain(-1 if not in domain)
    Eigen::MatrixXd solve(std::function<double(Point)>, std::function<double(Point, Point)>);
    void compute();
    PoissonSolver(std::set<Point>, Size, cv::Mat); // constructor
private:
    unsigned int numNeighbors(Point);
    std::set<Point> neighbors(Point);
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
};

#endif /* PoissonSolver_hpp */
