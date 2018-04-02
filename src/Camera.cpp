#include "Camera.h"
#include <cstdio>

Camera::Camera()
{
	_index = 0;
	_empty = true;
}

Camera::Camera(const char * camerafile)
{
	Camera();
	FILE * fp = fopen(camerafile, "r");
	if (NULL != fp)
	{
		/* 检查fscanf的返回值，即输入成功的值的个数 */
		int n = fscanf(fp, "c_x:%f\nc_y:%f\nf_x:%f\nf_y:%f\nw:%d\nh:%d\n", &_cx, &_cy, &_fx, &_fy, &_width, &_height);
		if (n < 6)
		{
			fclose(fp);
			return;
		}
		fclose(fp);
		_empty = false;
	}
}

/* 设置相机获取图像的索引 */
void Camera::setCamera(int index)
{
	_index = index;
}

/* 获取相机拍摄的下一副图像 */
cv::Mat Camera::getNextImage(void)
{
	char filename[16];
	sprintf(filename, "./rgb/%05d.jpg", _index++);
	cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
	return image.clone();
}

cv::Mat Camera::getImageByIndex(int index)
{
	char filename[16];
	sprintf(filename, "./rgb/%05d.jpg", index);
	cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
	return image.clone();
}

/* 若成功加载数据返回false，否则返回true */
bool Camera::isEmpty(void)
{
	return _empty;
}

/* 获取x方向的相机中心 */
float Camera::getCenterX(void)
{
	return _cx;
}

/* 获取y方向的相机中心 */
float Camera::getCenterY(void)
{
	return _cy;
}

/* 获取x方向的焦距 */
float Camera::getFocalLengthX(void)
{
	return _fx;
}

/* 获取y方向的焦距 */
float Camera::getFocalLengthY(void)
{
	return _fy;
}

/* 获取图像宽 */
int Camera::getImageWidth(void)
{
	return _width;
}

/* 获取图像高 */
int Camera::getImageHeight(void)
{
	return _height;
}

/* 获取3x3的相机内参矩阵 */
cv::Mat Camera::getIntrinsicMatrix(void)
{
	cv::Mat K(cv::Mat::zeros(3, 3, CV_32F));
	K.at<float>(0, 0) = _fx; K.at<float>(0, 2) = _cx;
	K.at<float>(1, 1) = _fy; K.at<float>(1, 2) = _cy;
	K.at<float>(2, 2) = 1.f;
	return K.clone();
}

Camera::~Camera()
{
}