//
//  ofxDoubleToStringHelper.h
//  emptyExample
//
//  Created by Roy Macdonald on 06-10-15.
//
//

#pragma once

static double stringToDouble(string s) {
    
    double d;
    stringstream cur;
    cur.precision(20);
    cur << s;
    cur >> d;
    return d;
    
}
static float stringToFloat(string s) {
    
    float f;
    stringstream cur;
    cur.precision(20);
    cur << s;
    cur >> f;
    return f;
    
}

static string doubleToString(double d){
    string s;
    stringstream cur;
    cur.precision(20);
    cur << d;
    cur >> s;
    return s;
}
static string floatToString(float d){
    string s;
    stringstream cur;
    cur.precision(20);
    cur << d;
    cur >> s;
    return s;
}
