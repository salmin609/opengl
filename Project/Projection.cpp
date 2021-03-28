/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include "Projection.h"

Affine CameraToWorld(const Camera& cam)
{
	const Vector u = cam.Right();
	const Vector v = cam.Up();
	const Vector n = cam.Back();
	const Point eye = cam.Eye();

	Affine result;

	result.row[0].x = u.x;
	result.row[1].x = u.y;
	result.row[2].x = u.z;

	result.row[0].y = v.x;
	result.row[1].y = v.y;
	result.row[2].y = v.z;

	result.row[0].z = n.x;
	result.row[1].z = n.y;
	result.row[2].z = n.z;

	result.row[0].w = eye.x;
	result.row[1].w = eye.y;
	result.row[2].w = eye.z;

	return result;
}

Affine WorldToCamera(const Camera& cam)
{
	return inverse(CameraToWorld(cam));
}


Matrix NDCToCamera(const Camera& cam)
{
	Affine result;
	
	result[0][0] = cam.Get_Width() / 2;
	result[0][1] = 0;
	result[0][2] = 0;
	result[0][3] = 0;

	result[1][0] = 0;
	result[1][1] = cam.Get_Height() / 2;
	result[1][2] = 0;
	result[1][3] = 0;

	result[2][0] = 0;
	result[2][1] = 0;
	result[2][2] = -cam.NearDistance();
	result[2][3] = 0;

	result[3][0] = 0;
	result[3][1] = 0;
	result[3][2] = 0;
	result[3][3] = 1;

	return result;
}

Matrix DeviceToNdc(int width, int height)
{
	Affine result;

	int check = width / 2;
	result[0][0] = 1 / static_cast<float>(check);
	result[0][1] = 0;
	result[0][2] = 0;
	result[0][3] = -1;


	check = height / 2;
	result[1][0] = 0;
	result[1][1] = -1 / static_cast<float>(check);
	result[1][2] = 0;
	result[1][3] = 1;

	result[2][0] = 0;
	result[2][1] = 0;
	result[2][2] = 1;
	result[2][3] = 0;

	result[3][0] = 0;
	result[3][1] = 0;
	result[3][2] = 0;
	result[3][3] = 1;

	return result;
}

Affine NDCToDevice(int width, int height)
{
	Affine result;

	result[0][0] = static_cast<float>(width) / 2;
	result[0][1] = 0;
	result[0][2] = 0;
	result[0][3] = -0.5f;

	result[1][0] = 0;
	result[1][1] = static_cast<float>(height) / 2;
	result[1][2] = 0;
	result[1][3] = -0.5f;

	result[2][0] = 0;
	result[2][1] = 0;
	result[2][2] = 1;
	result[2][3] = 0;

	result[3][0] = 0;
	result[3][1] = 0;
	result[3][2] = 0;
	result[3][3] = 1;

	return result;
}


Matrix Shadow(Point L)
{
	Matrix result;
	result[0][0] = L.y;
	result[0][1] = -L.x;
	result[0][2] = 0.f;
	result[0][3] = 0;

	result[1][0] = 0.f;
	result[1][1] = 0;
	result[1][2] = 0.f;
	result[1][3] = 0;

	result[2][0] = 0.f;
	result[2][1] = -L.z;
	result[2][2] = L.y;
	result[2][3] = 0;

	result[3][0] = 0.f;
	result[3][1] = -1.f;
	result[3][2] = 0.f;
	result[3][3] = L.y;

	return result;
}

Matrix CameraToNDC(const Camera& cam)
{
	const float far_dis = cam.FarDistance();
	const float near_dis = cam.NearDistance();
	const Vector viewport_info = cam.ViewportGeometry();

	const float distance_double = viewport_info.z * 2;
	const float n_minus_f = near_dis - far_dis;
	const float n_plus_f = near_dis + far_dis;
	const float d_nf = 2 * near_dis * far_dis;

	Affine result;

	result.row[0].x = distance_double / viewport_info.x;
	result.row[1].y = distance_double / viewport_info.y;
	result.row[2].z = n_plus_f / n_minus_f;
	result.row[2].w = d_nf / n_minus_f;
	result.row[3].z = -1.f;
	result.row[3].w = 0.f;

	return result;
}