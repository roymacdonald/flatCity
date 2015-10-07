#include "ofApp.h"

#define EARTH_RADIUS 6366197

//--------------------------------------------------------------
void ofApp::setup(){
    
    lim1.set(40.911773, -74.005716);//(40.74171, -74.0086659);
    lim2.set(40.649392,-73.945211);//(40.7027061, -73.9875523);
    collector.setup(lim1, lim2);
    
    origin = (lim1 + lim2)*0.5;
    centroid.set(0);
    centroid = convertLatLon(origin);
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    normalMesh.setMode(OF_PRIMITIVE_LINES);
    
    bDrawGrid  = true;
    bUseCam = true;
    
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    
    doShader = true;
    
    shader.load("shader");

    gui.setup();
    gui.add(thickness.set("thickness", 1,0, 30));
    gui.add(heightMult.set("heightMult", 0, 1, 100));
    
    
}
//--------------------------------------------------------------
void ofApp::update(){}
//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(mesh.getNumVertices()), 10, 30);
    
    if (bUseCam) {
        cam.begin();
    }
    if(doShader) {
        shader.begin();
        shader.setUniform1f("thickness", thickness);
        shader.setUniform1f("heightMult", heightMult);
        
    }
    ofSetColor(255);
    mesh.draw();
    if(doShader) shader.end();
    normalMesh.draw();
    
    if(bDrawGrid){
        ofDrawGrid(1, 20, true);
    }
    
    if(bUseCam){
        cam.end();
    }
    
    gui.draw();
    
    
    
    //*/
}

//--------------------------------------------------------------
void ofApp::loadPoints(string path){
    ofXml xml;
    points.clear();
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
                    addPoint(point);
                }
            }
            while( xml.setToSibling() );
        }
    }
    /*
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    ofQuaternion rot;
    rot.makeRotate(centroid.getNormalized(), ofVec3f(0,1,0));
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] = rot*mesh.getVertices()[i];
    }
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] *=0.1;
    }
    //*/
//    moveMeshToOrigin(mesh);
    moveMeshToOrigin(normalMesh);
    vector<ofVec3f>& v = normalMesh.getVertices();
    for (int i = 0; i < v.size()-1; i+=2) {
    
     int  ind = mesh.getNumVertices();
     ofVec3f c = v[i+1] - v[i];
     c.normalize();
     c.getCrossed(ofVec3f(0,1,0));
        
     ofFloatColor fc(c.x, c.y, c.z, 1.0);
     mesh.addColor(fc);
     mesh.addColor(fc);
     mesh.addColor(fc);
     mesh.addColor(fc);
    
     mesh.addVertex(v[i]);
     mesh.addVertex(v[i+1]);

     mesh.addIndex(ind);
     mesh.addIndex(ind + 1);
     mesh.addIndex(ind + 2);
     mesh.addIndex(ind + 1);
     mesh.addIndex(ind + 2);
     mesh.addIndex(ind + 3);
        
     }
    
}
//--------------------------------------------------------------
void ofApp::moveMeshToOrigin(ofMesh& mesh){
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] -= centroid;
    }
    ofQuaternion rot;
    rot.makeRotate(centroid.getNormalized(), ofVec3f(0,1,0));
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] = rot*mesh.getVertices()[i];
    }
    for (int i = 0; i < mesh.getVertices().size(); i++) {
        mesh.getVertices()[i] *=0.1;
    }
}
//--------------------------------------------------------------
void ofApp::addPoint(SVPoint & p){
    ofVec3f v = p.pos;
    if(p.z != -1.0){
        for (int i = 0; i < p.links.size(); i++) {
            if (points.count(p.links[i])>0) {
                if(points[p.links[i]].z != -1.0){
                    /*
                    int  ind = mesh.getNumVertices();
                    ofVec3f cc = points[p.links[i]].pos - v;
                    cc.normalize();
                    ofVec3f c = cc.getCrossed(ofVec3f(0,1,0));
                    
                    c.y = 0;
                    c.x *= 10.0;
                    c.z *= 10.0;
                    //*/
                    /*
                    ofFloatColor fc(c.x, c.y, c.z, 1.0);
                    mesh.addColor(fc);
                    mesh.addColor(fc);
                    mesh.addColor(fc);
                    mesh.addColor(fc);
                    //*/
                    normalMesh.addVertex(v);// + c);
                    //mesh.addVertex(v);// - c);
                    normalMesh.addVertex(points[p.links[i]].pos);// + c);
                    normalMesh.addColor(ofFloatColor::yellow);
                    normalMesh.addColor(ofFloatColor::yellow);
                    //  mesh.addVertex(points[p.links[i]].pos);// - c);
               //     mesh.getVertices()[mesh.getNumVertices()-1].y = 0;
                 //   mesh.getVertices()[mesh.getNumVertices()-2].y = 0;
                   // mesh.getVertices()[mesh.getNumVertices()-3].y = 0;
                   // mesh.getVertices()[mesh.getNumVertices()-4].y = 0;
                    /*
                    mesh.addIndex(ind);
                    mesh.addIndex(ind + 1);
                    mesh.addIndex(ind + 2);
                    mesh.addIndex(ind + 1);
                    mesh.addIndex(ind + 2);
                    mesh.addIndex(ind + 3);
                    //
                    normalMesh.addVertex(v);
                    normalMesh.addVertex(points[p.links[i]].pos);
                    normalMesh.addColor(ofFloatColor::yellow);
                    normalMesh.addColor(ofFloatColor::yellow);
                    normalMesh.addVertex(v);
                    normalMesh.addVertex(c*10+v);
                    normalMesh.addColor(ofFloatColor::magenta);
                    normalMesh.addColor(ofFloatColor::magenta);
                    normalMesh.addVertex(v);
                    normalMesh.addVertex(ofVec3f(0,10,0)+v);
                    normalMesh.addColor(ofFloatColor::white);
                    normalMesh.addColor(ofFloatColor::white);
                    //*/
                }
            }
        }
    }
}
//--------------------------------------------------------------
ofVec3f ofApp::convertLatLon(ofVec3f v){
    ofQuaternion latRot, longRot;
    latRot.makeRotate(v.x, 1, 0, 0);
    longRot.makeRotate(v.y, 0,1,0);
    ofVec3f center = ofVec3f(0,0,EARTH_RADIUS+(v.z));///1000));
    
    ofVec3f r(latRot * longRot * center);//
//
//    r = (r -centroid );
//    r.y -= EARTH_RADIUS;
    return r;
    //*/
}

//--------------------------------------------------------------
void ofApp::exit(){
    collector.save();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 's'){
        collector.startCollectingData();
    }else if(key == 'w'){
        doShader ^= true;
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