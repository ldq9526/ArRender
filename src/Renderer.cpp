#include <cstring>
#include "Renderer.h"

static float _w, _h;/* 窗口的宽与高 */
static int _oldx, _oldy;/* 鼠标移动之前的位置 */
static float _angleX, _angleY, _moveX, _moveY, _moveZ;/* 绕x,y轴转动的角度，沿x,y,z轴移动的距离 */
static bool _buttonleft;/* 鼠标左键是否点击 */
static GLfloat _texture_coordinate[8];/* 纹理的4个点的坐标 */
static GLfloat _image_coordinate[8];/* 图像4个点的坐标 */
static GLfloat _texture_projection[16];/* 纹理贴图采用的投影矩阵 */
static GLfloat _model_projection[16];/* 绘制模型采用的投影矩阵 */
static Model3D _model3d;/* 需要绘制的3d模型 */
static Camera _camera;/* 当前使用的照相机 */
static cv::Mat _background;/* 相机获取的图像作为背景 */
static cv::Mat _cameraPose;/* 4x4的相机的位姿矩阵 */

/* 光照系数 */
static GLfloat _light_ambient[4] = { 0.f, 0.f, 0.f, 1.f };
static GLfloat _light_specular[4] = { 0.2f, 0.2f, 0.2f, 1.f };
static GLfloat _light_diffuse[4] = { 1.f, 1.f, 1.f, 1.f };

static GLfloat _light_position[8][4] =
{
	{ 50.f, 50.f, 50.f, 1.f },
	{ -50.f, 50.f, 50.f, 1.f },
	{ 50.f, -50.f, 50.f, 1.f },
	{ 50.f, 50.f, -50.f, 1.f },
	{ -50.f, -50.f, 50.f, 1.f },
	{ -50.f, 50.f, -50.f, 1.f },
	{ 50.f, -50.f, -50.f, 1.f },
	{ -50.f, -50.f, -50.f, 1.f },
};
static GLfloat _light_direction[8][4] =
{
	{ -50.f, -50.f, -50.f, 1.f },
	{ 50.f, -50.f, -50.f, 1.f },
	{ -50.f, 50.f, -50.f, 1.f },
	{ -50.f, -50.f, 50.f, 1.f },
	{ 50.f, 50.f, -50.f, 1.f },
	{ 50.f, -50.f, 50.f, 1.f },
	{ -50.f, 50.f, 50.f, 1.f },
	{ 50.f, 50.f, 50.f, 1.f },
};
/* 材质系数 */
static GLfloat _material_ambient[4] = { 0.f, 0.f, 0.f, 1.f };
static GLfloat _material_specular[4] = { 0.3f, 0.3f, 0.3f, 1.f };

/* 把相机拍摄到的图像转换成纹理 */
GLuint Renderer::loadTexture(cv::Mat image)
{
	size_t lengthPixels = image.cols * image.rows * image.channels();
	uchar * pixels = (uchar *)malloc(lengthPixels * sizeof(uchar));
	if (pixels == NULL)
		return 0;
	memcpy(pixels, image.data, lengthPixels*sizeof(uchar));
	GLuint textureId;
	glGenTextures(1, &textureId);
	if (textureId == 0)
		return 0;
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	free(pixels);
	return textureId;
}

/* 模型绘制 */
void Renderer::drawModel(void)
{
	std::vector<mesh> m = _model3d.getMeshList();
	std::vector<vertex> v = _model3d.getVertexList();
	for (unsigned int i = 0; i < m.size(); i++)
	{
		glBegin(GL_POLYGON);
		glNormal3fv(m[i].n);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m[i].c);
		glVertex3fv(v[m[i].v[0]].p);
		glVertex3fv(v[m[i].v[1]].p);
		glVertex3fv(v[m[i].v[2]].p);
		glVertex3fv(v[m[i].v[3]].p);
		glEnd();
	}
}

/* 初始化光照与材质 */
void Renderer::initializeLight(void)
{
	int lightId[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
	for (int id = 0; id < 8; id++)
	{
		glLightfv(lightId[id], GL_POSITION, _light_position[id]);
		glLightfv(lightId[id], GL_SPOT_DIRECTION, _light_direction[id]);
		glLightfv(lightId[id], GL_AMBIENT, _light_ambient);
		glLightfv(lightId[id], GL_DIFFUSE, _light_diffuse);
		glLightfv(lightId[id], GL_SPECULAR, _light_specular);
		glEnable(lightId[id]);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, _material_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, _material_specular);
}

/* 键盘事件 */
void Renderer::keyDown(unsigned char key, int x, int y)
{
	static FILE * fp = NULL;
	if (NULL == fp)
		fp = fopen("transformation.txt", "r");
	if (key == 's' || key == 'S')
	{
		if (NULL != fp)
		{
			int index;
			float t[4][4];
			if (fscanf(fp, "%d\n", &index) > 0)
			{
				setBackground(_camera.getImageByIndex(index));
				int n = fscanf(fp, "[%f,%f,%f,%f;\n%f,%f,%f,%f;\n%f,%f,%f,%f;\n%f,%f,%f,%f]\n"
					, &t[0][0], &t[0][1], &t[0][2], &t[0][3]
					, &t[1][0], &t[1][1], &t[1][2], &t[1][3]
					, &t[2][0], &t[2][1], &t[2][2], &t[2][3]
					, &t[3][0], &t[3][1], &t[3][2], &t[3][3]);
				/*t[0][3] *= 10.f;
				t[1][3] *= 10.f;
				t[2][3] *= 10.f;*/
				cv::Mat cameraPose(4, 4, CV_32F, t);
				setCameraPose(cameraPose);
				glutPostRedisplay();
			}
			else
			{
				fclose(fp);
				fp = NULL;
			}
		}
	}
}

/* 鼠标点击事件 */
void Renderer::mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//右键按下
	{
		_buttonleft = false;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		_buttonleft = false;
		_oldx = x;
		_oldy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//左键按下
	{
		_buttonleft = true;//标志左键
		_oldx = x;//记录旧的x值与y值
		_oldy = y;
	}
}

/* 鼠标按下移动事件 */
void Renderer::mouseMove(int x, int y)
{
	if (_buttonleft)//如果鼠标左键拖着移动
	{
		int gapX = x - _oldx;
		int gapY = y - _oldy;
		_oldx = x;
		_oldy = y;
		_angleX += float(gapY);
		_angleY += float(gapX);
		glutPostRedisplay();//重绘界面
	}
}

/* 鼠标滚轮 */
void Renderer::mouseWheel(int button, int state, int x, int y)
{
	if (state == 1)//向前滚,向z轴正半轴撤，距离变远
		_moveZ += 0.2f;
	else
		_moveZ -= 0.2f;
	glutPostRedisplay();//重绘界面
}

/* 按特殊键 */
void Renderer::keySpecial(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		_moveX -= 0.1f;
		break;
	case GLUT_KEY_RIGHT:
		_moveX += 0.1f;
		break;
	case GLUT_KEY_DOWN:
		_moveY -= 0.1f;
		break;
	case GLUT_KEY_UP:
		_moveY += 0.1f;
		break;
	case GLUT_KEY_PAGE_UP:
		_moveZ += 0.2f;
		break;
	case GLUT_KEY_PAGE_DOWN:
		_moveZ -= 0.2f;
	}
	glutPostRedisplay();/* 重绘界面 */
}

/* 绘制模型 */
void Renderer::display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint textureId = loadTexture(_background);

	/* 纹理贴图 */
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(_texture_projection);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);	//允许使用纹理
	glBindTexture(GL_TEXTURE_2D, textureId);	//选择纹理对象
	glVertexPointer(2, GL_FLOAT, 0, _image_coordinate);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, _texture_coordinate);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor4f(1, 1, 1, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);	//禁止使用纹理

	/* 绘制3d模型 */
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(_model_projection);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cv::Mat T = _cameraPose.t();
	glTranslatef(0.f, 0.f, -10.f + _moveZ);
	glMultMatrixf((GLfloat *)T.data);
	glTranslatef(_moveX, _moveY, -10.f + _moveZ);
	glRotatef(_angleX, 1.f, 0.f, 0.f);
	glRotatef(_angleY, 0.f, 1.f, 0.f);
	glRotatef(0.f, 0.f, 0.f, 1.f);
	drawModel();
	glutSwapBuffers();
}

/* 构造函数 */
Renderer::Renderer(Camera & camera, Model3D & model3d)
{
	_model3d = Model3D(model3d);
	_camera = Camera(camera);
	_cameraPose = cv::Mat::eye(4, 4, CV_32F);
	_buttonleft = false;
	_angleX = _angleY = _moveX = _moveY = _moveZ = 0.f;
	_w = float(_camera.getImageWidth()); _h = float(_camera.getImageHeight());
	const static GLfloat image_coordinate[] =
	{
		0.f, 0.f,
		_w, 0.f,
		0.f, _h,
		_w, _h
	};
	memcpy(_image_coordinate, image_coordinate, 8 * sizeof(GLfloat));
	const static GLfloat texture_coordinate[] =
	{
		1.f, 0.f,
		1.f, 1.f,
		0.f, 0.f,
		0.f, 1.f
	};
	memcpy(_texture_coordinate, texture_coordinate, 8 * sizeof(GLfloat));
	const static GLfloat texture_projection[] =
	{
		0.f, -2.f / _w, 0.f, 0.f,
		-2.f / _h, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		1.f, 1.f, 0.f, 1.f
	};
	memcpy(_texture_projection, texture_projection, 16 * sizeof(GLfloat));
	float n = 0.01f, f = 100.f;
	float f_x = _camera.getFocalLengthX(), f_y = _camera.getFocalLengthY(),
		c_x = _camera.getCenterX(), c_y = _camera.getCenterY();
	const static GLfloat model_projection[] =
	{
		2.f * f_x / _w, 0.f, 0.f, 0.f,
		0.f, 2.f * f_y / _h, 0.f, 0.f,
		1.f - 2.f * c_x / _w, 2.f * c_y / _h - 1.f, -(f + n) / (f - n), -1.f,
		0.f, 0.f, -2.f * f * n / (f - n), 0.f
	};
	memcpy(_model_projection, model_projection, 16 * sizeof(GLfloat));
}

/* 设置背景图片 */
void Renderer::setBackground(const cv::Mat & background)
{
	_background = background.clone();
}

/* 设置相机位姿矩阵 */
void Renderer::setCameraPose(const cv::Mat & cameraPose)
{
	_cameraPose = cameraPose.clone();
}

/* 初始化绘制环境 */
void Renderer::initialize(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(int(_w), int(_h));
	glutCreateWindow("GLWindow");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	initializeLight();
	glutDisplayFunc(Renderer::display);
	glutSpecialFunc(Renderer::keySpecial);
	glutMouseFunc(Renderer::mouseClick);
	glutMotionFunc(Renderer::mouseMove);
	glutMouseWheelFunc(Renderer::mouseWheel);
	glutKeyboardFunc(Renderer::keyDown);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}

void Renderer::startRendering(void)
{
	glutMainLoop();
}

Renderer::~Renderer()
{
}
