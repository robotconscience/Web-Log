#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "testApp.h"

//========================================================================
int main( ){
    ofAppGlutWindow window; // create a window
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
    window.setGlutDisplayString("rgba double samples>=8 depth");
	ofSetupOpenGL(&window, 960, 540, OF_WINDOW);
	ofRunApp(new testApp()); // start the app

}
