//
//  draggable.h
//  floatingPointImageExample
//
//  Created by Roy Macdonald on 13-06-15.
//
//

#pragma once
#include "ofMain.h"

#define DRAGGABLE_TOLERANCE 5

class draggable: public ofVec3f{
public:
    //--------------------------------------------------------------
    draggable():bDragging(false), bSelected(false){}
    bool isOver(float px, float py){
        return isOver(ofVec3f(px, py));
    }
    //--------------------------------------------------------------
    bool isOver(ofVec3f v, float scaling = 1){
        return (v.distance(*this)<= DRAGGABLE_TOLERANCE / scaling);
    }
    //--------------------------------------------------------------
    void setSelected(bool b=true){
        bSelected = b;
    }
    //--------------------------------------------------------------
    void setDragging(bool e = true, ofVec3f p = ofVec3f()){
        bDragging = e;
        //if (bDragging) {
        //    dragOffset = p - *this;
        //}
    }
    //--------------------------------------------------------------
    void drag(ofVec3f v){
        if (bDragging) {
            this->set( v);// - dragOffset);
        }
    }
    //--------------------------------------------------------------
    void draw(float scaling =1){
        ofPushStyle();
        ofSetLineWidth(1);
        ofFill();
        if (bSelected) {
            ofSetColor(ofColor::magenta);
            ofDrawSphere(*this, 10/scaling);
        }else if (bDragging) {
            ofSetColor(ofColor::cyan);
            ofDrawSphere(*this, 10/scaling);
        }
        ofSetColor(ofColor::black);
        ofDrawSphere(*this, 6/scaling);
        ofSetColor(ofColor::white);
        ofDrawSphere(*this, 4/scaling);
    //    ofDrawBitmapStringHighlight(ofToString(*this), *this);
        ofPopStyle();
    }
    //--------------------------------------------------------------
    bool isDragging(){return bDragging;}
    bool isSelected(){return bSelected;}
protected:
    bool bDragging, bSelected;
//    ofVec3f dragOffset;
};

