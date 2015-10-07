#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxGrabCam.h"
#include "ofxDrawCamera.h"
#include "ofxGui.h"
#include "SVPoint.h"
#include "ofxDoubleToStringHelper.h"
#include "ofxStreetViewCollector.h"

class ofApp : public ofBaseApp{//, public ofThread{
	public:
		void setup();
		void update();
		void draw();
    void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    ofxStreetViewCollector collector;
    
    
    ofxGrabCam  cam;

    ofMesh mesh;
    

    bool bUseCam;
    map<string, SVPoint> points;
    void addPoint(SVPoint & p);
    ofVec3f convertLatLon(ofVec3f v);
    void loadPoints(string path);
    
    
    ofNode node;
    
    void rotatePoints();
    ofVec3f norm, dir, axis, mn, mx, mn2, mx2, centroid;
    ofVec3f lim1, lim2, origin;
    bool bDrawGrid;

};
