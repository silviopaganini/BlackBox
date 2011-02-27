/*
 *  Particle.cpp
 *  emptyExample
 *
 *  Created by Silvio Paganini on 19/01/2011.
 *  Copyright 2011 s2paganini.com. All rights reserved.
 *
 */

#include "Particle.h"

Particle::Particle()
{
	speedX = ofRandom(0, .003);
	speedY = ofRandom(0.001, .05);
	radius = ofRandom(0.001, 0.006);
	
	age = 0;
	dead = false;
	inited = false;
	drawn = false;
	maxAge = ofRandom(0, 20);
}

void Particle::init(ofColor color, float _x, float _y)
{
	y = _y;
	x = _x;
	_color = color;
	
	inited = true;
}

void Particle::update()
{
	if(drawn){
	age++;
	y -= speedY;
	if (age >= maxAge) {
		dead = true;
		inited = false;
	}
	}
}

void Particle::draw()
{
	ofSetColor(_color.r * 1.5, _color.g * 1.5, _color.b * 1.5, 100-(age * 10));
	ofRect(x, y, radius, radius);
	drawn = true;
}