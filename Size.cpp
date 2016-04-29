//
//  Size.cpp
//  poisson
//

#include "Size.hpp"

Size::Size(unsigned int height, unsigned int width) {
    this->height = height;
    this->width = width;
}

Size::Size() {
    height = 0;
    width = 0;
}