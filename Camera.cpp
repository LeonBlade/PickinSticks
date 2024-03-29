#include "Camera.h"

#include "OpenGL.h"
#include "Log.h"

#include <stdlib.h>
#include <math.h>`

Camera::Camera(float x, float y)
{
	this->x = x;
	this->y = y;
	this->toX = 0.0f;
	this->toY = 0.0f;
	this->speed = 0.0f;
	this->rotation = 0.0f;
	this->zoom = 1.0f;
	this->toZoom = 0.0f;
	this->toRotation = 0.0f;
	this->zoomSpeed = 0.0f;
	this->rotateSpeed = 0.0f;
}

void Camera::setPosition(float x, float y, bool relative)
{
	if (relative)
	{
		this->x += x;
		this->y += y;
	}
	else
	{
		this->x = x;
		this->y = y;
	}
}

void Camera::setPan(float speed, float x, float y, bool relative)
{
	this->speed = speed;
	if (relative)
	{
		this->toX = this->x + x;
		this->toY = this->y + y;
	}
	else
	{
		this->toX = x;
		this->toY = y;
	}
}

float Camera::getX()
{
	return this->x;
}

float Camera::getY()
{
	return this->y;
}

void Camera::setRotation(float angle, bool relative, float speed)
{
	if (speed > 0.0f)
	{
		this->toRotation = (relative) ? this->rotation + angle : angle;
		this->rotateSpeed = speed;
	}
	else
	{
		this->toRotation = 0.0f;
		this->rotation = angle;
	}
}

void Camera::setZoom(float zoom, bool relative, float speed)
{
	if (speed > 0.0f)
	{
		this->toZoom = (relative) ? this->zoom + zoom : zoom;
		this->zoomSpeed = speed;
	}
	else
	{
		this->toZoom = 0.0f;
		this->zoom = zoom;
	}
}

void Camera::draw()
{
	// if toX or toY is not 0 then we will check this
	if (this->toX != 0.0f || this->toY != 0.0f)
	{
		// get the delta changes
		float dX = this->toX - this->x;
		float dY = this->toY - this->y;

		// get -1 or 1 for direction
		int sX = (dX == 0) ? 0 : dX / fabs(dX);
		int sY = (dY == 0) ? 0 : dY / fabs(dY);

		// get the speed values
		float speedX = sX * this->speed;
		float speedY = sY * this->speed;

		// move the X and Y accordingly
		this->x += speedX;
		this->y += speedY;

		// remove the to if the destination is close enough
		if ((int)this->x == (int)this->toX) this->toX = 0.0f;
		if ((int)this->y == (int)this->toY) this->toY = 0.0f;
	}

	// if toZoom is not 0 then we increase it
	if (this->toZoom != 0.0f)
	{
		// get the delta change
		float dZ = this->toZoom - this->zoom;

		// get -1 or 1 for direction
		int sZ = (dZ == 0) ? 0 : dZ / fabs(dZ);

		// get speed for zoom
		float speed = sZ * this->zoomSpeed;

		// move the zoom accordingly
		this->zoom += speed;

		if ((int)this->zoom == (int)this->toZoom) this->toZoom = 0.0f;
	}

	// if toRotation is not 0 then we increase it
	if (this->toRotation != 0.0f)
	{
		// get the delta change
		float dR = this->toRotation - this->rotation;

		// get -1 or 1 for direction
		int sR = (dR == 0) ? 0 : dR / fabs(dR);

		// get speed for zoom
		float speed = sR * this->rotateSpeed;

		// move the zoom accordingly
		this->rotation += speed;

		if ((int)this->rotation == (int)this->toRotation) this->toRotation = 0.0f;
	}

	glRotatef(this->rotation, 0.0f, 0.0f, 1.0f);
	glScalef(this->zoom, this->zoom, 1.0f);
	glTranslatef(-this->x, -this->y, 0);
}
