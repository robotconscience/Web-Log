//
//  Log.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "Searcher.h"

class Log : public Searcher {
public:
    
    Log() : Searcher() {
        term = "log";
        x = 960;
        y = 782;
        width = 642;
        height = 174;
        tint.set(97,59,0);
        cylinder.set(50, 550, 20, 20);
    }
    
    /*void draw(){
        Searcher::draw();
        ofPushMatrix();
        //ofTranslate(x,y,-50);
        ofRotateZ(90);
        ofRotateX( ofGetElapsedTimeMillis() * .1 );
        fbo.getTextureReference().bind();
        cylinder.draw();
        fbo.getTextureReference().unbind();
        ofPopMatrix();
    }*/
    
    ofCylinderPrimitive cylinder;
};
