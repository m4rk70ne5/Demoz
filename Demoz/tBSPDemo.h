#ifndef TBSPDEMO_H
#define TBSPDEMO_H

#include "tMesh.h"
#include "tDemo.h"
#include <string>
#include <fstream>

typedef unsigned char ubyte;

struct BSP_DIRENTRY
{
	int offset;
	int size;
};

struct BSP_HEADER
{
	char magic[4];
	int version;
	BSP_DIRENTRY direntry[17];
};

struct BSP_TEXTURE
{
    char name[64];
    int flags;
    int contents;
};

enum eLumps
{
    kEntities = 0,
    kTextures,
    kPlanes,
    kNodes,
    kLeafs,
    kLeafFaces,
    kLeafBrushes,
    kModels,
    kBrushes,
    kBrushSides,
    kVertices,
    kVertexIndices,
    kShaders,
    kFaces,
    kLightmaps,
    kLightVolumes,
    kVisData,
};

struct BSP_VERTEX
{
    float position[3];
    float tex_color_coord[2];
    float tex_light_coord[2];
    float normal[3];
    ubyte color[4];
};

struct BSP_FACE
{
    int texture; //texture index
    int effect;
    int type; //1 = polygon, 2 = patch, 3 = mesh, 4 = billboard
    int vertex; //the index of first one
    int n_vertices; //number of vertices
    int meshvert; //index of first mesh vertex
    int n_meshvert; //number of mesh vertices (face indices)
    int lm_index;  //lightmap data
    int lm_start[2];
    int lm_size[2];
    float lm_origin[3];
    float lm_vecs[2][3];
    float normal[3]; //surface normal
    int size[2]; //patch dimensions
};

struct BSP_VISDATA
{
     int n_vecs; //number of vectors
     int sz_vecs; //size of each vector, in bytes
     ubyte* vecs; //visibility data
};

struct BSP_PLANE
{
    float normal[3]; //plane normal
    float dist; //distance from origin to normal
};

struct BSP_NODE
{
    int plane;	//Plane index.
    int children[2];	//Children indices. Negative numbers are leaf indices: -(leaf+1).
    int mins[3];	//Integer bounding box min coord.
    int maxs[3];	//Integer bounding box max coord.
};

struct BSP_LEAF
{
    unsigned int cluster;    //Visdata cluster index.
    int area;	//Areaportal area.
    int mins[3];	//Integer bounding box min coord.
    int maxs[3];	//Integer bounding box max coord.
    int leafface;	//First leafface for leaf.
    int n_leaffaces;	//Number of leaffaces for leaf.
    int leafbrush;	//First leafbrush for leaf.
    int n_leafbrushes;	   //Number of leafbrushes for leaf.
};

class tBSPFile
{
	friend class tBSPTree;
	friend class tBSPDemo;
protected:
	int m_numVertices, m_numFaces, m_numIndices, m_numTextures;
    int m_numVectors;
    int m_numPlanes, m_numNodes, m_numLeafs, m_numLeafFaces;
	BSP_HEADER m_header;
	BSP_TEXTURE* m_textures;
    BSP_VERTEX* m_vertices;
    BSP_FACE* m_faces;
	int* m_vertexIndex;
    BSP_VISDATA m_visData;
    BSP_PLANE* m_planes;
    BSP_NODE* m_nodes;
    BSP_LEAF* m_leaves;
    int* m_leafFaces;
	char* m_entString;
	ifstream m_bspFile;
	void* ReadLump(eLumps);
public:
	tBSPFile(std::string);
};

class tBSPTree : public tGeometry
{
protected:
	bool ClusterVisible(int x, int y);
	int FindLeaf(glm::vec3);
	tBSPFile* m_file;
	tCamera* m_player;
	virtual void Init();
	string GrabEntity(string);
	string ExtractValue(string, string);
public:
	tBSPTree(tBSPFile*, bool, int&, tCamera*);
	~tBSPTree();
	virtual void Draw(int);
	glm::vec3 m_startPosition;
};

class tBSPDemo : public tDemo
{
public:
	tBSPDemo() : tDemo() {};
	virtual void InitDemo(int = 0, int = 0);
	virtual void EndDemo();
	tCamera* m_Camera;
};

#endif