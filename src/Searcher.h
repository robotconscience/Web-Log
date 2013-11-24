//
//  Searcher.h
//  LogFlickr
//
//  Created by BRenfer on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFlickr.h"
#include "ofxThreadedImageLoader.h"

class Searcher : protected ofThread, public ofRectangle {
public:
    
    Searcher(){
        lastUpdated = 0;
        updateRate  = 3000;
        lastChanged = 0;
        frame       = 0;
        changeRate  = 100;
        api = NULL;
        page = 0;
        currentImage = 0;
        media = "photos";
        shader.load("knockoutBlack");
    }
    
    void attachFlickr( ofxFlickr::API * _api ){
        api = _api;
        startThread();
    }
    
    void attachLoader( ofxThreadedImageLoader * _loader ){
        loader = _loader;
    }
    
    void setSearchTerm( string t ){
        term = t;
    }
    
    virtual void update(){
        lock();
        for (int i=0; i<mediaBuffer.size(); i++){
            currentMedia.push_back( mediaBuffer[i]);
        }
        mediaBuffer.clear();
        unlock();
        render();
    }
    
    void render(){
        if ( !fbo.isAllocated() ){
            fbo.allocate(width, height);
            renderedFbo.allocate(width, height);
        }
        lock();
        if ( images.size() > 1 ){
            fbo.begin();
            ofSetColor(0,0);
            ofRect(0,0,fbo.getWidth(), fbo.getHeight());
            ofSetColor(255);
            images[currentImage]->draw(0,0, width, height);
            ofSetColor(255,ofMap(frame - lastChanged, 0, changeRate, 0, 255, true));
            images[currentImage+1]->draw(0,0, width, height);
            ofSetColor(255);
            fbo.end();
            if ( frame - lastChanged > changeRate ){
                lastChanged = frame;
                currentImage++;
                if (currentMedia.size() > 0){
                    images.push_back( new ofImage());
                    loader->loadFromURL( *images.back(), currentMedia[0].getURL() );
                    currentMedia.erase(currentMedia.begin());
                }
                if ( currentImage + 1 >= images.size() || images[currentImage]->width == 0){
                    currentImage = 0;
                }
            }
            
        } else if (currentMedia.size() > 0){
            images.push_back( new ofImage());
            loader->loadFromURL( *images.back(), currentMedia[0].getURL() );
            currentMedia.erase(currentMedia.begin());
        }
        unlock();
        frame++;
    }
    
    virtual void draw(){
        ofPushStyle();
//        ofRect(x-(width+20)/2.0f,y-(height+20)/2.0f, width+20, height+20);
        shader.begin();
        shader.setUniformTexture("tex", fbo.getTextureReference(), 1);
        ofSetColor(tint);
        renderedFbo.begin();
        fbo.draw(0,0);
        //fbo.draw(x-width/2.0f,y-height/2.0f);
        renderedFbo.end();
        shader.end();
        ofPopStyle();
    }
    
    void addImage( ofImage & image ){
        images.push_back(&image);
    }
    
    void addImage( ofImage * image ){
        images.push_back(image);
    }
    
    ofFbo & getFbo(){
        return renderedFbo;
    }
    
protected:
    ofColor tint;
    ofFbo fbo, renderedFbo;
    ofxThreadedImageLoader * loader;
    ofShader shader;
    string media;
    
    int lastUpdated, updateRate;
    int lastChanged, frame, changeRate;
    int currentImage;
    string term;
    
    ofxFlickr::API * api;
    vector<ofxFlickr::Media> currentMedia, mediaBuffer;
    vector<ofImage *> images;
    int page;
    
    void threadedFunction(){
        while (isThreadRunning()){
            if ( ofGetElapsedTimeMillis() - lastUpdated > updateRate && api != NULL ){
                lastUpdated = ofGetElapsedTimeMillis();
                ofxFlickr::Query q;
                q.page = page;
                q.text = term;
                q.media = media;
                page++;
                vector<ofxFlickr::Media> media = api->search( q );
                
//                for (int i=0; i<media.size(); i++){
//                    media[i] = api->getMediaById(media[i].id);
//                }
                
                lock();
                for (int i=0; i<media.size(); i++){
                    mediaBuffer.push_back( media[i]);
                }
                unlock();
            }
            sleep(100);
            yield();
        }
    }
    
private:
    
};