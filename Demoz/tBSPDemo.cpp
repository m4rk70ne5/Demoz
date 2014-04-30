#include "tBSPDemo.h"
#include "tGraphicsController.h"
#include "tCamera.h"
#include "tProgramManager.h"
#include "tInputController.h"
#include "tTextureManager.h"
#include <vector>
#include <bitset>

using namespace std;

void* tBSPFile::ReadLump(eLumps lump)
{
    //this makes it easier for us to read in bsp data
    //the file is split up into three parts:  a header, a lump directory, and the actual lumps themselves
    //the lump directory contains the size of the lump and offset location in the file where it is stored
    int size = m_header.direntry[lump].size; //the size of the lump
    int offset = m_header.direntry[lump].offset; // the location in the file

    void* new_structure = NULL; //we will return this, depending on what type of lump we're reading in

    m_bspFile.seekg(offset, ios::beg); //go the file location specified by the offset
    switch (lump)
    {
        case kEntities:
        {
            new_structure = (void*)(new char[size]); //entities are just composed of characters
        }
        break;
        case kTextures:
        {
            m_numTextures = size / sizeof(BSP_TEXTURE); //gives us the number of texture structures
            new_structure = (void*)(new BSP_TEXTURE[m_numTextures]); //create the array of bsp_textures
        }
        break;
        case kVertices:
        {
            m_numVertices = size / sizeof(BSP_VERTEX); //same principle as above from now on
            new_structure = (void*)(new BSP_VERTEX[m_numVertices]);
        }
        break;
        case kFaces:
        {
            m_numFaces = size / sizeof(BSP_FACE);
            new_structure = (void*)(new BSP_FACE[m_numFaces]);
        }
        break;
        case kVertexIndices:
        {
            m_numIndices = size / sizeof(int);
            new_structure = (void*)(new int[m_numIndices]);
        }
        break;
        case kPlanes:
        {
            m_numPlanes = size / sizeof(BSP_PLANE);
            new_structure = (void*)(new BSP_PLANE[m_numPlanes]);
        }
        break;
        case kNodes:
        {
            m_numNodes = size / sizeof(BSP_NODE);
            new_structure = (void*)(new BSP_NODE[m_numNodes]);
        }
        break;
        case kLeafs:
        {
            m_numLeafs = size / sizeof(BSP_LEAF);
            new_structure = (void*)(new BSP_LEAF[m_numLeafs]);
        }
        break;
        case kLeafFaces:
        {
            m_numLeafFaces = size / sizeof(int); //there's only one field in the leafface structure, and that's an index (which is of type int)
            new_structure = (void*)(new int[m_numLeafFaces]);
        }
        break;
    }
    m_bspFile.read((char*)new_structure, size); //now, read the data into the struct

    return new_structure;  //return the struct
}

tBSPFile::tBSPFile(string filename)
{
	m_bspFile.open(filename, ifstream::binary); // open file

	// read in data
	m_bspFile.read((char*)&m_header, sizeof(BSP_HEADER));
	
	// read lumps
	m_entString = (char*)ReadLump(kEntities);
	m_textures = (BSP_TEXTURE*)ReadLump(kTextures);
	m_vertices = (BSP_VERTEX*)ReadLump(kVertices);
	m_faces = (BSP_FACE*)ReadLump(kFaces);
	m_vertexIndex = (int*)ReadLump(kVertexIndices);

	// visData is a special case
	m_bspFile.seekg(m_header.direntry[kVisData].offset, ios::beg);
    m_bspFile.read((char*)&m_visData, sizeof(int)*2); //only read in the first two data fields of visibility data
                                                     //this is because we don't know how much space to allocate for
                                                     //the third field until we have the values from the firt two data fields
    m_numVectors = m_visData.n_vecs * m_visData.sz_vecs;
    m_visData.vecs = new ubyte[m_numVectors];
    m_bspFile.read((char*)m_visData.vecs, sizeof(ubyte)*m_numVectors);

	// resume lumps
	m_planes = (BSP_PLANE*)ReadLump(kPlanes);
    m_nodes = (BSP_NODE*)ReadLump(kNodes);
    m_leaves = (BSP_LEAF*)ReadLump(kLeafs);
    m_leafFaces = (int*)ReadLump(kLeafFaces);

	m_bspFile.close();
}

tBSPTree::tBSPTree(tBSPFile* bspFile, bool newBatch, int& vaoId, tCamera* player) : tGeometry(newBatch, vaoId), m_file(bspFile), m_player(player)
{
	// swizzle data
	// Go through all of the vertices that need to be read and swap axis's
	for(int i = 0; i < bspFile->m_numVertices; i++)
	{
		// Swap the y and z values, and negate the new z so Y is up.
		float temp = bspFile->m_vertices[i].position[1];
		bspFile->m_vertices[i].position[1] = bspFile->m_vertices[i].position[2];
		bspFile->m_vertices[i].position[2] = -temp;
		// texture
		// bspFile->m_vertices[i].tex_color_coord[1] = -bspFile->m_vertices[i].tex_color_coord[1];
	}
	// Go through every plane and convert it's normal to the Y-axis being up
	for(int i = 0; i < bspFile->m_numPlanes; i++)
	{
		float temp = bspFile->m_planes[i].normal[1];
		bspFile->m_planes[i].normal[1] = bspFile->m_planes[i].normal[2];
		bspFile->m_planes[i].normal[2] = -temp;
	}
	// set the vao and buffers
	glBindVertexArray(vaoId);
	
	Init();
	
	glBindVertexArray(0); // all done with init, unbind

	// entity string
    string player_start = GrabEntity("info_player_start");
    player_start = ExtractValue(player_start, "origin"); //extract value from the origin key
    player_start.erase(0,1);  //erase first quote
    char* token1 = NULL;
	char* next_token = NULL;
    int p = 0;
    float position[3];
    token1 = strtok_s((char*)player_start.c_str(), " \"", &next_token);
    while (token1 != NULL)
    {
        position[p++] = atof(token1); //convert the string of values to actual floating-point coordinates
        token1 = strtok_s(NULL, " \"", &next_token);
    }
	m_startPosition = glm::vec3(position[0], position[1], position[2]);
	// swizzle the start position
	float temp = m_startPosition[1];
	m_startPosition[1] = m_startPosition[2];
	m_startPosition[2] = -temp;
	player->m_position = m_startPosition;
}

void tBSPTree::Init()
{
	// vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_file->m_numVertices * sizeof(BSP_VERTEX), m_file->m_vertices, GL_STATIC_DRAW);

	// index buffer
	glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_file->m_numIndices * sizeof(int), m_file->m_vertexIndex, GL_STATIC_DRAW);

	// vertex attribute array
	glEnableVertexAttribArray(0); // 0 will always be position
	glEnableVertexAttribArray(1); // 1 will be textures
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BSP_VERTEX), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BSP_VERTEX), (GLvoid*)(offsetof(BSP_VERTEX, tex_color_coord))); // textures
}

void tBSPTree::Draw(int prog)
{
	// find the current leaf the camera is in
	// get its cluster
	glm::vec3 pos = m_player->GetPosition();
	int leafIndex = FindLeaf(pos);
	int currentCluster = m_file->m_leaves[leafIndex].cluster;

	vector<int> visibleFaces; // vector of visible faces
	map<int, bool> setFaces;
	// bitset to avoid duplicate face indices
	for (int i = 0; i < m_file->m_numLeafs; i++)
    {
        //loop through all the leaves
        //each time check to see if the leaf is visible from the cluster we're in
        if(ClusterVisible(m_file->m_leaves[i].cluster, currentCluster))
        {
			// the cluster containing leaf i is visible
			// so, leaf i is visible
			// add all the faces to the visible faces
			for (int j = 0; j < m_file->m_leaves[i].n_leaffaces; j++)
			{
				int leafFaceIndex = m_file->m_leaves[i].leafface + j;
				int faceIndex = m_file->m_leafFaces[leafFaceIndex];
				if (!setFaces.count(faceIndex))
				{
					// the face isn't added yet
					setFaces[faceIndex] = true;
					visibleFaces.push_back(faceIndex);
				}
			}
		}
	}

	// cycle through all the faces and draw each one
	glEnable(GL_TEXTURE_2D);
	tTextureManager* pTexManager = tTextureManager::GetTextureManager();
	for (auto iFace = visibleFaces.begin(); iFace != visibleFaces.end(); iFace++)
	{
		BSP_FACE* pFace = &m_file->m_faces[*iFace];
		GLint texture = pTexManager->GetTexture(m_file->m_textures[pFace->texture].name);
		// bind texture and draw
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElementsBaseVertex(GL_TRIANGLES, pFace->n_meshvert, GL_UNSIGNED_INT, 
			(GLvoid*)(sizeof(int) * pFace->meshvert), pFace->vertex);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//tTextureManager* pTexManager = tTextureManager::GetTextureManager();
	//for (int i = 0; i < m_file->m_numFaces; ++i)
	//{
	//	BSP_FACE* pFace = &m_file->m_faces[i];
	//	GLint texture = pTexManager->GetTexture(m_file->m_textures[pFace->texture].name);
	//	// bind texture and draw
	//	glBindTexture(GL_TEXTURE_2D, texture);
	//	glDrawElementsBaseVertex(GL_TRIANGLES, pFace->n_meshvert, GL_UNSIGNED_INT, 
	//		(GLvoid*)(sizeof(int) * pFace->meshvert), pFace->vertex);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
}

int tBSPTree::FindLeaf(glm::vec3 pos)
{
	// finds the leaf where pos is contained in
	int leafIndex = 0;

	// negative leaf indices are actual leaves
	while (leafIndex >= 0)
	{
		// check to see if pos is in front or behind the plane
		BSP_NODE* pParent = &m_file->m_nodes[leafIndex];
		BSP_PLANE plane = m_file->m_planes[pParent->plane];
		glm::vec3 planeNormal(plane.normal[0], plane.normal[1], plane.normal[2]);
		float distance = glm::dot(planeNormal, pos) - plane.dist;
	
		// in front
		if (distance >= 0)
		{
			leafIndex = pParent->children[0];
		}
		else
		{
			// back
			leafIndex = pParent->children[1];
		}
	}
	return ~leafIndex;
}

bool tBSPTree::ClusterVisible(int x, int y)
{
	// is cluster x visible from cluster y?
	BSP_VISDATA& vis = m_file->m_visData;
	ubyte vec = vis.vecs[(x * vis.sz_vecs) + (y / 8)];
	return ((1 << y % 8) & vec);
}

string tBSPTree::GrabEntity(string classname)
{
    //this searches the list of entities for a certain entity
    int classname_position, position_of_brace, end_position;
    int temp_pos = 0;
    string entity_string(m_file->m_entString);
    string entity;
    classname_position = entity_string.find(classname);
    while ((temp_pos < classname_position) && (temp_pos != (int)string::npos))
    {
        temp_pos = entity_string.find('{',temp_pos);
        if ((temp_pos < classname_position) && (temp_pos != (int)string::npos))
        {
            position_of_brace = temp_pos;
            temp_pos++;
        }
    }
    end_position = entity_string.find('}',position_of_brace);
    entity.assign(entity_string, position_of_brace, (end_position - position_of_brace) + 1);
    return entity;
}

string tBSPTree::ExtractValue(string entity, string key)
{
    //this is to help us actually get the value(s) specified by a certain key in the entity
    string key_string;
    int temp_pos = -1;
    int last_quote;
    temp_pos = entity.find(key);
    temp_pos += key.size() + 2; //+2 to skip over last quote and space
    last_quote = entity.find('"',temp_pos + 1);
    key_string.assign(entity, temp_pos, (last_quote - temp_pos) + 1);
    return key_string;
}

tBSPTree::~tBSPTree()
{
	delete m_file;
}

void tBSPDemo::InitDemo(int min, int max)
{
	tDemo::InitDemo(50, 10);

	// add the camera
	m_Camera = new tCamera(tVecf(0.0f, 0.0f, 0.0f), tVecf(0.0f, 1.0f, 0.0f), tVecf(0.0f, 0.0f, -1.0f), 0.05f);
	m_dynamicObj.push_back(m_Camera);
	m_tGC->SetCam(m_Camera); // set the graphic controller's camera matrix
	
	// geometry
	vector<tGeometry*> tempGeo; // for storing geometry
	int vao;
	tBSPFile* bsp = new tBSPFile(ResString("test_s.bsp"));
	// map programs and vao to batches
	tempGeo.push_back(new tBSPTree(bsp, true, vao, m_Camera));
	m_geoContainer[vao] = tempGeo;
	// vao to program
	// load the program
	tProgramManager* pProgramManager = tProgramManager::GetProgramManager();
	int programID = pProgramManager->CreateProgram(ShadString("bspView.vert"), ShadString("bspView.frag"));
	string programName = "bspProg";
	pProgramManager->AddProgram(programName, programID);
	m_vaoProgram[vao] = programName;
	
	// textures of the bsp file
	tTextureManager* pTexManager = tTextureManager::GetTextureManager();
	for (int i = 0; i < bsp->m_numTextures; i++)
	{
		pTexManager->AddTexture(bsp->m_textures[i].name, ResString(bsp->m_textures[i].name) + ".tga", "tga");
	}

	//set the texture uniform once and for all, future demos using more than one
	// texture target will want to move this in SetUniforms
	glActiveTexture(0);
	GLint texLoc = glGetUniformLocation(programID, "tex");
	glProgramUniform1i(programID, texLoc, 0); // set the sampler to active texture 0

	GLint projLoc = glGetUniformLocation(programID, "proj"); // projection matrix
	tGraphicsController* pgc = tGraphicsController::GetGraphicsController();
	glm::mat4 proj = pgc->GetProjectionMatrix();
	glProgramUniformMatrix4fv(programID, projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	GLint camLoc = glGetUniformLocation(programID, "camera");
	glProgramUniformMatrix4fv(programID, camLoc, 1, GL_FALSE, glm::value_ptr(m_Camera->m_transformationMat));

	// attach objects to input controller
	m_IC->AttachMObject(m_Camera);
}

void tBSPDemo::EndDemo()
{
	// call base end demo
	tDemo::EndDemo();
}