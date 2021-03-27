/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#ifndef CS250_PROJECTION_H
#define CS250_PROJECTION_H

#include "Camera.h"


Affine CameraToWorld(const Camera& cam);
Affine WorldToCamera(const Camera& cam);

Matrix Shadow(Point L);

Matrix CameraToNDC(const Camera& cam);
Matrix NDCToCamera(const Camera& cam);

Matrix DeviceToNdc(int width, int height);
Affine NDCToDevice(int width, int height);


#endif

