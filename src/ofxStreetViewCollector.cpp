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
void ofxStreetViewCollector::loadAndCollect(string path){
    loadPoints(path);
    collectIterator = points.begin();
    startCollectingData();
    
}
//--------------------------------------------------------------
void ofxStreetViewCollector::startCollectingData(){
    bNewDataCollected = true;
    
    if (points.size() == 0) {
        bFirstPoint = true;
        setLatLon(40.714373, -74.006141);
    }else{
        collectFromNextPoint();
    }
}
//--------------------------------------------------------------
void ofxStreetViewCollector::collectFromNextPoint(){
    if (collectIterator != points.end()) {
        SVPoint p = collectIterator->second;
        
        ++collectIterator;
        if (!getPointLinks(p)) {
            collectFromNextPoint();
        }
    }else{
        cout << "COLLECT END!!!!" << endl;
    }
}
//--------------------------------------------------------------
bool ofxStreetViewCollector::getPointLinks(SVPoint& p){
    bool bFound = false;
    for (int i =0; i < p.links.size(); i++) {
        //cout << p.links[i] << endl;
        bFound |= setPanoId(p.links[i]);
    }
    return  bFound;
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
bool ofxStreetViewCollector::setPanoId(string _pano_id){
    if(points.count(_pano_id) == 0){
        if(!bRegister){
            ofRegisterURLNotification(this);
            bRegister = true;
        }
        data_url = "http://cbk0.google.com/cbk?output=xml&panoid="+_pano_id;
        ofLoadURLAsync(data_url);
        return true;
    }
    return false;
}
//--------------------------------------------------------------
void ofxStreetViewCollector::loadPoints(string path){
    ofXml xml;
    //  points.clear();
    if (xml.load(path)) {
        if(xml.exists("SVPoint")){
            xml.setTo("SVPoint[0]");
            do {
                if(xml.getName() == "SVPoint"){
                    SVPoint point;
                    point.ID = xml.getValue("id");
                    string pos = xml.getValue("pos");
                    vector<string>p = ofSplitString(pos, ", ");
                    
                    point.setPos(stringToDouble(p[0]), stringToDouble(p[1]), stringToDouble(p[2]));
                    if (xml.exists("links")) {
                        xml.setTo("links[0]");
                        if (xml.getName() == "links") {
                            if (xml.exists("link")) {
                                xml.setTo("link[0]");
                                do {
                                    if (xml.getName() == "link") {
                                        point.links.push_back(xml.getValue());
                                    }
                                }
                                while(xml.setToSibling());
                            }
                        }
                        xml.setToParent();
                        xml.setToParent();
                    }
                    if(points.count(point.ID) == 0){
                        savePoint(point);
                        points[point.ID] = point;
                    }
                }
            }
            while( xml.setToSibling() );
        }
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
        
        cout << "new response  "<< latS << "  lonS: " << lonS << "  elev: " << elevation;// << endl;
        
        if(points.count(pano_id) == 0){
            
            if (area.inside(lat, lon)) {
                cout << " adding :  " << pano_id ;
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
                
                if(!getPointLinks(p)){
                    collectFromNextPoint();
                }
            }else{
                points[pano_id] = SVPoint();
            }
        }
        cout << endl;
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