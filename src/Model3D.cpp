#include "Model3D.h"
#include <vector>
#include <cstdio>

vertex::vertex(float x, float y, float z, float nx, float ny, float nz)
{
	p[0] = x; p[1] = y; p[2] = z;
	n[0] = nx; n[1] = ny; n[2] = nz;
}


mesh::mesh(int vi[], float nx, float ny, float nz, float r, float g, float b)
{
	v[0] = vi[0]; v[1] = vi[1]; v[2] = vi[2]; v[3] = vi[3];
	c[0] = r; c[1] = g; c[2] = b;
	n[0] = nx; n[1] = ny; n[2] = nz;
}

Model3D::Model3D()
{
	_empty = true;
}

Model3D::Model3D(const char * modelfile)
{
	_empty = true;
	FILE * fp = fopen(modelfile, "r");
	if (NULL != fp)
	{
		int n_v, n_m, n;
		n = fscanf(fp, "%d\n%d\n", &n_v, &n_m);/* 读取顶点数与面片数 */
		if (n < 2)
		{
			fclose(fp);
			return;
		}
		/* 读取顶点 */
		for (int i = 0; i < n_v; i++)
		{
			float x, y, z, nx, ny, nz;
			n = fscanf(fp, "%f %f %f %f %f %f\n", &x, &y, &z, &nx, &ny, &nz);
			if (n < 6)
			{
				fclose(fp);
				return;
			}
			_v.push_back(vertex(x, y, z, nx, ny, nz));
		}
		/* 读取面片 */
		for (int i = 0; i < n_m; i++)
		{
			int vi[4];
			float nx, ny, nz, r, g, b;
			n = fscanf(fp, "4 %d %d %d %d %f %f %f %f %f %f\n", vi, vi + 1, vi + 2, vi + 3, &nx, &ny, &nz, &r, &g, &b);
			if (n < 10)
			{
				fclose(fp);
				return;
			}
			_m.push_back(mesh(vi, nx, ny, nz, r, g, b));
		}
		fclose(fp);
		_empty = false;
	}
}

bool Model3D::isEmpty(void)
{
	return _empty;
}

const std::vector<vertex> Model3D::getVertexList(void)
{
	return _v;
}

const std::vector<mesh> Model3D::getMeshList(void)
{
	return _m;
}

Model3D::~Model3D()
{
}