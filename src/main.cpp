#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
#ifdef TARGET_OSX
    //this allows to embed all the data folder inside the app bundle.
    string newRoot = "../Resources/data/";
    ofEnableDataPath();
    ofSetDataPathRoot(newRoot);
#endif

	ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp());

}
