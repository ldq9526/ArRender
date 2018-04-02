#pragma once
#include <vector>

/* 顶点类型定义 */
class vertex
{
public:
	float p[3], n[3];/* 顶点坐标与法向量 */
	vertex(float x, float y, float z, float nx, float ny, float nz);
};

/* 面片类型定义 */
class mesh
{
public:
	/* 面片的顶点 */
	int v[4];
	/* 面片的法向量、颜色 */
	float c[3], n[3];/* 面片颜色与法向量 */
	mesh(int vi[], float nx, float ny, float nz, float r, float g, float b);
};

/* 简单3d模型类型定义 */
class Model3D
{
private:
	std::vector<vertex> _v;/* 顶点列表 */
	std::vector<mesh> _m;/* 面片列表 */
	bool _empty;/* 数据加载成功与否 */
public:
	Model3D();

	Model3D(const char * modelfile);

	bool isEmpty(void);

	/* 获取顶点列表 */
	const std::vector<vertex> getVertexList(void);

	/* 获取面片列表 */
	const std::vector<mesh> getMeshList(void);

	~Model3D();
};

