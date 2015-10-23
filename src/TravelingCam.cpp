//
//  TravelingCam.cpp
//  floatingPointImageExample
//
//  Created by Roy Macdonald on 10-06-15.
//
//

#include "TravelingCam.h"
#include "pathSaveLoad.h"
#define DEFAULT_PATH "defaultCamPath"
//------------------------------------------------
void TravelingCam::setup(ofVec3f meshOffset){
    parameters.setName("Traveling Cam");
    parameters.add(speed.set("Cam Speed", 0.001, 0, 0.1));
    parameters.add(rampTime.set("Start/Stop ramp", 500, 0, 5000));
    parameters.add(altura.set("altura camara", 0, -100, 100 ));
    
    resolucionCurvas.addListener(this, &TravelingCam::curveResChanged);
    
    parameters.add(resolucionCurvas.set("resolucion Curvas", 20, 2, 130));
    
    parameters.add(xenoFactor.set("xenoFactor", 0.5,0,1));
    
    this->meshOffset = meshOffset;
    
    ofFile f(DEFAULT_PATH);
    if (f.exists()) {
        pathSaveLoad::loadPolylineVertices(DEFAULT_PATH, path, meshOffset, resolucionCurvas);
    }
    xenoFactor = 0.5;
    xenoPos.set(ofGetWidth()/2, ofGetHeight()/2);
    enableMouseInput();
    slowdownDist = speed*rampTime;
    
    heightMult =1;
}
//------------------------------------------------
void TravelingCam::enableMouseInput(bool e){
    if(e != bMouseEnabled){
        if (e) {
            ofRegisterMouseEvents(this);
        }else{
            ofUnregisterMouseEvents(this);
        }
        bMouseEnabled = e;
    }
}
//------------------------------------------------
void TravelingCam::initCamPos(){
    if (path.size()>1) {
        setPosition(path.getVertices()[0]);
        ofPoint p = path.getVertices()[1];
        ofPoint p0 = path.getVertices()[0];
        
        dirVec  = p + (p - p0).normalize();
        lookAt(dirVec, ofVec3f(0,1,0));
        
        boom(altura);
    }
}
//------------------------------------------------
void TravelingCam::curveResChanged(int& r){
    ofFile f(DEFAULT_PATH);
    if (f.exists()) {
        pathSaveLoad::loadPolylineVertices(DEFAULT_PATH, path,meshOffset, resolucionCurvas);
    }
}
//------------------------------------------------
void TravelingCam::customDraw(){
    
    ofPushStyle();
    ofSetColor(ofColor::gray);
    ofDrawBox(10);
    ofDrawAxis(20);
    
    ofVec3f l = getLookAtDir();
    l.normalize();
    l*=200;
    ofSetColor(ofColor::magenta);
    ofSetLineWidth(3);
    ofDrawArrow(ofVec3f(0), ofVec3f(0, 0, -200));
    ofPopStyle();
}
//------------------------------------------------
void TravelingCam::tweenEnded(int &i){
    bTweening = false;
}
//------------------------------------------------
void TravelingCam::update(ofEventArgs& a){

    unsigned long long t = ofGetElapsedTimeMillis();
    if (bTweening) {
        currentSpeed = tween.update();
    }else if (pathLength < currentDistance + slowdownDist){
        
        currentSpeed = ofMap(currentDistance, pathLength - slowdownDist, pathLength, 0, speed);
        
    }else{
        currentSpeed = speed;
    }
    
    
    
    unsigned long long dt = t - lastUpdateTime;
    currentDistance += currentSpeed * dt;
    
    ofPoint p = path.getPointAtLength(currentDistance);
    float inc = 10;

    if (pathLength < currentDistance + inc ) {
        inc = pathLength - currentDistance;
    }
    ofPoint p0 = path.getPointAtLength(currentDistance + inc);
    
    p.y *= heightMult;
    p0.y *= heightMult;
    //lookAt( p + (p - p0).normalize());
    setPosition( p);
    dirVec  = p + (p0 - p).normalize();

    lookAt(dirVec, ofVec3f(0,1,0));
    
    boom(altura);
    if(bMousePressed || bXenoRunning){
        ofVec2f c (ofGetWidth()/2, ofGetHeight()/2);
        

        
        ofVec2f mouseNorm = mousePos - c;
        xenoPos = xenoPos + (mouseNorm- xenoPos)*xenoFactor;
        if ((mouseNorm - xenoPos).length() < 1) {
            xenoPos = mouseNorm;
            bXenoRunning = false;
        }
        
//        mouseNorm.x /= c.x;
  //      mouseNorm.y /= c.y;
        
        rotate(ofMap(xenoPos.x, -c.x, c.x, 90, -90), getYAxis());
        rotate(ofMap(xenoPos.y, -c.y, c.y, 60, -60), getXAxis());
    }
    lastUpdateTime = t;
}
//------------------------------------------------
void TravelingCam::mouseDragged(ofMouseEventArgs& args){
    dragged(args.x, args.y, args.button);
}
//------------------------------------------------
void TravelingCam::mousePressed(ofMouseEventArgs& args){
    pressed(args.x, args.y, args.button);
}
//------------------------------------------------
void TravelingCam::mouseReleased(ofMouseEventArgs& args){
    released(args.x, args.y, args.button);
}
//------------------------------------------------
void TravelingCam::dragged(int x, int y, int button){
    mousePos.set(x, y);
    bMousePressed = true;
}
//------------------------------------------------
void TravelingCam::pressed(int x, int y, int button){
    mousePos.set(x, y);
    bMousePressed = true;
    bXenoRunning = true;
}
//------------------------------------------------
void TravelingCam::released(int x, int y, int button){
    mousePos.set(ofGetWidth()/2, ofGetHeight()/2);
    bMousePressed = false;
    bXenoRunning = true;
}
//------------------------------------------------
void TravelingCam::touchDown(ofTouchEventArgs & touch){
    pressed(touch.x, touch.y, 0);
}
//------------------------------------------------
void TravelingCam::touchMoved(ofTouchEventArgs & touch){
    dragged(touch.x, touch.y, 0);
}
//------------------------------------------------
void TravelingCam::touchUp(ofTouchEventArgs & touch){
    released(touch.x,touch.y, 0);
}
//------------------------------------------------
void TravelingCam::togglePause(){
    pause(!bPaused);
}
//------------------------------------------------
void TravelingCam::pause(bool p){
    if (p!=bPaused) {
        bPaused = p;
        if (p) {
            prevVel = speed;
            speed.set(0);
        }else{
            speed.set(prevVel);
        }
    }
}
//------------------------------------------------
void TravelingCam::start(){
    if (path.size()>1) {
        if (!bStarted) {
            ofAddListener(ofEvents().update, this, &TravelingCam::update);
        }
        tween.setParameters(easing, ofxTween::easeIn, 0, speed, rampTime, 0);
        tween.start();
        setPosition(path.getVertices()[0]);
        currentDistance = 0;
        bStarted = bTweening = true;
        pathLength = path.getPerimeter();
        lastUpdateTime = ofGetElapsedTimeMillis();
        ofAddListener(tween.end_E, this, &TravelingCam::tweenEnded);
    }
}
//------------------------------------------------
void TravelingCam::stop(){
    if (bStarted) {
        ofRemoveListener(ofEvents().update, this, &TravelingCam::update);
        enableMouseInput(false);
        bStarted = false;
    }
    
}
