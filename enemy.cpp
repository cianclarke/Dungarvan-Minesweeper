//#include "enemy.h"


#include <iostream>


class enemy {
  public:
    enemy(float xVal, float yVal, float zVal, float radiusVal, float speedVal){
		x = xVal;
		y = yVal;
		z = zVal;
		radius = radiusVal;
		speed = speedVal;	
		directionX = true; // are we incrementing or decrementing?
		directionZ = true; // "
		dead = false;
	}       
	float getX(){
		return x;
	}
	float getY(){
		return y;
	}
	float getZ(){
		return z;
	}
	float getRadius(){
		return radius;
	}
	void setRadius(float r){
		radius = r;
	}
	float getSpeed(){
		return speed;
	}
	void increment(float lowerX, float upperX, float lowerZ, float upperZ){
		//printf("setting X to %i with upperbound %i | and Z to %i with upperbound %i\n", x, upperX, z, upperZ);

		
		if (x<=upperX && directionX){
			x+=speed;
		}
		if (x>=lowerX && !directionX){
			x-=speed;
		}
		if (x>=upperX) {
			directionX = false;
		}
		if (x<=lowerX){
			directionX  = true;
		}
		
		if (z<=upperZ && directionZ){
			z+=speed;
		}
		if (z>=lowerZ && !directionZ){
			z-=speed;
		}
		if (z>=upperZ) {
			directionZ = false;
		}
		if (z<=lowerZ){
			directionZ  = true;
		}
		
	}
	bool checkCollission(float xPos, float yPos, float zPos){
		// Check distance between the two points
		float dx = xPos-x;
		float dy = yPos-y;
		float dz = zPos-z;
		// Typically this should be square rooted, but squaring the radius is probably cheaper?
		float distance = (dx * dx) + (dz * dz);
		if (distance<(radius*radius)){
			return true;
		}
		return false;
	}
	void kill(){
		dead = true;
		radius = 0.0;
	}
	bool isDead(){
		return dead;
	}
	void reverseDirection(){
		if (directionX){
			directionX = false;
		}else{
			directionX = true;
		}
		if (directionZ){
			directionZ = false;
		}else{
			directionZ = true;
		}
	}

private:
	float x;
	float y;
	float z;
	float radius;
	float speed;
	bool directionX;
	bool directionZ;
	bool dead;
};

