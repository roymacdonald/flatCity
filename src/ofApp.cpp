#include "ofApp.h"

#define EARTH_RADIUS 6366197

double stringToDouble(string s) {
    
    double d;
    stringstream cur;
    cur.precision(10);
    cur << s;
    cur >> d;
    return d;
    
}
float stringToFloat(string s) {
    
    float f;
    stringstream cur;
    cur.precision(10);
    cur << s;
    cur >> f;
    return f;
    
}

string doubleToString(double d){
    string s;
    stringstream cur;
    cur.precision(10);
    cur << d;
    cur >> s;
    return s;
}
string floatToString(float d){
    string s;
    stringstream cur;
    cur.precision(10);
    cur << d;
    cur >> s;
    return s;
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    bRegister = false;
    currentPoint = NULL;
    
    
    lim1.set(40.911773, -74.005716);//(40.74171, -74.0086659);
    lim2.set(40.649392,-73.945211);//(40.7027061, -73.9875523);
    origin = (lim1 + lim2)*0.5;
    centroid.set(0);
    centroid = convertLatLon(origin);
    area.x = MIN(lim1.x, lim2.x);
    area.y = MIN(lim1.y, lim2.y);
    area.width = MAX(lim1.x, lim2.x) - area.x;
    area.height = MAX(lim1.y, lim2.y) - area.y;
    mesh.setMode(OF_PRIMITIVE_LINES);
    //  mesh.disableColors();
    //  cout << "area: " << area << endl;
    
    
    bMapPoints = false;
    bUseCam = true;
    xml.addChild("Points");
    xml.setTo("//Points");
    
    bNewDataCollected = false;
    bDrawGrid  = true;
    // setLatLon(40.7304056,-74.0000006);
    
}
//--------------------------------------------------------------
void ofApp::rotatePoints(){
    
    
    ofVec3f centroid = mesh.getCentroid();
    
    float angle = centroid.getNormalized().angle(ofVec3f(0,1,0));
    //*
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    //*/
    //    centroid = mesh.getCentroid();
    
    centroid = mesh.getCentroid();
    axis = centroid.getNormalized().getCrossed(ofVec3f(0,1,0)).normalize();
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i].rotate(angle, ofVec3f(0), axis);
        mesh.getVertices()[i] -= centroid;
    }
    centroid = mesh.getCentroid();
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    
    /*
    mn.set(FLT_MAX), mx.set(FLT_MIN);
    for (int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f& v = mesh.getVertices()[i];
        if (mx.x < v.x || mx.y < v.y || mx.z < v.z) {
            mx = v;
        }
        if (mn.x > v.x || mn.x > v.y || mn.z > v.z) {
            mn = v;
        }
    }
    dir = mx - mn;
    
    //    if (1.0 - dir.dot(ofVec3f(0,1,0)) > FLT_EPSILON) {
    axis = dir.getCrossed(ofVec3f(0, 0, 1));
    norm = dir.getCrossed(axis);
    
    centroid = mesh.getCentroid();
    angle = dir.angle(ofVec3f(0,0, 1));
    for (int i = 0; i < mesh.getNumVertices(); i++) {
        mesh.getVertices()[i] = mesh.getVertices()[i].rotate(angle, centroid, axis) - centroid;
    }
   
    cout<< "Min: " << mn << "  Max: " << mx << "  mx - mn: " << mx - mn << "  Dir: " << dir << "  Axis: " << axis << " Norm: "<<norm << endl;
    
    //  }
    //*/
    
}
//--------------------------------------------------------------
void ofApp::startCollectingData(){
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
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(mesh.getNumVertices()), 10, 30);
    
    if (bUseCam) {
        cam.begin();
    }
    /*
     if(!bMapPoints){
     ofPushMatrix();
     ofScale(ofGetWidth()/area.getWidth(), ofGetHeight()/area.getHeight());
     ofTranslate(-area.getX(), -area.getY());
     }
     //    glPointSize(10);
     //*/
    ofSetColor(255);
    mesh.draw();
    if(mesh.getNumVertices()){
        ofPushMatrix();
        ofTranslate(mesh.getCentroid());
        ofDrawAxis(3);
        ofPopMatrix();
        ofPushStyle();
        ofSetLineWidth(4);
        ofSetColor(ofColor::magenta);
        ofDrawLine(ofVec3f(0), axis);
        ofSetColor(ofColor::cyan);
        ofVec3f c =mesh.getCentroid();
        ofDrawLine(c + c.getNormalized()*1000, c);
        ofSetColor(ofColor::yellow);
        ofDrawLine(ofVec3f(0), norm);
        ofSetLineWidth(6);
        ofSetColor(ofColor::red);
        ofDrawLine(mn,mx);
        ofSetColor(ofColor::blue);
        ofDrawLine(mn2,mx2);
        
        ofPopStyle();
    }
    //ofxDrawCamera::draw(cam);
    if(bDrawGrid){
        ofDrawGrid(1, 20, true);
    }
    ofPopMatrix();
    if(bUseCam){
        cam.end();
    }
    
    //*/
}
//--------------------------------------------------------------
void ofApp::setLatLon(double _lat, double _lon){
    bNewDataCollected = true;
    if(!bRegister){
        ofRegisterURLNotification(this);
        bRegister = true;
    }
    
    data_url = "http://cbk0.google.com/cbk?output=xml&ll="+ofToString(_lat)+","+ofToString(_lon);
    ofLoadURLAsync(data_url);
}
//--------------------------------------------------------------
void ofApp::setPanoId(string _pano_id){
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
void ofApp::urlResponse(ofHttpResponse & response){
    
    //  cout << "Status: " << response.status << endl;
    //  cout << "Request_url: " << response.request.url << endl;
    
    if(response.status==200){// && (response.request.url == data_url )){
        
        ofxXmlSettings  XML;
        XML.loadFromBuffer(response.data);
        
        //cout << "-----------------------------------------------------------------------------------" << endl;
        // cout << response.data.getText() << endl;
        
        string pano_id = XML.getAttribute("panorama:data_properties", "pano_id", "");
        
        
        
        string latS = XML.getAttribute("panorama:data_properties", "original_lat", "0.0");
        string lonS = XML.getAttribute("panorama:data_properties", "original_lng", "0.0");
        string eleS = XML.getAttribute("panorama:data_properties", "elevation_egm96_m", "-1.0");//"elevation_wgs84_m"
        double lat = stringToDouble(latS);
        double lon = stringToDouble(lonS);
        double elevation = stringToDouble(eleS);
        
        // cout << "lat: " << latS << "  :: lon: " << lonS << "  :: elevation: " << eleS  << endl;
        
        
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
                //  p.bIsInside = true;
                p.ID = pano_id;
                points[pano_id] = p;
                
                addPoint(p);
                
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
void ofApp::loadPoints(string path){
    xml.clear();
    points.clear();
    gridRect.set(0, 0, 0, 0);
    bFirstPoint = true;
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
                    
                    point.pos = convertLatLon(point.pos);
                    ///  cout << point.pos << endl;
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
                    points[point.ID] = point;
                    addPoint(point, false);
                    bFirstPoint = false;
                }
            }
            while( xml.setToSibling() );
        }
    }else{
        
    }
    /*
     string str = "";
     for (map<string, SVPoint>::iterator it = points.begin(); it != points.end(); it++) {
     str += "SVPoint: "  + it->second.ID + "  " + ofToString(it->second.pos);
     for (int i = 0; i < it->second.links.size(); i++) {
     str += "   " + it->second.links[i];
     }
     str += "\n";
     }
     cout << str << endl;
     //*/
    // ofBuffer b(str);
    //ofBufferToFile("test"+ ofGetTimestampString() +".txt", b);
    //*
    
    /*
     float as = ofGetWidth()/float(ofGetHeight());
     float am = gridRect.width/gridRect.height;
     
     bool bUseWidth = false;
     if (as < am) {
     bUseWidth = true;
     }
     
     //*/
    
    /*
    ofVec3f centroid = mesh.getCentroid();
    
    float angle = centroid.getNormalized().angle(ofVec3f(0,0,1));
    //*
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    
//    centroid = mesh.getCentroid();
    
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i].rotate(angle, centroid.getNormalized().getCrossed(ofVec3f(0,0,1)));
      //  mesh.getVertices()[i] -= centroid;
        if (i==0) {
            gridRect.set(mesh.getVertices()[i], 0,0);
        }else{
            gridRect.growToInclude(mesh.getVertices()[i]);
        }
    }
    //*/
    //*/
    //rotatePoints();
/*
    for (int i = 0; i < mesh.getNumVertices(); i++) {
        mesh.getVertices()[i] *= 0.001;
    }
    
    //*
     ofVec3f centroid = mesh.getCentroid();
    for (int i = 0; i < mesh.getNumVertices(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    
//     cout << "cam.getImagePlaneDistance(gridRect): " << cam.getImagePlaneDistance(gridRect) << endl;
  //   ofVec3f p = centroid.getNormalized() * (centroid.length() + cam.getImagePlaneDistance(gridRect));
     
     
    // cout << "centroid: " << centroid << endl;
    // cout << "pos: " << p << endl;
     //*/
     //*
    
     cam.setPosition(mesh.getCentroid() +20);
     cam.lookAt(mesh.getCentroid());//centroid);
    cam.setCursorWorld(mesh.getCentroid());
     cam.setNearClip(0);
     //*/
    
    //    camPos.set(centroid);
    
    cout << "gridRect: " << gridRect << "  centroid: " << mesh.getCentroid() << endl;
    
    cout << "cam  pos: " << cam.getPosition() << "   lookat: " << cam.getLookAtDir() << endl;
    //*/
}
//--------------------------------------------------------------
void ofApp::savePoint(SVPoint & p){
    
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
    //    x.setToParent();
    //  x.setToParent();
    x.setTo("//SVPoint");
    xml.addXml(x);
    //*/
}
//--------------------------------------------------------------
void ofApp::addPoint(SVPoint & p, bool bSave){
    ofVec3f v = p.pos;
    if (bFirstPoint) {
        bFirstPoint = false;
        gridRect.set(p.pos, 0, 0);
    }else{
        gridRect.growToInclude(v);
    }
    if(v.z != -1.0){
        
        if(bMapPoints){
            v.x = ofMap(v.x, area.getMinX(), area.getMaxX(), 0, ofGetWidth());
            v.y = ofMap(v.y, area.getMinY(), area.getMaxY(), 0, ofGetHeight());
        }
        for (int i = 0; i < p.links.size(); i++) {
            if (points.count(p.links[i])>0) {
                if(points[p.links[i]].z != -1.0){
                    mesh.addVertex(v);
                    mesh.addColor(ofFloatColor::white);
                    mesh.addVertex(points[p.links[i]].pos);
                    mesh.addColor(ofFloatColor::white);
                }
            }
        }
        if (bSave) {
            //    cout << p.pos << "  " << p.ID << endl;
            savePoint(p);
        }
    }
}


//--------------------------------------------------------------
ofVec3f ofApp::convertLatLon(ofVec3f v){
    /*
     double y = sin(v.x * DEG_TO_RAD)*(EARTH_RADIUS + v.z);
     double x = cos(v.y * DEG_TO_RAD)*(EARTH_RADIUS + v.z);
     y /= 1000.0;
     x /= 1000.0;
     return ofVec3f(x,y,v.z/1000);
     //*/
    //*
    ofQuaternion latRot, longRot;
    latRot.makeRotate(v.x, 1, 0, 0);
    longRot.makeRotate(v.y, 0,1,0);
    ofVec3f center = ofVec3f(0,0,EARTH_RADIUS+(v.z));///1000));
    
    ofVec3f r(latRot * longRot * center);///1000.0;
//
 //   r = (r -centroid );
//    r.y -= EARTH_RADIUS;
    return r;
    //*/
}

//--------------------------------------------------------------
void ofApp::remapPoints(){
    mesh.clear();
    for (map<string, SVPoint>::iterator it = points.begin(); it != points.end(); it++) {
        ofVec3f v = it->second.pos;
        if (bMapPoints) {
            v.x = ofMap(v.x, area.getMinX(), area.getMaxX(), 0, ofGetWidth());
            v.y = ofMap(v.y, area.getMinY(), area.getMaxY(), 0, ofGetHeight());
        }
        mesh.addVertex(v);
        mesh.addColor(ofFloatColor::white);
    }
}
//--------------------------------------------------------------
void ofApp::exit(){
    if (bNewDataCollected) {
        xml.setTo("//Points");
        xml.save(ofGetTimestampString()+".xml");
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ' ') {
        bMapPoints ^= true;
        remapPoints();
    }else if(key == 's'){
        startCollectingData();
    }else if(key == 'g'){
        bDrawGrid ^= true;
    }else if(key == 'c'){
        bUseCam ^= true;
    }else if(key == 'l'){
        ofFileDialogResult res = ofSystemLoadDialog();
        if (res.bSuccess) {
            loadPoints(res.getPath());
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){}
//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}