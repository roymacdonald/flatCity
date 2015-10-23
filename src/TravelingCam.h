//
//  Header.h
//  floatingPointImageExample
//
//  Created by Roy Macdonald on 08-06-15.
//
//

#pragma once
#include "ofMain.h"
#include "ofxTween.h"
class TravelingCam: public ofCamera{
public:
    TravelingCam():bStarted(false), bMouseEnabled(false), bMousePressed(false), bXenoRunning(false), bPaused(false){
        currentDistance = currentSpeed = pathLength = 0;
        lastUpdateTime = 0;
    }
    ~TravelingCam(){
        stop();
    }
    void setup(ofVec3f meshOffset);
    void update(ofEventArgs& a);
    
    ofPolyline path;
    
    ofParameterGroup parameters;
    ofParameter<float> speed, altura, xenoFactor;
    ofParameter<int>rampTime, resolucionCurvas;
    
    void tweenEnded(int &i);
    void curveResChanged(int& r);
    
    void start();
    void stop();
    
    bool isStarted(){return bStarted;}
    
    float angle;
    
    float heightMult;
    
    ofVec3f dirVec;
    void customDraw();
    void initCamPos();
    void enableMouseInput(bool e = true);
    bool isMouseInputEnabled(){return bMouseEnabled;}

    void dragged(int x, int y, int button);
    void pressed(int x, int y, int button);
    void released(int x, int y, int button);
    
    void mouseDragged(ofMouseEventArgs& args);
    void mouseMoved(ofMouseEventArgs& args){}
    void mousePressed(ofMouseEventArgs& args);
    void mouseReleased(ofMouseEventArgs& args);
    void mouseScrolled(ofMouseEventArgs& args){}
    void mouseEntered(ofMouseEventArgs& args){}
    void mouseExited(ofMouseEventArgs& args){}
    
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch){};
    void touchCancelled(ofTouchEventArgs & touch){}

    
    void pause(bool p = true);
    void togglePause();
protected:
    bool bStarted, bTweening, bMouseEnabled;
    float slowdownDist;
    float currentSpeed, currentDistance, pathLength;
    ofxTween tween;
    ofxEasingExpo easing;
    
    ofVec3f meshOffset;
    ofVec2f mousePos, xenoPos;

    bool bMousePressed, bXenoRunning;
    unsigned long long lastUpdateTime;
    
    float prevVel;
    bool bPaused;
};