#pragma once

#include "ofMain.h"
#include "ofxFlickr.h"
#include "Log.h"
#include "Fire.h"
#include "Fireplace.h"
#include "FireplacePlayer.h"
#include "ofxThreadedImageLoader.h"
#include "Triangulator.h"

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
		
        ofxFlickr::API flickrAPI;
        Log log;
        Fire fire;
        Fireplace fireplace;
        FireplacePlayer fireSound;
    
        ofxThreadedImageLoader loader;
        ofLight                 light;
    
        ofPixels                logPixels, firePixels, fireplacePixels;
        ofImage                 logPreRender, firePreRender, fireplacePreRender;
        ofImage                 logRender, fireRender, fireplaceRender;
    
        Triangulator            triangles;
};
