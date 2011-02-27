#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxTriangle.h"
#include "Particle.h"

#define WIDTH 640
#define HEIGHT 480
#define MAX_PEOPLE 5
#define stepParticle 2

class BlackBoxApp : public ofBaseApp {
public:
	
		// OFX
	void	setup();
	void	update();
	void	draw();
	void	exit();
	void	keyPressed (int key);
	
		// EFFECTS
	void	drawPointCloud();
	void	drawParticlesCloud();
	void	drawCircleCloud();
	void	drawPixels();
	void	drawTriangleLines();
	void	drawTriangleColor();
	void	drawTriangleBlue();
	void 	drawLaser();
	void 	drawContour();
	
		// HELPERS
	void	loadImage();
	void	contourFinderUpdate();
	void	updateParticles();
	void	toogleRGB();
	void	setDrawMethod(int method);
	
		// VARS
	Particle*				p[HEIGHT * WIDTH];
	ofImage					image;
			
	float					ease;
	float					rotationPointCloud;
	float					kbControlEase, zoom, kbZoom;
	float					getResizedX (float);
	float					getResizedY (float);
	
	int						timerRandomColor;
	int						r, g, b;
	int						drawMethod, nearThreshold, farThreshold, angle;
	int						triangleComplexity; //the smaller the more complex and slower
	int						bgUpdateRatio;
		
	bool					vColor, vRandomColor, showRGB, bw, noEffects;

	ofxTriangle				triangle;
	ofxKinect				kinect;
	ofxCvColorImage			colorImage;
	ofxCvGrayscaleImage		blackAndWhite;
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayThresh;
	ofxCvGrayscaleImage 	grayThreshFar;
	ofxCvContourFinder		contourFinder;
	ofColor					randomColorContour;
};
