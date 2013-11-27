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
#include "ofxVoronoi.h"

enum RenderMode {
    RENDER_DELAUNAY,
    RENDER_VORONOI,
    RENDER_SQUARES
};

class Triangulator : protected ofThread {
public:
    
    Triangulator(){
        bNeedToProcess = false;
        setThreshold(0.01);
        renderMode = RENDER_DELAUNAY;
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
        switch ( renderMode ){
            case RENDER_DELAUNAY:
                processDelaunay(i);
                break;
            case RENDER_VORONOI:
                processVoronoi( i );
                break;
                
            case RENDER_SQUARES:
                processDelaunay(i);
                processVoronoi( i );
                break;
        }
        ofImage * img  = render(i);
        return img;
    }
    
    void process( ofImage * from, ofImage * to ){
        switch ( renderMode ){
            case RENDER_DELAUNAY:
                processDelaunay(from);
                break;
            case RENDER_VORONOI:
                processVoronoi( from );
                break;
            case RENDER_SQUARES:
                processVoronoi( from );
                processDelaunay( from );
                break;
        }
        render(from, to);
    }
    
    void setThreshold( float thresh ){
        threshold = thresh;
    }
    
    void setRenderMode( RenderMode render ){
        renderMode = render;
    }
    
protected:
    float threshold;
    
    RenderMode renderMode;
    
    vector<ofImage*> queue;
    
    vector<Triangle> triangles;
    vector<ofPoint> points;
    ofxDelaunay delaunayProcessor;
    
    ofxVoronoi voronoiProcessor;
    vector<ofMesh> voronoiMesh;
    
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
        static ofFbo fbo;
        static ofFbo pongFbo;
        if ( fbo.getWidth() != from->width || fbo.getHeight() != from->height ){
            fbo.allocate( from->width, from->height, GL_RGBA, 8  );
            pongFbo.allocate( from->width, from->height, GL_RGBA, 8  );
        }
        //delaunayProcessor.draw();
        
        switch (renderMode){
            case RENDER_DELAUNAY:
                fbo.begin();
                ofClear(255,255,255, 0);
                ofSetColor(255);
                for ( int i=0, len = triangles.size(); i<len; i++){
                    triangles[i].draw();
                }
                ofSetColor(255);
                ofDisableBlendMode();
                fbo.end();
                break;
            case RENDER_VORONOI:
                fbo.begin();
                ofClear(255,255,255, 0);
                for ( int i=0, len = voronoiMesh.size(); i<len; i++){
                    voronoiMesh[i].draw();
                }
                ofSetColor(255);
                ofDisableBlendMode();
                fbo.end();
                break;
            case RENDER_SQUARES:
                pongFbo.begin();
                ofClear(255,255,255, 0);
                for ( int i=0, len = voronoiMesh.size(); i<len; i++){
                    voronoiMesh[i].drawWireframe();
                }
                pongFbo.end();
                fbo.begin();
                ofClear(255,255,255, 0);
                for ( int i=0, len = triangles.size(); i<len; i++){
                    triangles[i].draw();
                }
                ofSetColor(255,255,255,200);
                pongFbo.draw(0,0);
                
                fbo.end();
                ofSetColor(255);
                break;
        }
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
        delaunayProcessor.reset();
        triangles.clear();
        
        lock();
        // find important points
        findPoints( cImage );
        unlock();
        
        // process!
        for ( int i=0, len = points.size(); i<len; i++){
            // why do we have to do this ? :(
            float x = ofRandom(points[i].x,points[i].x+.1), y =  ofRandom(points[i].y,points[i].y+.1);
            delaunayProcessor.addPoint(x,y, 0 );
        }
        lock();
        if ( points.size() > 0 ){
            delaunayProcessor.triangulate();
        }
        unlock();
        // add colors to faces
        ofColor white;
        for ( int i=0, len = delaunayProcessor.triangleMesh.getNumVertices(); i<len; i++){
            delaunayProcessor.triangleMesh.addColor(white);
        }
        
        for ( int i=0, len = delaunayProcessor.triangleMesh.getNumIndices(); i<len; i+=3){
            ofIndexType i1 = delaunayProcessor.triangleMesh.getIndex( (ofIndexType) i);
            ofIndexType i2 = delaunayProcessor.triangleMesh.getIndex( (ofIndexType) i+1);
            ofIndexType i3 = delaunayProcessor.triangleMesh.getIndex( (ofIndexType) i+2);
            
            ofPoint p1 = delaunayProcessor.triangleMesh.getVertex( i1 );
            ofPoint p2 = delaunayProcessor.triangleMesh.getVertex( i2 );
            ofPoint p3 = delaunayProcessor.triangleMesh.getVertex( i3 );
            
            ofPoint center;
            center.x = (p1.x + p2.x + p3.x )/3.0;
            center.y = (p1.y + p2.y + p3.y )/3.0;
            
            ofColor c = cImage->getColor(round(center.x), round(center.y));
            //        c.a = 200;
            ofFloatColor toSet; toSet.set( c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f);
            delaunayProcessor.triangleMesh.setColor(i1, toSet);
            delaunayProcessor.triangleMesh.setColor(i2, toSet);
            delaunayProcessor.triangleMesh.setColor(i3, toSet);
            
            Triangle t = Triangle();
            t.setup( p1, p2, p3 );
            t.color = c;
            triangles.push_back(t);
        }
    }
    
    void processVoronoi( ofImage * cImage ){
        points.clear();
        voronoiProcessor.clear();
        voronoiProcessor.setBounds(0,0, cImage->width, cImage->height);
        
        lock();
        // find important points
        findPoints( cImage );
        unlock();
        
        // process!
        for ( int i=0, len = points.size(); i<len; i++){
            // why do we have to do this ? :(
            float x = ofRandom(points[i].x,points[i].x+.1), y =  ofRandom(points[i].y,points[i].y+.1);
            voronoiProcessor.addPoint(x,y);
        }
        
        lock();
        if ( points.size() > 0 ){
            voronoiProcessor.generateVoronoi();
        }
        unlock();
        
        voronoiMesh.clear();
        
        
        // add colors to faces
        ofColor white;
        
        for (int i=0; i < voronoiProcessor.cells.size(); i++){
            ofPath line;
            line.moveTo(voronoiProcessor.cells[i].pts[0]);
            for ( int j=1; j<voronoiProcessor.cells[i].pts.size(); j++){
                line.lineTo(voronoiProcessor.cells[i].pts[j]);
            }
            line.close();
            voronoiMesh.push_back(line.getTessellation());
            
            ofColor c = cImage->getColor(round(voronoiMesh.back().getCentroid().x), round(voronoiMesh.back().getCentroid().y));
            //        c.a = 200;
            ofFloatColor toSet; toSet.set( c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f);
            
            for ( int k=0; k < voronoiMesh.back().getNumVertices(); k++){
                voronoiMesh.back().addColor(c);
            }
            
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
