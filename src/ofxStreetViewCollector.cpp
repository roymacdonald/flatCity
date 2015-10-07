//
//  ofxStreetViewCollector.cpp
//  emptyExample
//
//  Created by Roy Macdonald on 06-10-15.
//
//

#include "ofxStreetViewCollector.h"
#include "ofxDoubleToStringHelper.h"

//--------------------------------------------------------------
void ofxStreetViewCollector::setup(ofVec3f lim1, ofVec3f lim2){
    
    bRegister = false;
    this->lim1 = lim1;
    this->lim2 = lim2;
    ofVec3f   origin = (lim1 + lim2)*0.5;
    area.x = MIN(lim1.x, lim2.x);
    area.y = MIN(lim1.y, lim2.y);
    area.width = MAX(lim1.x, lim2.x) - area.x;
    area.height = MAX(lim1.y, lim2.y) - area.y;
    
    xml.addChild("Points");
    xml.setTo("//Points");
    
    bNewDataCollected = false;
    
}
//--------------------------------------------------------------
void ofxStreetViewCollector::startCollectingData(){
    bNewDataCollected = true;
    
    if (points.size() == 0) {
        bFirstPoint = true;
        setLatLon(40.7304056,-74.0000006);
    }else{
        map< string, SVPoint>::iterator it = points.end();
        it--;
        SVPoint p = it->second;
        for (int i =0; i < p.links.size(); i++) {
            cout << p.links[i] << endl;
            setPanoId(p.links[i]);
        }
    }
}

//--------------------------------------------------------------
void ofxStreetViewCollector::setLatLon(double _lat, double _lon){
    bNewDataCollected = true;
    if(!bRegister){
        ofRegisterURLNotification(this);
        bRegister = true;
    }
    
    data_url = "http://cbk0.google.com/cbk?output=xml&ll="+ofToString(_lat)+","+ofToString(_lon);
    ofLoadURLAsync(data_url);
}
//--------------------------------------------------------------
void ofxStreetViewCollector::setPanoId(string _pano_id){
    if(points.count(_pano_id) == 0){
        if(!bRegister){
            ofRegisterURLNotification(this);
            bRegister = true;
        }
        data_url = "http://cbk0.google.com/cbk?output=xml&panoid="+_pano_id;
        ofLoadURLAsync(data_url);
    }
}
//--------------------------------------------------------------
void ofxStreetViewCollector::urlResponse(ofHttpResponse & response){
    
    
    if(response.status==200){// && (response.request.url == data_url )){
        
        ofxXmlSettings  XML;
        XML.loadFromBuffer(response.data);
        
        string pano_id = XML.getAttribute("panorama:data_properties", "pano_id", "");
        
        string latS = XML.getAttribute("panorama:data_properties", "original_lat", "0.0");
        string lonS = XML.getAttribute("panorama:data_properties", "original_lng", "0.0");
        string eleS = XML.getAttribute("panorama:data_properties", "elevation_egm96_m", "-1.0");//"elevation_wgs84_m"
        double lat = stringToDouble(latS);
        double lon = stringToDouble(lonS);
        double elevation = stringToDouble(eleS);
        
        
        if(points.count(pano_id) == 0){
            if (area.inside(lat, lon)) {
                SVPoint  p;
                XML.pushTag("panorama");
                XML.pushTag("annotation_properties");
                int nLinks = XML.getNumTags("link");
                for (int i = 0; i < nLinks; i++) {
                    p.links.push_back(XML.getAttribute("link", "pano_id", ""));
                    XML.removeTag("link",0);
                }
                XML.popTag();
                XML.popTag();
                
                p.setPos(lat, lon, elevation);
                p.ID = pano_id;
                points[pano_id] = p;
                
                savePoint(p);
                for (int i = 0; i < p.links.size(); i++) {
                    setPanoId(p.links[i]);
                }
            }else{
                points[pano_id] = SVPoint();
            }
        }
    }
}
//--------------------------------------------------------------
void ofxStreetViewCollector::savePoint(SVPoint & p){
    
    ofXml x;
    x.addChild("SVPoint");
    x.setTo("//SVPoint");
    x.addValue("pos", doubleToString(p.x) +", "+ doubleToString(p.y) +", "+doubleToString(p.z));
    x.addValue("id", p.ID);
    x.addChild("links");
    x.setTo("links");
    for(int i = 0; i < p.links.size(); i++) {
        x.addValue("link", p.links[i]);
    }
    x.setTo("//SVPoint");
    xml.addXml(x);
}
//--------------------------------------------------------------
void ofxStreetViewCollector::save(){
    if (bNewDataCollected) {
        xml.setTo("//Points");
        xml.save(ofGetTimestampString()+".xml");
    }
}