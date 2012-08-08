#include <iostream>

class enemy {
  public:
    enemy(int x, int y, int z, int speed);       
    int getX();     
	int getY();            
	int getZ();  
	int getSpeed();
	void increment();
private:
	int x;
	int y;
	int z;
	int speed;
};
