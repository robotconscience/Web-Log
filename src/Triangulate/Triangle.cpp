//
//  Triangle.cpp
//  BackgroundGenerator
//
//  Created by rockwell on 10/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Triangle.h"

/**
	<#Description#>
	@returns <#return value description#>
 */
Triangle::Triangle()
{
    rotate = 0;//ofRandom(-360,360);
    amount = 1;//ofRandom(2,6);
};

/**
	<#Description#>
	@param _p1 <#_p1 description#>
	@param _p2 <#_p2 description#>
	@param _p3 <#_p3 description#>
 */
void Triangle::setup( ofPoint _p1, ofPoint _p2, ofPoint _p3)
{
    p1 = _p1; p2 = _p2; p3 = _p3;
    center.x = (p1.x + p2.x + p3.x )/3.0;
    center.y = (p1.y + p2.y + p3.y )/3.0;
    radius = 0;
    
    ofPoint p1MCenter = p1 - center;
    ofPoint p2MCenter = p2 - center;
    ofPoint p3MCenter = p3 - center;
    
    float xMin = fmin( p1MCenter.x, fmin(p2MCenter.x, p3MCenter.x) );
    float xMax = fmax( p1MCenter.x, fmax(p2MCenter.x, p3MCenter.x) );
    float yMin = fmin( p1MCenter.y, fmin(p2MCenter.y, p3MCenter.y) );
    float yMax = fmax( p1MCenter.y, fmax(p2MCenter.y, p3MCenter.y) );
    radius = fmax( xMax - xMin, yMax - yMin);
};

/**
	<#Description#>
 */
void Triangle::draw()
{
    for ( int i=0; i<amount; i++){
        ofPushMatrix();{
            ofTranslate(center.x, center.y);
            ofRotate(rotate * i/amount);
            ofSetColor(color);
            ofTriangle(p1-center,p2-center,p3-center);
        } ofPopMatrix();
    }
};