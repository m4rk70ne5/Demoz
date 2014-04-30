#ifndef TOCTREEDEMO_H
#define TOCTREEDEMO_H

#include "tDemo.h"
#include "tMesh.h"

class tOctreeDemo : public tDemo
{
protected:
	void CreateBoundingBoxBuffer(vector<tBoundingBox*>&, int);
public:
	tOctreeDemo() : tDemo() {};
	virtual void InitDemo(int = 0, int = 0);
	virtual void EndDemo();
	tCamera* m_Camera;
};

#endif