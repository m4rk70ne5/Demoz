#ifndef TLIGHTING_DEMO_H
#define TLIGHTING_DEMO_H

#include "tDemo.h"
#include "tMesh.h"
#include "tVector.h"

class tLight
{
public:
	tLight(tVecf = tVecf(1.0, 1.0, 1.0), tVecf = tVecf(0.0, 0.0, 2.0));
	tVecf m_color;
	tVecf m_position;
};

class tLightingDemo : public tDemo
{
public:
	tLightingDemo() : tDemo() {};
	virtual void InitDemo(int = 50, int = 25);
	virtual void EndDemo();
	tCamera* m_Camera;
};

class tTexturedCube : public tCubeMesh
{
protected:
	virtual void Init();
public:
	tTexturedCube(tVecf, double, tVecf, bool, int&, tMeshReader&);
	virtual void Draw(int);
};

#endif