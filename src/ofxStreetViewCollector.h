//
//  ofxStreetViewCollector.h
//  emptyExample
//
//  Created by Roy Macdonald on 06-10-15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "SVPoint.h"

class ofxStreetViewCollector{
public:
    void setup(ofVec3f lim1, ofVec3f lim2);
    void update();
    void startCollectingData();
    void collectFromNextPoint();
    void exit();
    bool setPanoId(string _pano_id);
    void setLatLon(double _lat, double _lng);
    
    void urlResponse(ofHttpResponse & response);
    
    ofVec3f convertLatLon(ofVec3f v);
    void savePoint(SVPoint & p);
    void loadPoints(string path);
    void save();
    bool bRegister;
    string data_url;
    ofRectangle area;
    
    ofVec3f centroid, lim1, lim2;
    
    map<string, SVPoint> points;
    
    void loadAndCollect(string path);
    
    ofXml xml;
    bool bFirstPoint;
    
    bool bNewDataCollected;

    bool getPointLinks(SVPoint& p);

private:

    map< string, SVPoint>::iterator collectIterator;
    
};
