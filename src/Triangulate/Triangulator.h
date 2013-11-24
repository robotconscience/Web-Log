//
//  Triangulator.h
//  LogFlickr
//
//  Created by BRenfer on 11/17/13.
//
//

#pragma once
#include "ofMain.h"
#include "Triangle.h"
#include "ofxCv.h"
#include "ofxDelaunay.h"

class Triangulator : protected ofThread {
public:
    
    Triangulator(){
        bNeedToProcess = false;
        setThreshold(0.01);
    }
    
    // incomplete
    void processThreaded( ofImage * i ){
        lock();
        queue.push_back(i);
        unlock();
        
        if (!isThreadRunning()){
            startThread();
        }
    }
    
    ofImage * process( ofImage * i ){
        processDelaunay(i);
        ofImage * img  = render(i);
        return img;
    }
    
    void process( ofImage * from, ofImage * to ){
        processDelaunay(from);
        render(from, to);
    }
    
    void setThreshold( float thresh ){
        threshold = thresh;
    }
    
protected:
    float threshold;
    
    vector<ofImage*> queue;
    
    vector<Triangle> triangles;
    vector<ofPoint> points;
    ofxDelaunay triangulator;
    
    void threadedFunction(){
        while (!isThreadRunning()){
            if ( queue.size() > 0 ){
                processDelaunay( queue[0]);
                lock();
                queue.erase(queue.begin());
                unlock();
            }
            yield();
            ofSleepMillis(50);
        }
    }
    
    void render( ofImage * from, ofImage * to ){
        static ofPixels pix;
        ofFbo fbo; fbo.allocate( from->width, from->height, GL_RGBA, 4  );
        fbo.begin();
        ofSetColor(0);
        ofRect(0, 0, from->getWidth(), from->getHeight() );
        ofSetColor(255);
        triangulator.draw();
        
        for ( int i=0, len = triangles.size(); i<len; i++){
            triangles[i].draw();
        }
        /*switch (blendMode){
         case 1:
         ofEnableBlendMode(OF_BLENDMODE_ADD);
         break;
         case 2:
         ofEnableBlendMode(OF_BLENDMODE_ALPHA);
         break;
         case 3:
         ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
         break;
         case 4:
         ofEnableBlendMode(OF_BLENDMODE_SCREEN);
         break;
         case 5:
         ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
         break;
         case 0:
         default:
         ofDisableBlendMode();
         ofEnableAlphaBlending();
         break;
         }*/
        ofSetColor(255);
        ofDisableBlendMode();
        fbo.end();
        fbo.readToPixels(pix);
        to->setFromPixels(pix);
    }
    
    ofImage * render( ofImage * src ){
        ofImage * ret = new ofImage();
        ret->allocate( src->width, src->height, (ofImageType) src->type);
        render( src, ret );
        return ret;
    }
    
    void processDelaunay( ofImage * cImage ){
        points.clear();
        triangulator.reset();
        triangles.clear();
        
        lock();
        // find important points
        findPoints( cImage );
        unlock();
        
        // process!
        for ( int i=0, len = points.size(); i<len; i++){
            // why do we have to do this ? :(
            float x = ofRandom(points[i].x,points[i].x+.1), y =  ofRandom(points[i].y,points[i].y+.1);
            triangulator.addPoint(x,y, 0 );
        }
        lock();
        if ( points.size() > 0 ){
            triangulator.triangulate();
        }
        unlock();
        // add colors to faces
        ofColor white;
        for ( int i=0, len = triangulator.triangleMesh.getNumVertices(); i<len; i++){
            triangulator.triangleMesh.addColor(white);
        }
        
        for ( int i=0, len = triangulator.triangleMesh.getNumIndices(); i<len; i+=3){
            ofIndexType i1 = triangulator.triangleMesh.getIndex( (ofIndexType) i);
            ofIndexType i2 = triangulator.triangleMesh.getIndex( (ofIndexType) i+1);
            ofIndexType i3 = triangulator.triangleMesh.getIndex( (ofIndexType) i+2);
            
            ofPoint p1 = triangulator.triangleMesh.getVertex( i1 );
            ofPoint p2 = triangulator.triangleMesh.getVertex( i2 );
            ofPoint p3 = triangulator.triangleMesh.getVertex( i3 );
            
            ofPoint center;
            center.x = (p1.x + p2.x + p3.x )/3.0;
            center.y = (p1.y + p2.y + p3.y )/3.0;
            
            ofColor c = cImage->getColor(round(center.x), round(center.y));
            //        c.a = 200;
            ofFloatColor toSet; toSet.set( c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f);
            triangulator.triangleMesh.setColor(i1, toSet);
            triangulator.triangleMesh.setColor(i2, toSet);
            triangulator.triangleMesh.setColor(i3, toSet);
            
            Triangle t = Triangle();
            t.setup( p1, p2, p3 );
            t.color = c;
            triangles.push_back(t);
        }
    }
    
    void findPoints( ofImage * ci ){
        static cv::Mat toProcess;
        toProcess = ofxCv::toCv(ci->getPixelsRef());
        
        vector<cv::KeyPoint> objectKeypoints;
        
        // Extract key points
        //    The detector can be any of (see OpenCV features2d.hpp):
        //    Ôªøcv::FeatureDetector * detector = new cv::DenseFeatureDetector();
        //    Ôªøcv::FeatureDetector * detector = new cv::FastFeatureDetector();
        //    Ôªøcv::FeatureDetector * detector = new cv::GFTTDetector();
        //    cv::FeatureDetector * detector = new cv::MSER();
        //    Ôªøcv::FeatureDetector * detector = new cv::ORB();
        //    Ôªøcv::FeatureDetector * detector = new cv::StarFeatureDetector();
        //    Ôªøcv::FeatureDetector * detector = new cv::SURF(600.0);
        const cv::SIFT::DetectorParams& detectorParams = cv::SIFT::DetectorParams(threshold, 10.0);
        
        cv::FeatureDetector * detector = new cv::SiftFeatureDetector( detectorParams );
        detector->detect(toProcess, objectKeypoints);
        delete detector;
        
        // convert to ofPoint vector
        
        for ( int i=0, len = objectKeypoints.size(); i<len; i++){
            points.push_back( ofPoint( objectKeypoints[i].pt.x, objectKeypoints[i].pt.y ) );
        }
    }
    
    bool bNeedToProcess;
    
};
