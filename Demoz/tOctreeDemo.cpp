#include "tOctreeDemo.h"
#include "tOctree.h"
#include "tProgramManager.h"
#include "tMesh.h"
#include "tMeshReader.h"
#include "Utility.h"
#include "tCamera.h"
#include "tGraphicsController.h"
#include "tInputController.h"
#include "tModeToggler.h"

void tOctreeDemo::InitDemo(int maxFPS, int minFPS)
{
	// call original, with max fps of 50 and min fps of 25
	tDemo::InitDemo(50, 25);

	// load the programs
	// program 1 -- cubes
	tProgramManager* pProgramManager = tProgramManager::GetProgramManager();
	int programID = pProgramManager->CreateProgram(ShadString("cubeV.vert"), ShadString("cubeF.frag"));
	string cubeProgramName = "cubeProg";
	pProgramManager->AddProgram(cubeProgramName, programID);
	
	// program 2 -- wireframes
	programID = pProgramManager->CreateProgram(ShadString("wireV.vert"), ShadString("wireF.frag"));
	string wireProgramName = "wireProg";
	pProgramManager->AddProgram(wireProgramName, programID);

	// program 3 -- bsps


	// now, load geometry
	// batch 1 and 2 (all the cubes and their bounding boxes)
	int cubeVaoId = -1; // for storing the vao
	int bbVaoId = -1;
	vector<tGeometry*> tempCubeGeo; // for storing geometry
	vector<tMesh*> meshesForOctree; // for octree
	vector<tGeometry*> boundingBoxGeo;
	vector<tBoundingBox*> boundingBoxVec;
	tMeshReader mr(ResString("cube2.msh")); // load mesh
	// create cubes based off of it
	bool newVao = true;
	for (int i = 0; i < 20; i++)
	{
		// create random axis to spin
		tVecf axis = tCubeMesh::RandomVector(1.0f, 2.0f);
		axis.normalize();
		// create cube
		tVecf dist = tCubeMesh::RandomVector(1.0f, 30.0f, i);
		tCubeMesh* pCube = new tCubeMesh(dist, RandomNum(0.01f, 0.09f, i), axis, newVao, cubeVaoId, mr);
										// distance, rotation speed, axis, new batch?, vao, mesh
		pCube->CreateBoundingBox(bbVaoId, newVao, wireProgramName);
		pCube->m_boundingBox.SetOffset(i);
		pCube->m_boundingBox.SetDistance(glm::vec3(dist.x, dist.y, dist.z));
		// append it
		meshesForOctree.push_back(pCube);
		tempCubeGeo.push_back(dynamic_cast<tMesh*>(pCube));
		m_dynamicObj.push_back(pCube); // also to be updated
		boundingBoxGeo.push_back(&pCube->m_boundingBox);
		boundingBoxVec.push_back(&pCube->m_boundingBox);
		if (newVao)
		{
			newVao = false;
		}
	}
	// now map them
	m_geoContainer[cubeVaoId] = tempCubeGeo; // vao id -> geometry batch
	m_vaoProgram[cubeVaoId] = cubeProgramName; // vao id -> default program
	// create bounding box data
	CreateBoundingBoxBuffer(boundingBoxVec, bbVaoId);
	m_geoContainer[bbVaoId] = boundingBoxGeo;
	m_vaoProgram[bbVaoId] = wireProgramName;

	// batch 3 (octree wireframe)
	int octreeId;
	vector<tGeometry*> wireGeo;
	// vertex buffer data
	tOctree* pOctree = new tOctree(meshesForOctree, octreeId);
	pOctree->CreateVertexBuffers(octreeId);
	wireGeo.push_back(pOctree);

	// map
	m_geoContainer[octreeId] = wireGeo;
	m_vaoProgram[octreeId] = wireProgramName;
	// batch 4 (bsp)


	// dynamic objects
	// mode toggler
	tModeToggler* pModeToggler = tModeToggler::GetModeToggler();
	pModeToggler->SetCurrentDemo("OctreeDemo");
	m_dynamicObj.push_back(pModeToggler);
	// now for the camera
	m_Camera = new tCamera(tVecf(0.0f, 0.0f, 0.0f), tVecf(0.0f, 1.0f, 0.0f), tVecf(0.0f, 0.0f, -1.0f), 0.1f, true, 3.0f);
	m_dynamicObj.push_back(m_Camera);
	m_tGC->SetCam(m_Camera); // set the graphic controller's camera
	m_Camera->SetCamPosition(pOctree->GetCenter());

	// initialize global uniforms (put this in a function later?)
	tGraphicsController* pgc = tGraphicsController::GetGraphicsController();
	glm::mat4 proj = pgc->GetProjectionMatrix();
	glGenBuffers(1, &m_globalUBuffers);
	glBindBuffer(GL_UNIFORM_BUFFER, m_globalUBuffers);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_Camera->m_transformationMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	m_Camera->SetUBO(m_globalUBuffers);

	// connect uniform block indices to binding points and initialize the static uniforms for the program of batch 1
	int bindingIndex = 0; // transforms at binding point 0
	int prog = pProgramManager->GetProgram("cubeProg");
	glUniformBlockBinding(prog, glGetUniformBlockIndex(prog, "TransformMat"), bindingIndex);
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, m_globalUBuffers, 0, sizeof(glm::mat4) * 2);
	GLenum e = glGetError();
	// any regular uniforms?
	// end batch 1

	// uniforms for batch 2 and wireProg uniform block
	prog = pProgramManager->GetProgram("wireProg");
	glUniformBlockBinding(prog, glGetUniformBlockIndex(prog, "TransformMat"), bindingIndex);
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, m_globalUBuffers, 0, sizeof(glm::mat4) * 2);
	// color
	glUseProgram(prog);
	GLint colorLoc = glGetUniformLocation(prog, "inColor");
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.7, 0.7, 1.0)));
	
	// any regular uniforms?
	// end batch 2

	// uniforms for batch 3
	// color

	// uniforms for batch 4
	
	// attach objects to input controller
	// the keys are handled in the update automatically
	m_IC->AttachMObject(m_Camera);

	// collidable geometry
	m_Camera->AddCollidable(pOctree);
}

void tOctreeDemo::EndDemo()
{
	delete m_Camera;
	// call base end demo
	tDemo::EndDemo();
}

void tOctreeDemo::CreateBoundingBoxBuffer(vector<tBoundingBox*>& boundingBoxes, int vao)
{
	vector<glm::vec3> rect_points;
	rect_points.reserve(boundingBoxes.size() * 4 * 4);
	for (int numRects = 0; numRects < boundingBoxes.size(); numRects++)
	{
		tBoundingBox& bb = *(boundingBoxes[numRects]);
		// get data
		tRect* pRect = bb.GetFaces();
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				rect_points.push_back(pRect[j].m_points[k]);
			}
		}
		// set the offset
		bb.SetOffset(numRects);
	}
	glBindVertexArray(vao);
	// buffer operations
	// vertex buffer
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, rect_points.size() * sizeof(glm::vec3), static_cast<GLvoid*>(&rect_points[0]), GL_STATIC_DRAW);
	// vertex attribute array
	glEnableVertexAttribArray(0); // 0 will always be position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glBindVertexArray(0);
}