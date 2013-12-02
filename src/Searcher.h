//
//  Searcher.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxThreadedImageLoader.h"

class Searcher : protected ofThread, public ofRectangle {
public:
        
    int numImages;
    int imageIndex;
    
    Searcher(){
        lastUpdated = 0;
        updateRate  = 3000;
        lastChanged = 0;
        frame       = 0;
        changeRate  = 100;
        page = 0;
        currentImage = 0;
        shader.load("knockoutBlack");
        shader.begin();
        minDist = .75;
        shader.setUniform1f("minDist", minDist);
        shader.end();
        bFadeDir = true;
        numImages = 0;
        imageIndex = -1;
        frontImage = new ofImage();
        backImage  = new ofImage();
        changeRateMin = 10;
        changeRateMax = 200;
        bAddCorners = false;
    }
    
    
    void attachLoader( ofxThreadedImageLoader * _loader ){
        loader = _loader;
    }
    
    void setSearchTerm( string t ){
        term = t;
    }
    
    virtual void update(){
        if (imageIndex == -1 ){
            imageIndex++;
            backImage->loadImage( imagePaths[imageIndex] );
            changeRate = changeRates[imageIndex];
            imageIndex++;
            frontImage->loadImage( imagePaths[imageIndex]);
        }
        render();
    }
    
    void render(){
        if ( !fbo1.isAllocated() ){
            fbo1.allocate(width, height);
            fbo2.allocate(width, height);
            renderedFbo.allocate(width, height);
        }
        
        
//        lock();
        if ( frame - lastChanged >= changeRate ){
            lastChanged = frame;
            
            backImage->loadImage( imagePaths[imageIndex] );
            imageIndex++;;
            //bFadeDir = !bFadeDir;
            
            if (imageIndex >= imagePaths.size() ){
                imageIndex = 0;
            }
            frontImage->loadImage( imagePaths[imageIndex]);
            
            changeRate = changeRates[imageIndex];
        }
        
        ofPushStyle();
        fbo1.begin();
        ofClear(255,255,255, 0);
        ofSetColor(255);
        frontImage->draw(0,0, width, height);
        fbo1.end();
        
        fbo2.begin();
        ofClear(255,255,255,0);
        backImage->draw(0,0, width, height);
        fbo2.end();
        
        ofPopStyle();
    
//        unlock();
        frame++;
    }
    
    virtual void draw(){
        ofPushStyle();
//        ofRect(x-(width+20)/2.0f,y-(height+20)/2.0f, width+20, height+20);
        fbo1.getTextureReference().bind();
        fbo2.getTextureReference().bind();
        
        shader.begin();
        shader.setUniformTexture("tex1", fbo1.getTextureReference(), 1);//fbo1.getTextureReference().texData.textureID);
        shader.setUniformTexture("tex2", fbo2.getTextureReference(), 2);//fbo2.getTextureReference().texData.textureID);
        shader.setUniform1f("minDist", minDist);
        shader.setUniform1f("time", ofMap(frame - lastChanged, 0, changeRate, 0.0, 1.0, true));
        
//        if (term == "fire") cout << ofMap(frame - lastChanged, 0, changeRate, 0.0, 1.0, true) << endl;
        renderedFbo.begin();
        ofClear(255,255,255, 0);
        ofSetColor(tint);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);
        glTexCoord2f(width, 0);
        glVertex3f(width, 0, 0);
        glTexCoord2f(width, height);
        glVertex3f(width, height, 0);
        glTexCoord2f(0, height);
        glVertex3f(0, height, 0);
        glEnd();
        
        renderedFbo.end();
        shader.end();
        
        fbo1.getTextureReference().unbind();
        fbo2.getTextureReference().unbind();
        
//        shader.begin();
//        shader.setUniformTexture("tex", images[currentImage+1]->getTextureReference(), 1);
//        renderedFbo.begin();
////        ofClear(255,255,255, 0);
//        
//        ofSetColor(tint.r, tint.g, tint.b, ofMap(frame - lastChanged, 0, changeRate, 0, 255, true));
//        images[currentImage+1]->draw(0,0, width, height);
//        //fbo.draw(x-width/2.0f,y-height/2.0f);
//        renderedFbo.end();
//        shader.end();
//        ofPopStyle();
    }
    
    
    ofFbo & getFbo(){
        return renderedFbo;
    }
    
    void setHueDistance( float dist ){
        minDist = dist;
    }
    
    void addImagePath( string path ){
        imagePaths.push_back(path);
        changeRates.push_back(ofRandom(changeRateMin, changeRateMax));
    }
    
    bool bAddCorners;
    
protected:
    ofColor tint;
    ofFbo fbo1, fbo2, renderedFbo;
    ofxThreadedImageLoader * loader;
    ofShader shader;
    string media;
    
    vector<string>  imagePaths;
    vector<int>     changeRates;
    
    bool bFadeDir;
    
    float minDist;
    
    int lastUpdated, updateRate;
    int lastChanged, frame, changeRate, changeRateMin, changeRateMax;
    int currentImage;
    string term;
    
    ofImage * frontImage;
    ofImage * backImage;
    int page;
    
private:
    
};