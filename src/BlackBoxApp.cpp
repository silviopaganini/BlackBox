#include "BlackBoxApp.h"

//--------------------------------------------------------------
void BlackBoxApp::setup() {
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetBackgroundAuto(false);
	ofSetFrameRate(60);
	ofHideCursor();
	
	// INIT VARS
	showRGB				= true;		// show RGB camera
	vColor				= false;	// toogle colours
	vRandomColor		= false;	// toogle random colours
	zoom				= 1;		// init zoom
	kbZoom				= 1;		// keyboard zoom to calculate ease
	timerRandomColor	= 0;		// timer to swap colours
	angle				= 0;		// Kinect motor tilt
	ease				= 30;		// ease reference
	rotationPointCloud	= 0;		// point cloud rotation
	kbControlEase		= 0;		// keyboard rotation to calculate ease
	nearThreshold		= 0;		// near Threhsold  -- > 150;//25;//50;//80;//50;
	farThreshold		= 255;		// far Threshold -- > 200;//195;//80;//180;//210;//180;
	triangleComplexity	= 10;		// triangle complexity
	drawMethod			= 1;		// set draw method
	bgUpdateRatio		= 40;		// bg update ratio
	bw					= false;	// black and white
	noEffects			= false;	// toggle effects
	
	// init kinect
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	kinect.setCameraTiltAngle(angle);
	kinect.enableDepthNearValueWhite(true);
	
	
	// init particles 
	int i = 0;
	
	for(int y = 0; y < HEIGHT; y+= stepParticle) {
		for(int x = 0; x < WIDTH; x+= stepParticle) {
			
			p[i] = new Particle();	
			i++;
		}
	}
	
	
	
	// @danilo
	loadImage();
	
	colorImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThresh.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	blackAndWhite.allocate(kinect.width, kinect.height);
}

//--------------------------------------------------------------
void BlackBoxApp::update() 
{
	kinect.update();
	colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
	colorImage.flagImageChanged();
	
	blackAndWhite.setFromPixels(kinect.getPixels(), kinect.width,kinect.height);
	blackAndWhite.flagImageChanged();
	
	switch (drawMethod) {
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			contourFinderUpdate();
			triangle.clear();
			for (int iT = 0; iT < contourFinder.nBlobs; iT++){
				triangle.triangulate(contourFinder.blobs[iT], max( 3.0f, (float)contourFinder.blobs[iT].nPts/triangleComplexity));
			}
			
			break;
			
		case 3:
			updateParticles();
			break;
			
	}
}

void BlackBoxApp::updateParticles() 
{
	int i = 0;
	
	for(int y = 0; y < HEIGHT; y+= stepParticle) {
		for(int x = 0; x < WIDTH; x+= stepParticle) {
			
			p[i]->update();
			
			if(p[i]->dead == true) {
				
				p[i] = new Particle();
			}
			
			if(p[i]->inited == false)
			{
				ofPoint cur = kinect.getWorldCoordinateFor(x, y);
				ofColor color = kinect.getCalibratedColorAt(x,y);
				
				p[i]->init(color, cur.x, cur.y);
			}
			
			i++;
		}
	}
}

void BlackBoxApp::contourFinderUpdate() 
{
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayThreshFar = grayImage;
	grayThresh = grayImage;
	grayThreshFar.threshold(farThreshold, true);
	grayThresh.threshold(nearThreshold);
	cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
	
	//update the cv image
	grayImage.flagImageChanged();
	
	// find contours which are between the size of 40 pixels and 1/3 the w*h pixels.
	// also, find holes is set to true so we will get interior contours as well....
	//contourFinder.findContours(grayImage, 40, (kinect.width*kinect.height)/3, MAX_PEOPLE, false);
	contourFinder.findContours(grayImage, 20, kinect.width*kinect.height, MAX_PEOPLE, true);
}

void BlackBoxApp::toogleRGB()
{
	if(noEffects){
		colorImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		
		if(bw){
			blackAndWhite.setFromColorImage(colorImage);
			blackAndWhite.draw(0, 0, ofGetWidth(), ofGetHeight());
		}		
		
	} else{
		
		if(showRGB)	{
			
			if(drawMethod != 1 && drawMethod != 2 && drawMethod != 3){
				
				colorImage.draw(0, 0, ofGetWidth(), ofGetHeight());
				
				if(bw){
					blackAndWhite.setFromColorImage(colorImage);
					blackAndWhite.draw(0, 0, ofGetWidth(), ofGetHeight());
				}		
			} else{
				
				ofSetColor(0, 0, 0, bgUpdateRatio);
				ofRect(0, 0, ofGetWidth(), ofGetHeight());
				
				colorImage.draw(0, 0, 0, 0);
			}
			
		} else {
			
			ofSetColor(0, 0, 0, bgUpdateRatio);
			ofRect(0, 0, ofGetWidth(), ofGetHeight());
			
			colorImage.draw(0, 0, 0, 0);
		}
		
	}
	
}

void BlackBoxApp::draw() 
{
	toogleRGB();
	
	if(noEffects) return;
	
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	
	switch (drawMethod) 
	{
		case 1:
			rotationPointCloud += (kbControlEase - rotationPointCloud)/ease;
			ofRotateY(rotationPointCloud);
			
			zoom += (kbZoom - zoom) / ease;
			ofScale(400 * zoom, 400 * zoom, 400 * zoom);
			drawPointCloud();
			break;
			
		case 2:
			zoom += (kbZoom - zoom) / ease;
			ofScale(400 * zoom, 400 * zoom, 400 * zoom);
			drawCircleCloud();
			break;
			
		case 3:
			ofScale(400, 400, 400);
			drawParticlesCloud();
			break;
			
		case 4:
			ofScale(1000, 1000, 1000);
			drawPixels();
			break;
			
		case 5:
			drawTriangleLines();
			break;
			
		case 6:
			drawLaser();
			break;
			
		case 7:
			drawContour();
			break;
			
		case 8:
			drawTriangleColour();
			break;
			
		case 9:
			drawTriangleBlue();
			break;
	}
	
	ofPopMatrix();
	
	ofSetColor(255, 255, 255);
	string showrgbString = "showRGB: "+ofToString(showRGB, 2);
	//	ofDrawBitmapString(showrgbString, 200, 200);
}

void BlackBoxApp::drawParticlesCloud() 
{
	int i = 0;
	
	for(int y = 0; y < HEIGHT; y+= stepParticle) {
		for(int x = 0; x < WIDTH; x+= stepParticle) {
			
			p[i]->draw();
			i++;
		}
	}
	
}

void BlackBoxApp::drawPixels()
{	
	glBegin(GL_QUADS);
	
	int step = 5;
	
	if(timerRandomColor % 20 == 0){
		r = ofRandom(0, 255);
		g = ofRandom(0, 255);
		b = ofRandom(0, 255);
		timerRandomColor = 0;
	}
	
	timerRandomColor++;
	
	ofPushStyle();
	ofFill();
	
	for(int y = 0; y < HEIGHT; y += step) {
		for(int x = 0; x < WIDTH; x += step) {
			
			ofPoint cur = kinect.getWorldCoordinateFor(x, y);
			ofColor color = kinect.getCalibratedColorAt(x, y);
			
			if(vRandomColor)
			{
				ofSetColor(r,g,b);
			} else {
				glColor3ub((unsigned char)color.r,(unsigned char)color.g,(unsigned char)color.b);
			}
			
			float size = cur.z / 110;
			
			if(cur.z < 2){
				
				glVertex2f(cur.x, cur.y);
				glVertex2f(cur.x + size, cur.y);
				glVertex2f(cur.x + size, cur.y + size); 
				glVertex2f(cur.x, cur.y + size);
				
			}
		}
	}
	
	ofPopStyle();
	
	glEnd();
}

void BlackBoxApp::drawPointCloud() 
{
	
	glBegin(GL_POINTS);
	
	int step = 2;
	
	if(timerRandomColor % 20 == 0){
		r = ofRandom(0, 255);
		g = ofRandom(0, 255);
		b = ofRandom(0, 255);
		timerRandomColor = 0;
	}
	
	timerRandomColor++;
	
	ofPushStyle();
	
	for(int y = 0; y < HEIGHT; y += step) {
		for(int x = 0; x < WIDTH; x += step) {
			ofPoint cur = kinect.getWorldCoordinateFor(x, y);
			ofColor color = kinect.getCalibratedColorAt(x,y);
			
			if(vColor){
				if(vRandomColor){
					
					glColor3ub(r, g, b);
					
				} else {
					
					glColor3ub((unsigned char)color.r,(unsigned char)color.g,(unsigned char)color.b);
				}
				
			} else {
				glColor3ub(255, 255, 255);
			}
			
			glVertex3f(cur.x, cur.y, cur.z);
		}
	}
	
	ofPopStyle();
	glEnd();
}

void BlackBoxApp::drawCircleCloud()
{
	int step = 10;
	
	if(timerRandomColor % 20 == 0){
		r = ofRandom(0, 255);
		g = ofRandom(0, 255);
		b = ofRandom(0, 255);
		timerRandomColor = 0;
	}
	
	timerRandomColor++;
	
	ofPushStyle();
	ofFill();
	
	for(int y = 0; y < HEIGHT; y += step) {
		for(int x = 0; x < WIDTH; x += step) {
			
			ofPoint cur = kinect.getWorldCoordinateFor(x, y);
			ofColor color = kinect.getCalibratedColorAt(x, y);
			
			if(vRandomColor)
			{
				ofSetColor(r,g,b);
			} else {
				ofSetColor((unsigned char)color.r,(unsigned char)color.g,(unsigned char)color.b);
			}
			
			ofCircle(cur.x, cur.y, .01);
		}
	}
	
	ofPopStyle();
}

void BlackBoxApp::drawContour()
{
	int r = ofRandom(0, 255);
	int g = ofRandom(0, 255);
	int b = ofRandom(0, 255);
	
	if(timerRandomColor % 20 == 0 && vRandomColor){
		randomColorContour.r = r;
		randomColorContour.g = g;
		randomColorContour.b = b;
		timerRandomColor = 0;
	} 
	
	if(!vRandomColor && !bw){
		
		randomColorContour.r = ofRandom(0, 150);
		randomColorContour.g = ofRandom(75, 220);
		randomColorContour.b = ofRandom(200, 255);
		timerRandomColor = 0;
		
	} else if(bw){
		
		randomColorContour.r = 255;
		randomColorContour.g = 255;
		randomColorContour.b = 255;
		timerRandomColor = 0;
		
	}
	
	timerRandomColor++;
	
	contourFinder.drawNoBox(-ofGetWidth()*0.5, -ofGetHeight()*0.5, ofGetWidth(), ofGetHeight(), randomColorContour);
}

void BlackBoxApp::drawTriangleLines()
{
	ofPushStyle();
	
	ofxTriangleData* tData;
	int i = triangle.triangles.size() - 1;
	int step = 1;  //ofRandom(1, 10);
	for (; i>=0; i-=step) {
		tData = &triangle.triangles[i];
		
		if(vRandomColor){
			ofSetColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255), 255);
		} else {
			ofSetColor(ofRandom(0, 150), ofRandom(75, 220), ofRandom(200, 255), 255);
		}
		
		
		ofLine(getResizedX(tData->a.x), getResizedY(tData->a.y), getResizedX(tData->b.x), getResizedY(tData->b.y));
		ofLine(getResizedX(tData->b.x), getResizedY(tData->b.y), getResizedX(tData->c.x), getResizedY(tData->c.y));
		ofLine(getResizedX(tData->c.x), getResizedY(tData->c.y), getResizedX(tData->a.x), getResizedY(tData->a.y));
	}
	
	ofPopStyle();
}

void BlackBoxApp::drawTriangleColour() {
    ofPushStyle();
    
    ofFill();
    int i = 0;
    int l = triangle.nTriangles;
    for (; i<l; i++) {
        ofSetColor(ofRandom(0, 0xffffff));
        ofTriangle(getResizedX(triangle.triangles[i].a.x), getResizedY(triangle.triangles[i].a.y),
                   getResizedX(triangle.triangles[i].b.x), getResizedY(triangle.triangles[i].b.y),
                   getResizedX(triangle.triangles[i].c.x), getResizedY(triangle.triangles[i].c.y));
    }
    
    ofPopStyle();
}

void BlackBoxApp::drawTriangleBlue() {
    ofPushStyle();
    
    ofFill();
    int i = 0;
    int l = triangle.nTriangles;
    for (; i<l; i++) {
        ofSetColor(ofRandom(0, 150), ofRandom(75, 220), ofRandom(200, 255), 255);
        ofTriangle(getResizedX(triangle.triangles[i].a.x), getResizedY(triangle.triangles[i].a.y),
                   getResizedX(triangle.triangles[i].b.x), getResizedY(triangle.triangles[i].b.y),
                   getResizedX(triangle.triangles[i].c.x), getResizedY(triangle.triangles[i].c.y));
    }
    
    ofPopStyle();
}

void BlackBoxApp::drawLaser()
{
	ofPushStyle();
	
	ofxTriangleData* tData;
	int i = triangle.triangles.size() - 1;
	int step = 1;//ofRandom(1, 10);
	int scale;
	for (; i>=0; i-=step) {
		tData = &triangle.triangles[i];
		
		//lines
		ofSetColor(ofRandom(0, 150), ofRandom(75, 220), ofRandom(200, 255), ofRandom(1, 20));
		ofLine(getResizedX(tData->a.x), getResizedY(tData->a.y), 0, 0);
		ofLine(getResizedX(tData->b.x), getResizedY(tData->b.y), 0, 0);
		ofLine(getResizedX(tData->c.x), getResizedY(tData->c.y), 0, 0);
		
		//gradient center
		//ofSetColor(0, 0, 0, 10);
		//image.draw(0.0, 0.0, 700, 700);	
		
		//dots
		ofSetColor(ofRandom(0, 150), ofRandom(75, 220), ofRandom(200, 255), 40);
		
		scale = ofRandom(1, 100);
		ofPushMatrix();
		ofTranslate(getResizedX(tData->a.x), getResizedY(tData->a.y), 0);
		ofRotateX(ofRandom(0, 360));
		//ofRotateY(ofRandom(0, 360));
		//ofRotateZ(ofRandom(0, 360));
		image.draw(0.0, 0.0, scale, scale);
		ofPopMatrix();
		
		scale = ofRandom(1, 100);
		ofPushMatrix();
		ofTranslate(getResizedX(tData->b.x), getResizedY(tData->b.y), 0);
		//ofRotateX(ofRandom(0, 360));
		//ofRotateY(ofRandom(0, 360));
		ofRotateZ(ofRandom(0, 360));
		image.draw(0.0, 0.0, scale, scale);
		ofPopMatrix();
		
		scale = ofRandom(1, 100);
		ofPushMatrix();
		ofTranslate(getResizedX(tData->c.x), getResizedY(tData->c.y), 0);
		//ofRotateX(ofRandom(0, 360));
		ofRotateY(ofRandom(0, 360));
		//ofRotateZ(ofRandom(0, 360));
		image.draw(0.0, 0.0, scale, scale);
		ofPopMatrix();
		
		
		//ofLine(getResizedX(tData->a.x), getResizedY(tData->a.y), getResizedX(tData->b.x), getResizedY(tData->b.y));
		//ofLine(getResizedX(tData->b.x), getResizedY(tData->b.y), getResizedX(tData->c.x), getResizedY(tData->c.y));
		//ofLine(getResizedX(tData->c.x), getResizedY(tData->c.y), getResizedX(tData->a.x), getResizedY(tData->a.y));
		
		
		//ofSetColor(ofRandom(0, 150), ofRandom(75, 220), ofRandom(200, 255), 255);
		//ofLine(getResizedX(tData->a.x)*zoom, getResizedY(tData->a.y)*zoom, getResizedX(tData->b.x)*zoom, getResizedY(tData->b.y)*zoom);
		//ofLine(getResizedX(tData->b.x)*zoom, getResizedY(tData->b.y)*zoom, getResizedX(tData->c.x)*zoom, getResizedY(tData->c.y)*zoom);
		//ofLine(getResizedX(tData->c.x)*zoom, getResizedY(tData->c.y)*zoom, getResizedX(tData->a.x)*zoom, getResizedY(tData->a.y)*zoom);
	}
	
	ofPopStyle();
}

//--------------------------------------------------------------
float BlackBoxApp::getResizedX(float x) {
	return (x * ofGetWidth()) / contourFinder.getWidth() - ofGetWidth()/2;
}

float BlackBoxApp::getResizedY(float y) {
	return (y * ofGetHeight()) / contourFinder.getHeight() - ofGetHeight()/2;
}

void BlackBoxApp::loadImage() 
{
#ifdef TARGET_OSX   
	// Get the absolute location of the executable file in the bundle.
	CFBundleRef appBundle     = CFBundleGetMainBundle();
	CFURLRef   executableURL = CFBundleCopyExecutableURL(appBundle);
	char execFile[4096];
	if (CFURLGetFileSystemRepresentation(executableURL, TRUE, (UInt8 *)execFile, 4096))
	{
		// Strip out the filename to just get the path
		string strExecFile = execFile;
		int found = strExecFile.find_last_of("/");
		string strPath = strExecFile.substr(0, found);
		
		// Change the working directory to that of the executable
		if(-1 == chdir(strPath.c_str())) {
			ofLog(OF_LOG_ERROR, "Unable to change working directory to executable's directory.");
		}
	}
	else {
		ofLog(OF_LOG_ERROR, "Unable to identify executable's directory.");
	}
	CFRelease(executableURL);
#endif
	
	image.loadImage("image.png");
	image.setAnchorPercent(0.5, 0.5);
}	

void BlackBoxApp::setDrawMethod(int method)
{
	noEffects = false;
	drawMethod = method;
}

void BlackBoxApp::exit(){
	kinect.close();
}

void BlackBoxApp::keyPressed (int key) {
	
	switch (key) {
			
		case '1':
			setDrawMethod(1);
			break;
		case '2':
			setDrawMethod(2);
			break;
		case '3':
			setDrawMethod(3);
			break;
		case '4':
			setDrawMethod(4);
			break;
		case '5':
			setDrawMethod(5);
			break;
		case '6':
			setDrawMethod(6);
			break;
		case '7':
			setDrawMethod(7);
			break;	
		case '8':
			setDrawMethod(8);
			break;
		case '9':
			setDrawMethod(9);
			break;
			
		case 'C':
		case 'c':
			vColor = !vColor;
			break;
			
		case 'b':
		case 'B':
			bw = !bw;
			break;
			
		case 'R':
		case 'r':
			vRandomColor = !vRandomColor;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'M':
		case 'm':
			showRGB = !showRGB;
			break;
			
		case 'Z':
		case 'z':
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case 'X':
		case 'x':
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case 'H':
		case 'h':
			noEffects = !noEffects;
			bw = false;
			break;
			
		case '[':
		case '{':
			bgUpdateRatio--;
			if(bgUpdateRatio < 0) bgUpdateRatio = 0;
			break;
			
		case ']':
		case '}':
			bgUpdateRatio++;
			if(bgUpdateRatio > 100) bgUpdateRatio = 100;
			break;
			
		case 'T':
		case 't':
			triangleComplexity++;
			if(triangleComplexity>200) triangleComplexity=200;
			break;
			
		case 'Y':
		case 'y':
			triangleComplexity--;
			if(triangleComplexity<1) triangleComplexity=1;
			break;			
			
		case OF_KEY_RIGHT:
			if(drawMethod == 1)kbControlEase += 10;
			break;
			
		case OF_KEY_LEFT:
			if(drawMethod == 1)kbControlEase-=10;
			break;
			
		case OF_KEY_UP:
			kbZoom = kbZoom < 10 ? kbZoom+.25 : kbZoom;
			break;
			
		case OF_KEY_DOWN:
			kbZoom = kbZoom > 0 ? kbZoom-.25 : kbZoom;
			break;
			
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
	}
}