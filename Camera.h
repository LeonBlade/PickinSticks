#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera
{

public:
	Camera(float x = 0.0f, float y = 0.0f);
	~Camera();

	void setPosition(float x, float y, bool relative = false);
	void setPan(float speed, float x, float y, bool relative = false);

	float getX();
	float getY();

	void draw();

private:
	float x, y;
	float speed;
	float toX, toY;

};

#endif
