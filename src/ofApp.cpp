#include "ofApp.h"

#define EARTH_RADIUS 6366197

//#include "ofVectorMathDouble.h"
#include "ofxModifierKeys.h"
//--------------------------------------------------------------
void ofApp::setup(){
    info = "";
    //lim1.set(40.911773, -74.005716);//(40.74171, -74.0086659);
    //lim2.set(40.649392,-73.945211);//(40.7027061, -73.9875523);
    
    lim1.set(40.845233, -74.024424);
    lim2.set(40.694552, -73.998585);
    
    collector.setup(lim1, lim2);
    
    origin = (lim1 + lim2)*0.5;
    centroid.set(0);
    centroid = convertLatLon(origin);
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    lineMesh.setMode(OF_PRIMITIVE_LINES);
    normalsMesh.setMode(OF_PRIMITIVE_LINES);
    bDrawGrid  = true;
    bUseCam = true;
    
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    
    doShader = true;
    
    shader.load("shader");
    
    bEnableFog = true;
    //    bUseTravelingCam = true;
    
    gui.setup();
    gui.add(thickness.set("thickness", 1,0, 40));
    gui.add(heightMult.set("heightMult", 1, 1, 10));
    
    gui.add(fogStart.set("Fog Start", 0, 0, 5000));
    gui.add(fogEnd.set("Fog End", 800, 0, 10000));
    gui.add(fogColor.set("Fog Color", ofFloatColor(1,1), ofFloatColor(0,0),ofFloatColor(1,1)));
    //*
    
    heightMult.addListener(this, &ofApp::heightMultChanged);
    
    tCam.setup(ofVec3f(0));
    tCam.initCamPos();
    tCam.setNearClip(0);
    gui.add(tCam.parameters);
    
    cam.setFarClip(5000);
    //*/
    
    
    
    
    // ssao.setup();
    
    ofEnableDepthTest();
    
    ofSetVerticalSync(true);
//    ofEnableLighting();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    light.enable();
    light.setPointLight();
    light.setPosition(0,20,0);
    
    
}
//--------------------------------------------------------------
void ofApp::heightMultChanged(float& f){

    tCam.heightMult = heightMult;
    
}
//--------------------------------------------------------------
void ofApp::update(){}
//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(points.size() ), 10, 30);
    //*
    if (bUseTravelingCam) {
        tCam.begin();
    }else{
        
        cam.begin();
    }
    //  glEnable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE, GL_ZERO);
    
    //*
    if(doShader) {
        shader.begin();
        shader.setUniform1f("thickness", thickness);
        shader.setUniform1f("heightMult", heightMult);
        shader.setUniform1f("fogStart",fogStart );
        shader.setUniform1f("fogEnd", fogEnd);
        shader.setUniform4f("fogColor", fogColor->r, fogColor->g, fogColor->b, fogColor->a);
    }
    //*/
    ofSetLineWidth(thickness);
    ofSetColor(255);
    if(doShader) {
        mesh.draw();
    }else{
        lineMesh.draw();
    }
    if(doShader) shader.end();
    
    normalsMesh.draw();
    
    if(bDrawGrid){
        ofDrawGrid(1, 20, true);
    }
    //*
    if (bUseTravelingCam) {
        tCam.end();
    }else{
        ofSetColor(ofColor::magenta);
        for (int i = 0; i < closest.size(); i++) {
            ofDrawSphere(closest[i], 1);
        }

        cam.end();
    }
    gui.draw();
    

    ofDrawBitmapStringHighlight(info, 200, 30);
 }

//--------------------------------------------------------------
void ofApp::loadPoints(string path){
    ofXml xml;
    points.clear();
    meshPoints.clear();
    mesh.clear();
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
                  //  movePointToOrigin(point.pos);
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
                    addPoint(point);
                }
            }
            while( xml.setToSibling() );
        }
    }
    moveMeshPointsToOrigin();
    buildMesh();
}
//--------------------------------------------------------------
void ofApp::buildMesh(){
    //    mesh.addVertices(meshPoints);
    //*
    mesh.clear();
    vector<ofVec3f>& v = meshPoints;
    for (int i = 0; i < v.size()-1; i+=2) {
     
        lineMesh.addVertex(v[i]);
        lineMesh.addVertex(v[i+1]);
        lineMesh.addColor(ofFloatColor::black);
        lineMesh.addColor(ofFloatColor::black);

        int  ind = mesh.getNumVertices();
        ofVec3f c = v[i+1] - v[i];
        c.normalize();
        c.cross(ofVec3f(0,1,0));

        lineMesh.addVertex(v[i]);
        lineMesh.addVertex(v[i]+c);
        lineMesh.addColor(ofFloatColor::blue);
        lineMesh.addColor(ofFloatColor::blue);
        
        /*
        ofVec3f mv [6];
        
        mv[0] = v[i];
        mv[1] = v[i];
        mv[2] = v[i+1]+c;
        
        mv[3] = v[i+1]+c;
        mv[4] = v[i]-c;
        mv[5] = v[i+1]-c;
        
        
        mesh.addVertex(mv[0]);
        mesh.addVertex(mv[1]);
        mesh.addVertex(mv[2]);
        mesh.addVertex(mv[3]);
        mesh.addVertex(mv[4]);
        mesh.addVertex(mv[5]);
        //*/
        
        mesh.addVertex(v[i]);
        mesh.addVertex(v[i]);
        mesh.addVertex(v[i+1]);
        mesh.addVertex(v[i+1]);
        /*
        ofVec3f n [2];
        
        n[0] = (mv[2] - mv[0]).normalize().getCrossed((mv[1] - mv[0]).normalize()).normalize();
        n[0] = (mv[5] - mv[3]).normalize().getCrossed((mv[4] - mv[3]).normalize()).normalize();
        mesh.addNormal(n[0]);
        mesh.addNormal(n[0]);
        mesh.addNormal(n[0]);
        mesh.addNormal(n[1]);
        mesh.addNormal(n[1]);
        mesh.addNormal(n[1]);
        //*/
        
        mesh.addIndex(ind);
        mesh.addIndex(ind + 1);
        mesh.addIndex(ind + 2);
        mesh.addIndex(ind + 1);
        mesh.addIndex(ind + 2);
        mesh.addIndex(ind + 3);
        
        //*/
        ofFloatColor fc(c.x, c.y, c.z, 1.0);
     //   ofFloatColor fc(1.0);
        mesh.addColor(fc);
        mesh.addColor(fc);
        mesh.addColor(fc);
        mesh.addColor(fc);
    }
    
    
    buildNormals();
    normalsMesh.clear();
    normalsMesh.setMode(OF_PRIMITIVE_LINES);
    
    for (int i = 0; i < mesh.getNumNormals(); i++) {
        normalsMesh.addVertex(mesh.getVertices()[i]);
        normalsMesh.addVertex(mesh.getVertices()[i]+mesh.getNormals()[i]);
        normalsMesh.addColor(ofFloatColor::magenta);
        normalsMesh.addColor(ofFloatColor::magenta);
    }
    
    
    //*/
}

//--------------------------------------------------------------
void ofApp::buildNormals(){
    vector<ofVec3f> ns;
    ns.resize(mesh.getNumVertices());
    mesh.addNormals(ns);
    
    
 //   cout << __PRETTY_FUNCTION__ << "  mesh num Normals: " << mesh.getNumNormals() << endl;
    
    for (unsigned int i =0; i<mesh.getNumIndices()-2; i+=3) {
        ofVec3f v[3];
        for (int j = 0; j <3; j++) {
            v[j] = mesh.getVertices()[mesh.getIndices()[i+j]];
        }
        
        ofVec3f n = (v[1] -v[0]).getCrossed(v[2]-v[0]);
        for (int j = 0; j <3; j++) {
            int ind = mesh.getIndices()[i+j];
            mesh.setNormal(ind, mesh.getNormals()[ind] + n);
        }
    }
    
    for (unsigned int i =0; i<mesh.getNumNormals(); i++) {
        mesh.getNormals()[i].normalize();
    }
    
}

//--------------------------------------------------------------
void ofApp::movePointToOrigin(ofVec3f& p){
    p -= centroid;
    ofVec3d pos (p.x, p.y, p.z);
    ofQuaternionD rot;
    ofVec3d c(centroid.x, centroid.y, centroid.z);
    rot.makeRotate(c.getNormalized(), ofVec3d(0,1,0));
    pos = rot*pos;
   // p *=0.1;
    pos.x *= -1;
    p.set(pos.x, pos.y, pos.z);
  /*
    cout << doubleToString(pos.x) << ", " << doubleToString(pos.y) << ", " << doubleToString(pos.z) << endl;
    cout << floatToString(p.x) << ", " << floatToString(p.y) << ", " << floatToString(p.z) << endl;

    cout << "//--------------------------------------------------------------" << endl;
    //*/
}
//--------------------------------------------------------------
void ofApp::moveMeshPointsToOrigin(){
    
    ofMesh m;
    for (int i = 0; i < meshPoints.size(); i++) {
        m.addVertex(meshPoints[i]);
    }
    centroid = m.getCentroid();
    
    cout << "centroid: " << centroid << endl;
    for (int i = 0; i < meshPoints.size(); i++) {
        movePointToOrigin(meshPoints[i]);
    }
    m.clear();
    for (int i = 0; i < meshPoints.size(); i++) {
        m.addVertex(meshPoints[i]);
    }
    centroid = m.getCentroid();
    
    for (int i = 0; i < meshPoints.size(); i++) {
     meshPoints[i] -= centroid;
    }
    
}
//--------------------------------------------------------------
void ofApp::addPoint(SVPoint & p){
    ofVec3f v = p.pos;
    if(p.z != -1.0){
        //meshPoints.push_back(v);
        //meshPoints.push_back(v);
        // p.meshIndex=meshPoints.size()-1;
        points[p.ID] = p;
        for (int i = 0; i < p.links.size(); i++) {
            if (points.count(p.links[i])>0) {
                if(points[p.links[i]].z != -1.0){
                    /*
                    mesh.addIndex(p.meshIndex);
                    mesh.addIndex(p.meshIndex-1);
                    mesh.addIndex(points[p.links[i]].meshIndex);
                    mesh.addIndex(p.meshIndex-1);
                    mesh.addIndex(points[p.links[i]].meshIndex);
                    mesh.addIndex(points[p.links[i]].meshIndex-1);
                    //*/
                    meshPoints.push_back(v);
                    meshPoints.push_back(points[p.links[i]].pos);
                    
                }
            }
        }
    }
}
//--------------------------------------------------------------
ofVec3f ofApp::convertLatLon(ofVec3f v){
    ofQuaternionD latRot, longRot;
    latRot.makeRotate(v.x, 1, 0, 0);
    longRot.makeRotate(v.y, 0,1,0);
    ofVec3d center = ofVec3d(0,0,EARTH_RADIUS+(v.z));
    ofVec3d r(latRot * longRot * center);
    
  ///  cout << doubleToString(r.x) << ", " << doubleToString(r.y) << ", " << doubleToString(r.z) << endl;
    return ofVec3f(r.x,r.y,r.z);
}
//--------------------------------------------------------------
void ofApp::exit(){
    collector.save();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    //*
    if (key == ' '){
        if (bUseTravelingCam) {
            
        
        if (tCam.isStarted()) {
            tCam.stop();
        }else{
            tCam.start();
        }
        }else{
        info = "";
            closest.clear();
        }
    }else
        //*/
        if(key == 's'){
            collector.startCollectingData();
        }else if(key == 'S'){
            ofFileDialogResult res = ofSystemLoadDialog();
            if (res.bSuccess) {
                collector.loadAndCollect(res.getPath());
            }
        }else if(key == 'w'){
            doShader ^= true;
        }else if(key == 'g'){
            bDrawGrid ^= true;
        }else if(key == 't'){
            bUseTravelingCam ^= true;
        }else if(key == 'c'){
            bUseCam ^= true;
        }else if(key == 'l'){
            ofFileDialogResult res = ofSystemLoadDialog();
            if (res.bSuccess) {
                loadPoints(res.getPath());
            }
        } else if (key == 'L'){
            if (ofGetLightingEnabled()) {
                ofDisableLighting();
            }else{
                ofEnableLighting();
            }
        }else if (key == 'e'){
            pathEdit.setMesh(lineMesh);
            pathEdit.setPath(tCam.path);
            pathEdit.runModal();
        }else if(key == 'f'){
            bEnableFog ^= true;
        }
    //*
        else if(key == 'p'){
            tCam.togglePause();
        }
    //*/
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){} 
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (ofGetModifierPressed(OF_KEY_SHIFT)) {
        cout << "getclosesrt" << endl;
    
    float minDist = FLT_MAX;
    ofVec3f m(x,y);
    size_t ind =0;
    for (size_t i =0; i < meshPoints.size(); i++) {
        float mn = cam.worldToScreen(meshPoints[i]).distance(m);
        if (mn < minDist) {
            minDist = mn;
            ind = i;
        }
    }
        info += ofToString(meshPoints[ind])+"\n";
        closest.push_back(meshPoints[ind]);
    }
    
}
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