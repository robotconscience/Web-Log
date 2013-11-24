//
//  Triangle.h
//  BackgroundGenerator
//
//  Created by rockwell on 10/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

class Triangle
{
public:
    Triangle();
    
    void setup( ofPoint _p1, ofPoint _p2, ofPoint _p3);
    void draw();
    
    ofColor color;
    
    float getRadius(){ return radius; };
    ofPoint getCenter(){ return center; };
    
private:
    
    float   radius;
    ofPoint center;
    ofPoint p1, p2, p3;
    float   rotate;
    int     amount;
};
