/*
 *  Particle.h
 *  emptyExample
 *
 *  Created by Silvio Paganini on 19/01/2011.
 *  Copyright 2011 s2paganini.com. All rights reserved.
 *
 */

#ifndef PARTICLE
#define PARTICLE

#import "ofMain.h"

class Particle {
private:
	
	float x, y;
		
	float radius;
	int age;
	float maxAge;
	float speedX;
	float speedY;
	
public:
	
	Particle();
	
	void init(ofColor color, float _x, float _y);
	void update();
	void draw();
	bool dead;
	bool inited;
	bool drawn;
	
	ofColor _color;
};

#endif