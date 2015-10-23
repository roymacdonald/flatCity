#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxGrabCam.h"
#include "ofxDrawCamera.h"
#include "ofxGui.h"
#include "SVPoint.h"
#include "ofxDoubleToStringHelper.h"
#include "ofxStreetViewCollector.h"
#include "ofxAutoReloadedShader.h"
#include "TravelingCam.h"
#include "pathEditor.h"

#include "ofQuaternionD.h"
#include "ofMatrix4x4D.h"
#include "ofVec3d.h"

//#include "ofxSSAO.h"

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
    bool bEnableFog;
    TravelingCam tCam;
   bool bUseTravelingCam;
    ofMesh mesh, lineMesh, normalsMesh;
  
    ofLight light;
    
    void buildNormals();
    
//    ofxSSAO ssao;
    
    pathEditor pathEdit;
    
    void buildMesh();
    
    void moveMeshPointsToOrigin();
    void movePointToOrigin(ofVec3f& p);
    vector<ofVec3f> meshPoints;
    
    bool bUseCam;
    map<string, SVPoint> points;
    void addPoint(SVPoint & p);
    ofVec3f convertLatLon(ofVec3f v);
    void loadPoints(string path);

    ofxPanel gui;
    ofParameter<float>thickness, heightMult, fogStart, fogEnd;
    ofParameter<ofFloatColor>fogColor;
    void heightMultChanged(float& f);
    ofVec3f norm, dir, axis, mn, mx, mn2, mx2, centroid;
    ofVec3f lim1, lim2, origin;
    bool bDrawGrid;
    ofxAutoReloadedShader shader;
    bool doShader;
    string info;
    vector<ofVec3f>closest;
    //*/
};
