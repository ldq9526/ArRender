#pragma once
#include <GL/freeglut.h>
#include <opencv2/highgui/highgui.hpp>
#include "Camera.h"
#include "Model3D.h"

/* 渲染器定义 */
class Renderer
{
private:
	/* 把相机拍摄到的图像转换成纹理 */
	static GLuint loadTexture(cv::Mat image);

	/* 初始化光照与材质 */
	static void initializeLight(void);

	/* 面片绘制 */
	static void drawModel(void);

	/* 回调函数：键盘事件 */
	static void keyDown(unsigned char key, int x, int y);

	/* 回调函数：鼠标点击事件 */
	static void mouseClick(int button, int state, int x, int y);

	/* 回调函数：鼠标按下移动事件 */
	static void mouseMove(int x, int y);

	/* 回调函数：鼠标滚轮 */
	static void mouseWheel(int button, int state, int x, int y);

	/* 回调函数：按特殊键 */
	static void keySpecial(int key, int x, int y);

	/* 回调函数：绘制模型 */
	static void display(void);
public:

	/* 构造函数 */
	Renderer(Camera & camera, Model3D & model3d);

	/* 设置背景图片 */
	static void setBackground(const cv::Mat & background);

	/* 设置相机位姿矩阵 */
	static void setCameraPose(const cv::Mat & cameraPose);

	/* 初始化绘制环境 */
	static void initialize(int argc, char * argv[]);

	/* 绘制开始 */
	static void startRendering(void);

	~Renderer();
};

