//
//  Size.cpp
//  poisson
//
//  Created by Priyatham Kattakinda on 19/04/16.
//  Copyright © 2016 Priyatham Kattakinda. All rights reserved.
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