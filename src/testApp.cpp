#include "testApp.h"

static string API_KEY = "912826c8fca6f1b850b239ae96cdbe76";
static string API_SEC = "7bf2405ff29ae266";

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
//    ofEnableLighting();
//    light.enable();
    
    // add flames
    ofDirectory flames;
    int nflames = flames.listDir("flame");
    for (int i=0; i<nflames; i++){  
        ofImage * img = new ofImage();
        img->loadImage(flames.getPath(i));
        fire.addImage( img );
    }
    
    ofDirectory logs;
    int nLogs = logs.listDir("log");
    for (int i=0; i<nLogs; i++){
        ofImage * img = new ofImage();
        img->loadImage(logs.getPath(i));
        log.addImage( img );
    }
    
    ofDirectory fireplaces;
    int nFp = fireplaces.listDir("fireplace");
    for (int i=0; i<nFp; i++){
        ofImage * img = new ofImage();
        img->loadImage(fireplaces.getPath(i));
        fireplace.addImage( img );
    }
    
//    screenFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
}

//--------------------------------------------------------------
void testApp::update(){
    fireplace.update();
    fire.update();
    log.update();
    fireSound.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    //ofSetupScreenOrtho();
    //screenFbo.begin();
    fireplace.draw();
    fire.draw();
//    ofEnableDepthTest();
    log.draw();
//    ofDisableDepthTest();
    //screenFbo.end();
    
    bool bT = true;
    
    if (bT){
        
        log.getFbo().readToPixels(logPixels);
        fire.getFbo().readToPixels(firePixels);
        fireplace.getFbo().readToPixels(fireplacePixels);
        
        logPreRender.setFromPixels(logPixels);
        firePreRender.setFromPixels(firePixels);
        fireplacePreRender.setFromPixels(fireplacePixels);
        
        triangles.setThreshold(.02);
        triangles.process(&logPreRender, &logRender);
        triangles.setThreshold(.01);
        triangles.process(&firePreRender, &fireRender);
        triangles.setThreshold(.05);
        triangles.process(&fireplacePreRender, &fireplaceRender);
        
        ofScale(.5,.5);
        
        fireplaceRender.draw( fireplace.x - fireplace.width/2.0, fireplace.y - fireplace.height/2.0 );
        logRender.draw( log.x - log.width/2, log.y - log.height/2);
        fireRender.draw( fire.x- fire.width/2.0, fire.y - fire.height/2.0 );
        ofSaveFrame();
    } else {
//        screenFbo.draw(0,0);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
