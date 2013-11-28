#pragma once

#include "ofMain.h"
#include "Log.h"
#include "Fire.h"
#include "Fireplace.h"
#include "FireplacePlayer.h"
#include "ofxThreadedImageLoader.h"
#include "Triangulator.h"

#include "ofxUI.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        Log log;
        Fire fire;
        Fireplace fireplace;
        FireplacePlayer fireSound;
    
        // rendering stuff
        ofFbo screenFbo;
        ofPixels renderPix;
        int frameCount;
    
        ofxThreadedImageLoader loader;
        ofLight                 light;
    
        ofPixels                logPixels, firePixels, fireplacePixels;
        ofImage                 logPreRender, firePreRender, fireplacePreRender;
        ofImage                 logRender, fireRender, fireplaceRender;
        ofImage                 logSmoothRender, fireSmoothRender, fireplaceSmoothRender;
        Triangulator            triangleLog, triangleFire, triangleFireplace;
    
        ofxUICanvas             * gui;
        void guiEvent(ofxUIEventArgs &e);
    
        // gui vars
        int blurAmount;
        bool bRendering, bTriangulate, bTriangulateOnce, bDebug, bRaw;
        float flameChangeRate, logChangeRate, fireplaceChangeRate;
        float flameHueDist, logHueDist, fireplaceHueDist;
        float flameThresh, logThresh, fireplaceThresh;
    
        RenderMode renderMode;
    
        bool bRenderFireplace;
    
        float frame;
};


