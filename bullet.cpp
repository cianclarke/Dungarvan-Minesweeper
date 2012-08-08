#include <iostream>


class bullet {
  public:
    bullet(float xVal, float yVal, float zVal, float rotationVal, float radiusVal, int speedVal){
		x = xVal;
		y = yVal;
		z = zVal;
		originX = x;
		originZ = z;
		rotation = rotationVal;
		radius = radiusVal;
		speed = speedVal;	
		directionX = true; // are we incrementing or decrementing?
		directionZ = true; // "
		firing = true;

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
	float getOriginX(){
		return originX;
	}
	float getOriginZ(){
		return originZ;
	}
	float getRadius(){
		return radius;
	}
	float getRotation(){
		return rotation;
	}
	float getSpeed(){
		return speed;
	}
	bool increment(float lowerX, float upperX, float lowerZ, float upperZ){
		
		float radians = (rotation-90) * (3.14159265f / 180);
					
		float deltax = speed * cos(radians);
		float deltaz = speed * sin(radians);
		x += deltax;
		z += deltaz;

		
		if (z<= lowerZ || z>= upperZ) return false;

		return true;
	}
	bool checkCollission(float xPos, float yPos, float zPos, float enemyRadius){
		// Check distance between the two points
		float dx = xPos-this->x;
		float dy = yPos-this->y;
		float dz = zPos-this->z;
		// Typically this should be square rooted, but squaring the radius is probably cheaper?
		float distance = ((dx * dx) + (dz * dz) - (radius*radius));
		//if (enemyRadius==14.23f) printf("\n%f | %f\n   ",distance, enemyRadius*enemyRadius);
		//if (enemyRadius==14.23f) printf("calculated by %f-%f and %f-%f\n",xPos, this->x, zPos, this->z);
		if (distance<(enemyRadius*enemyRadius)){
			return true;
		}
		return false;
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
	bool isFiring(){
		return firing;
	}
	void stop(){
		firing = false;
	}

private:
	float x;
	float y;
	float z;
	float originX;
	float originZ;
	float rotation;
	float radius;
	float speed;
	bool directionX;
	bool directionZ;
	bool firing;

};

