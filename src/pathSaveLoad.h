//
//  pathSaveLoad.h
//  floatingPointImageExample
//
//  Created by Roy Macdonald on 13-06-15.
//
//

#pragma once
#include "ofMain.h"

class pathSaveLoad{
public:
    static void save(string path, const vector<ofVec3f>& v){
        if (v.size()) {
        string s = "";
        for (int i =0; i < v.size(); i++) {
            s += ofToString(v[i]);
            if(i < v.size() -1){
                s += "\n";
            }
        }
        
        ofBuffer b(s);
        ofBufferToFile(path, b);
        }else{
        ofLogNotice() << "No se puede guardar vector porque no tiene elementos!.";
        }
    }
    static void load(string path, vector<ofVec3f>& v){
        ofFile file(path);
        
        if(!file.exists()){
            ofLogError("The file " + path + " is missing");
            return;
        }
        
        ofBuffer buffer(file);
        
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            ofVec3f t;
            istringstream cur(line);
            cur >> t;
            v.push_back(t);
        }
    }
    static void loadPolylineVertices(string path, ofPolyline& poly, ofVec3f offset, int resCurve = 20){
    
        vector<ofVec3f>vertices;
        load(path, vertices);
        if (vertices.size()) {
            poly.clear();
            for (int i =0; i < vertices.size(); i++) {
                if (i == 0 || i == vertices.size() -1) {
                    poly.curveTo(ofVec3f(vertices[i])+offset, resCurve);
                }
                poly.curveTo(ofVec3f(vertices[i])+offset, resCurve);
            }
        }
    }

};

