//
//  Point.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
//

#include "Point.hpp"

Point::Point(int x, int y) {
    this->x = x;
    this->y = y;
}

Point::Point() {
    x = 0;
    y = 0;
}

bool Point::operator<(const Point other) const {
    if (this->x != other.x) {
        return (this->x < other.x);
    } else {
        return (this->y < other.y);
    }
}

bool Point::operator==(const Point other) const {
    return (this->x == other.x) && (this->y == other.y);
}

bool Point::operator!=(const Point other) const {
    return (this->x != other.x) || (this->y != other.y);
}