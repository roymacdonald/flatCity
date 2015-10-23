//
//  pathEditor.h
//  floatingPointImageExample
//
//  Created by Roy Macdonald on 10-06-15.
//
//

#pragma once
#include "ofMain.h"
#include "ofxModal.h"
#include "draggable.h"
#include "ofxModifierKeys.h"
#include "pathSaveLoad.h"
#include "ofx2DCam.h"
//--------------------------------------------------------------
//--------------------------------------------------------------
class pathEditor: public ofxModal{
public:
    //--------------------------------------------------------------
    pathEditor():bUsingPolyline(true), meshPtr(NULL){
        setOverlayColor(ofColor::darkGray);
        width = ofGetWidth();
        height = ofGetHeight();
        polyPtr = NULL;
        //cam.disableMouseInput();
        cam.setLookAt(ofx2DCam::OFX2DCAM_TOP);
        bNewVertex = false;
        
    }
    //--------------------------------------------------------------
    ~pathEditor(){
        polyPtr = NULL;
        meshPtr = NULL;
    }
    //--------------------------------------------------------------
    void setPath(ofPolyline& p){
        polyPtr = &p;
        poly = p;
        bUsingPolyline = true;
    }
    //--------------------------------------------------------------
    void setMesh(ofMesh& m){
        meshPtr = &m;
    }
    //--------------------------------------------------------------
    void begin(){
        cam.begin(*this);
        ofDisableLighting();
    }
    //--------------------------------------------------------------
    void end(){

        ofPushStyle();
        ofSetColor(ofColor::red);
        ofSetLineWidth(3);
        
        if(bUsingPolyline){
            buildPolyline(verticesToVec3f());
            poly.draw();
        }
        for (int i =0; i < vertices.size(); i++) {
            vertices[i].draw(cam.getScale());
        }
        ofPopStyle();
        cam.end();
    }
    //--------------------------------------------------------------
    void customDraw(){
        begin();
        if(meshPtr){
            meshPtr->draw();
        }
        end();
    }
    //--------------------------------------------------------------
    vector<ofVec3f> verticesToVec3f(ofVec3f off = ofVec3f(0)){
        vector<ofVec3f>v;
        v.resize(vertices.size());
        for (int i = 0; i < v.size(); i++) {
            v[i].set(vertices[i] + off);
        }
        return v;
    }
    //--------------------------------------------------------------
    void buildPolyline(const vector<ofVec3f>& v){
        poly.clear();
        for (int i =0; i < v.size(); i++) {
            if (i == 0 || i == v.size() -1) {
                poly.curveTo(ofVec3f(v[i]));
            }
            poly.curveTo(ofVec3f(v[i]));
        }
    }
    //*
    //--------------------------------------------------------------
    virtual bool mouseDragged(ofMouseEventArgs& args){
        if (inside(args.x, args.y)) {
            bool bShift = ofGetModifierPressed(OF_KEY_SHIFT);
            if (!bShift) {
                for (int i =0; i < vertices.size(); i++) {
                    if (vertices[i].isDragging()) {
                        vertices[i].drag(cam.screenToWorld(ofVec3f(args.x, args.y)));
                    }
                }
            }else{
                cam.mouseDragged(args);
            }
        }
        return true;
    }
    //--------------------------------------------------------------
    virtual bool mousePressed(ofMouseEventArgs& args){
        if (inside(args.x, args.y)) {
            bool bShift = ofGetModifierPressed(OF_KEY_SHIFT);
            if (bShift) {
                cam.mousePressed(args);
            }else if(args.button != 0){
                for (int i =0; i < vertices.size(); i++) {
                    vertices[i].setSelected(vertices[i].isOver(cam.screenToWorld(ofVec3f(args.x, args.y)),cam.getScale()));
                }
            }else{
                bool bOver = false;
                for (int i =0; i < vertices.size(); i++) {
                    if (vertices[i].isOver(cam.screenToWorld(ofVec3f(args.x, args.y)))){
                        bOver = true;
                        vertices[i].setDragging();
                    }
                }
                if (!bOver) {
                    vertices.push_back(draggable());
                    vertices.back().set(cam.screenToWorld(ofVec3f(args.x, args.y)));
                    bNewVertex = true;
                }
            }
        }
        return true;
    }
    //--------------------------------------------------------------
    virtual bool mouseReleased(ofMouseEventArgs& args){
        if (inside(args.x, args.y)) {
            bool bShift = ofGetModifierPressed(OF_KEY_SHIFT);
            if (bShift) {
                cam.mouseReleased(args);
            }else{
                for (int i =0; i < vertices.size(); i++) {
                    vertices[i].setDragging(false);
                }
                if(bNewVertex){
                    if(meshPtr){
                        unsigned int minInd =0;
                        float minDist = FLT_MAX;
                        ofVec3f v = vertices.back();
                        bool bFound = false;
                        for(int i =0; i < meshPtr->getNumVertices(); i++) {
                            ofVec3f mv = meshPtr->getVertices()[i];
                            mv.y = 0;
                            float d = mv.distance(v);
                            if (d < minDist) {
                                minDist = d;
                                minInd = i;
                                bFound = true;
                            }
                        }
                        if (bFound) {
                            vertices.back().y = meshPtr->getVertices()[minInd].y;
                            cout << "vert mod: y = " << vertices.back().y << endl;
                        }
                    }
                }
            }
        }
        bNewVertex = false;
        return true;
    }
    //--------------------------------------------------------------
    virtual bool mouseScrolled(ofMouseEventArgs& args){
        bool bShift = ofGetModifierPressed(OF_KEY_SHIFT);
        if (bShift) {
            cam.mouseScrolled(args);
        }
        return true;
    }
    //--------------------------------------------------------------
    virtual bool keyReleased(ofKeyEventArgs& args){
        if (args.key == OF_KEY_ESC) {
            if (bUsingPolyline && polyPtr && poly.size() > 1) {
                buildPolyline(verticesToVec3f());
                *polyPtr = poly;
            }
            saveVertices("defaultCamPath");
            stopModal();
        }else if(args.key == OF_KEY_BACKSPACE){
            for (int i =0; i < vertices.size(); i++) {
             //   if(vertices[i].isSelected()){
                    vertices.erase(vertices.begin()+i);
                    i--;
               // }
            }
        }else if(args.key == 's'){
            if(vertices.size() ){
                ofFileDialogResult res = ofSystemSaveDialog("camPath", "Guarda el trazado de la camara...");
                if(res.bSuccess){
                    saveVertices(res.getPath());
                }
            }
        }else if(args.key == 'l'){
            ofFileDialogResult res = ofSystemLoadDialog();
            if(res.bSuccess){
                loadVertices(res.getPath());
            }
        /*
         }else if(args.key == ' '){
            if(cam.getLookAt() == ofx2DCam:: OFX2DCAM_BOTTOM){
                cam.setLookAt(ofx2DCam::OFX2DCAM_FRONT);
            }else{
                cam.setLookAt(ofx2DCam::LookAt((int)cam.getLookAt()+1));
            }
            //*/
        }
        return true;
    }
    //--------------------------------------------------------------
    virtual bool windowResized(ofResizeEventArgs& args){
     //   cout << __PRETTY_FUNCTION__ << endl;
        this->setWidth(args.width);
        this->setHeight(args.height);
        return false;}
    //--------------------------------------------------------------
    void loadVertices(string path){
        vector<ofVec3f>v;
        pathSaveLoad::load(path, v);
        if (v.size()) {
            vertices.clear();
            for (int i = 0; i < v.size(); i++) {
                vertices.push_back(draggable());
                vertices.back().set(v[i]);
            }
        }
    }
    //--------------------------------------------------------------
    void saveVertices(string path){
        if(vertices.size() ){
            pathSaveLoad::save(path, verticesToVec3f());
        }
    }
    //--------------------------------------------------------------
    vector <draggable> vertices;
    bool bUsingPolyline;
    ofPolyline poly;
    ofPolyline * polyPtr;
    ofMesh * meshPtr;
    
    
    ofx2DCam cam;
    bool bNewVertex;
    
};