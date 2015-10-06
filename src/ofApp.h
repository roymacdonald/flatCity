#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxGrabCam.h"
#include "ofxDrawCamera.h"
#include "ofxGui.h"

class SVPoint{
public:
    SVPoint(){}//:bCheckedAll(false), bIsSet(false), bIsInside(false){}
    ofVec3f pos;
    double x, y, z;
    void setPos(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
        pos.set(x,y,z);
    }
    vector<string>links;
//    bool bCheckedAll, bIsSet, bIsInside;
    string ID;
};

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
  //  void threadedFunction();
    
    void        setPanoId(string _pano_id);
    void        setLatLon(double _lat, double _lng);
    
    void        urlResponse(ofHttpResponse & response);
    
    ofxGrabCam  cam;
//    ofEasyCam cam;
    ofMesh mesh;
    
    bool bRegister;
    string data_url;
    ofRectangle area, gridRect;

    map<string, SVPoint> points;
    void addPoint(SVPoint & p, bool bSave = true);
    ofVec3f convertLatLon(ofVec3f v);
    void savePoint(SVPoint & p);
    void loadPoints(string path);
    bool bMapPoints;
    
    void remapPoints();
    
    bool bUseCam;

    void rotatePoints();
    ofVec3f norm, dir, axis, mn, mx, mn2, mx2, centroid;
    ofVec3f lim1, lim2, origin;
    bool bDrawGrid;
    SVPoint* currentPoint;
    ofXml xml;
    bool bFirstPoint;
    
    bool bNewDataCollected;
    void startCollectingData();
};
