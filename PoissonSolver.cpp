//
//  PoissonSolver.cpp
//  poisson
//

#include "PoissonSolver.hpp"
#include <Eigen/Sparse>
#include <vector>
#include <iostream>

PoissonSolver::PoissonSolver(std::set<Point> domain, Size sizeImage, cv::Mat domainMask) {
    Eigen::SparseMatrix<double, Eigen::ColMajor> A;
    this->domain = domain;
    this->sizeImage = sizeImage;
    this->domainMask = domainMask;
    std::cout << "Number of pixels in domain: " << domain.size() << std::endl;
}

// return the number of neighbors of a point.
unsigned int PoissonSolver::numNeighbors(Point p) {
    unsigned int numNeighbors = 4;
    if (p.x == sizeImage.width - 1 || p.x == 0) {
        numNeighbors--;
    }
    if (p.y == sizeImage.height - 1 || p.y == 0) {
        numNeighbors--;
    }
    return numNeighbors;
}

// return the neighbors of a point.
std::set<Point> PoissonSolver::neighbors(Point p) {
    std::set<Point> neighbors;
    if (p.x != 0) {
        neighbors.insert(Point(p.x - 1, p.y));
    }
    if (p.y != 0) {
        neighbors.insert(Point(p.x, p.y - 1));
    }
    if (p.x != sizeImage.width - 1) {
        neighbors.insert(Point(p.x + 1, p.y));
    }
    if (p.y != sizeImage.height - 1) {
        neighbors.insert(Point(p.x, p.y + 1));
    }
    return neighbors;
}

// the discretized poisson equation is a linear system (Ax = b, say).
// compute() computes A and performs LDLT decomposition of A.
void PoissonSolver::compute() {
    unsigned int n = (unsigned int) domain.size();
    Eigen::SparseMatrix<double, Eigen::ColMajor> A(n, n);
    A.reserve(4 * n);
    std::vector<Eigen::Triplet<double>> coefficients; // vector of triplets of the form (i, j, A(i, j))
    coefficients.reserve(4 * n);
    {
        int i = 0;
        int index;
        for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
            coefficients.push_back(Eigen::Triplet<double>(i, i, numNeighbors(*p)));
            std::set<Point> pNeighbors = neighbors(*p);
            for (std::set<Point>::iterator q = pNeighbors.begin(); q != pNeighbors.end(); q++) {
                index = domainMask.at<int>(q->y, q->x);
                if (index != -1) { // neighbor is in domain
                    coefficients.push_back(Eigen::Triplet<double>(i, index, -1));
                }
            }
        }
    }
    A.setFromTriplets(coefficients.begin(), coefficients.end());
    solver.compute(A);
}

// solve for x using the decomposition of A. A is same for all the channels, only b changes.
Eigen::MatrixXd PoissonSolver::solve(std::function<double(Point)> dirichlet, std::function<double(Point, Point)> guidance) {
    unsigned int n = (unsigned int) domain.size();
    Eigen::MatrixXd b(n, 1), x(n, 1);
    b.setZero();
    x.setZero();
    {
        int i = 0;
        int index;
        for (std::set<Point>::iterator p = domain.begin(); p != domain.end(); p++, i++) {
            std::set<Point> pNeighbors = neighbors(*p);
            for (std::set<Point>::iterator q = pNeighbors.begin(); q != pNeighbors.end(); q++) {
                index = domainMask.at<int>(q->y, q->x);
                if (index == -1) { // neighbor is not in domain
                    b(i, 0) += dirichlet(*q);
                }
            }
            for (std::set<Point>::iterator q = pNeighbors.begin(); q != pNeighbors.end(); q++) {
                b(i, 0) += guidance(*p, *q);
            }
        }
    }
    x = solver.solve(b);
    return x;
}