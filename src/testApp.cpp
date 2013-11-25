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
    
    int maxLoad = 10000;
    
    // add flames
    ofDirectory flames;
    int nflames = min(maxLoad,flames.listDir("flame"));
    fire.numImages = nflames;
    
    for (int i=0; i<nflames; i++){
        fire.addImagePath( flames.getPath(i) );
    }
    
    ofDirectory logs;
    int nLogs = min(maxLoad, logs.listDir("log"));
    log.numImages = nLogs;
    
    for (int i=0; i<nLogs; i++){
        log.addImagePath( logs.getPath(i) );
    }
    
    ofDirectory fireplaces;
    int nFp = min(maxLoad, fireplaces.listDir("fireplace"));
    fireplace.numImages = nFp;
    
    for (int i=0; i<nFp; i++){
        fireplace.addImagePath( fireplaces.getPath(i) );
    }

    screenFbo.allocate(1920, 1080, GL_RGBA, 4);
    
    // gui setup
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer(length-xInit, 1);
	gui->addWidgetDown(new ofxUILabel("SET RENDERING", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown( new ofxUIToggle("rendering", false, dim*.25, dim*.25));
    gui->addSpacer(length-xInit, 1);
	gui->addWidgetDown(new ofxUILabel("DO TRIANGULATION", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown( new ofxUIToggle("triangulate", false, dim*.25, dim*.25));
    gui->addWidgetDown( new ofxUIButton("triangulate_once", false, dim*.25, dim*.25));
    gui->addWidgetDown( new ofxUIToggle("debug", false, dim*.25, dim*.25));
    gui->addSpacer(length-xInit, 1);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->addSlider("flame_dist", 0.0, 1.0, .75);
    gui->addSlider("log_dist", 0.0, 1.0, .75);
    gui->addSlider("fireplace_dist", 0.0, 1.0, .75);
    gui->addSpacer(length-xInit, 1);
	gui->addWidgetDown(new ofxUILabel("BG BLUR", OFX_UI_FONT_MEDIUM));
    gui->addSlider("blurAmount", 0.0, 255.0f, 100.0f);
    gui->addSpacer(length-xInit, 1);
	gui->addWidgetDown(new ofxUILabel("THRESHOLDS", OFX_UI_FONT_MEDIUM));
    gui->addSlider("flameThresh", 0.0, 1.0, .1);
    gui->addSlider("logThresh", 0.0, 1.0, .01);
    gui->addSlider("fireplaceThresh", 0.0, 1.0, .1);
    
    flameChangeRate, logChangeRate, fireplaceChangeRate;
    flameThresh, logThresh, fireplaceThresh;
    
    bRendering = false;
    bTriangulate = false;
    bDebug = false;
    bTriangulateOnce = true;
    blurAmount = 200;
    
    flameThresh = 0.01;
    logThresh = .02;
    fireplaceThresh = .05;
    
    ofSetLogLevel(OF_LOG_ERROR);
    gui->loadSettings("settings.xml");
    
    fireplace.width = 1920 * 1.5;
    fireplace.height = 1080 * 1.5;
    
    frame = 0;
}

//--------------------------------------------------------------
void testApp::update(){
    if (bTriangulate || bTriangulateOnce || bDebug){
        fireplace.update();
        fire.update();
        log.update();
        //fireSound.update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if ( bTriangulateOnce || bTriangulate || bDebug ){
        fireplace.draw();
        fire.draw();
        log.draw();
    }
    if (bTriangulate || bTriangulateOnce){
        ofSetColor(255);
        
        bTriangulateOnce = false;
        log.getFbo().readToPixels(logPixels);
        fire.getFbo().readToPixels(firePixels);
        fireplace.getFbo().readToPixels(fireplacePixels);
        
        logPreRender.setFromPixels(logPixels);
        firePreRender.setFromPixels(firePixels);
        fireplacePreRender.setFromPixels(fireplacePixels);
        
        triangleLog.setThreshold(logThresh + sin(frame) * logThresh * .1 );
        triangleLog.process(&logPreRender, &logRender);
        triangleFire.setThreshold(flameThresh + sin(frame) * flameThresh * .1 );
        triangleFire.process(&firePreRender, &fireRender);
        triangleFireplace.setThreshold(fireplaceThresh + sin(frame) * fireplaceThresh * .1 );
        triangleFireplace.process(&fireplacePreRender, &fireplaceRender);
        
        ofxCv::blur(fireplaceRender, blurAmount);
        fireplaceRender.update();
        
        frame += .01;
    }
    
    screenFbo.begin();
    ofClear(255,255,255, 0);
    ofSetColor(0);
    ofRect(0,0,1920,1080);
    ofSetColor(255);
    fireplaceRender.draw( fireplace.x - fireplace.width/2.0, fireplace.y - fireplace.height/2.0 );
    logRender.draw( log.x - log.width/2, log.y - log.height/2);
    fireRender.draw( fire.x- fire.width/2.0, fire.y - fire.height/2.0 );
    screenFbo.end();
    
    if (bRendering){
        screenFbo.readToPixels(renderPix);
        
        string name = ofToString(frameCount);
        while ( name.length() < 4 ){
            name = "0"+name;
        }
        
        ofSaveImage(renderPix, name+".png" );
        frameCount++;
    }
    
    if (bDebug){
        ofPushMatrix();
        ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0);
        ofScale(.5,.5);
        ofSetColor(255);
        fireplace.getFbo().draw(fireplace.x,fireplace.y);
        fire.getFbo().draw(fire.x,fire.y);
        log.getFbo().draw(log.x,log.y);
        ofPopMatrix();
    } else {
        ofPushMatrix();
        ofScale(.5,.5);
        screenFbo.draw(0,0);
        ofPopMatrix();
    }
    
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
    
	if(name == "rendering")
	{
		ofxUIToggle *btn = (ofxUIToggle *) e.widget;
        bRendering = btn->getValue();
	} else if(name == "triangulate"){
		ofxUIToggle *btn = (ofxUIToggle *) e.widget;
        bTriangulate = btn->getValue();
	}else if(name == "triangulate_once") {
        bTriangulateOnce = true;
	} else if ( name == "flame_dist"){
        fire.setHueDistance(((ofxUISlider*)e.widget)->getValue() * 5.0f);
    } else if ( name == "log_dist"){
        log.setHueDistance(((ofxUISlider*)e.widget)->getValue() * 5.0f);
    } else if ( name == "fireplace_dist"){
        fireplace.setHueDistance(((ofxUISlider*)e.widget)->getValue() * 5.0f);
    } else if ( name == "debug"){
        bDebug = ((ofxUIToggle *) e.widget)->getValue();
    } else if ( name == "blurAmount" ){
        blurAmount = (int) ((ofxUISlider*)e.widget)->getScaledValue();
    } else if ( name == "flameThresh" ){
        flameThresh = ((ofxUISlider*)e.widget)->getScaledValue();
    }  else if ( name == "logThresh" ){
        logThresh = ((ofxUISlider*)e.widget)->getScaledValue();
    }  else if ( name == "fireplaceThresh" ){
        fireplaceThresh = ((ofxUISlider*)e.widget)->getScaledValue();
    }
    cout << blurAmount << endl;
    
    gui->saveSettings("settings.xml");
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == 'g')
    {
        gui->toggleVisible();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}
