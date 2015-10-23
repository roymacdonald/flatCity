//
//  SVPoint.h
//  emptyExample
//
//  Created by Roy Macdonald on 06-10-15.
//
//

#pragma once
#include "ofMain.h"

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
    ofIndexType meshIndex;
    vector<string>links;
    //    bool bCheckedAll, bIsSet, bIsInside;
    string ID;
};
