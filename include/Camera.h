#pragma once
#include <opencv2/highgui/highgui.hpp>

/* 针孔相机类型定义 */
class Camera
{
private:
	int _index;/* 捕获图像的索引 */
	int _width, _height;/* 图像的宽与高 */
	bool _empty;/* 相机参数加载成功与否 */
	float _fx, _fy, _cx, _cy;/* 相机内参 */
public:

	Camera();

	Camera(const char * camerafile);

	/* 设置相机获取图像的索引 */
	void setCamera(int index);

	/* 获取相机拍摄的下一副图像 */
	cv::Mat getNextImage(void);

	cv::Mat getImageByIndex(int index);

	/* 若成功加载数据返回false，否则返回true */
	bool isEmpty(void);

	/* 获取x方向的相机中心 */
	float getCenterX(void);

	/* 获取y方向的相机中心 */
	float getCenterY(void);

	/* 获取x方向的焦距 */
	float getFocalLengthX(void);

	/* 获取y方向的焦距 */
	float getFocalLengthY(void);

	/* 获取图像宽 */
	int getImageWidth(void);

	/* 获取图像高 */
	int getImageHeight(void);

	/* 获取3x3的相机内参矩阵 */
	cv::Mat getIntrinsicMatrix(void);

	~Camera();
};

