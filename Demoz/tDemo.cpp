#include "tDemo.h"
#include "resource.h"
#include "tDemoList.h"
#include "tGraphicsController.h"
#include "tInputController.h"
#include "tMeshReader.h"
#include "tProgramManager.h"
#include "tCamera.h"
#include "tGameClock.h"
#include "tTextureManager.h"
#include "Utility.h"
#include <glm/gtc/matrix_transform.hpp>

tGraphicsController* tDemo::m_tGC = NULL;
tInputController* tDemo::m_IC = NULL;
tTextureManager* tDemo::m_tTM = NULL;

void tDemo::Start()
{
	// initializations common to all demos
	InitDemo(); // creates objects, loads geometry, intializes subsystem (SDL)

	m_gameClock->Loop(); // main demo runs here

	// now quitting, call destructors
	EndDemo();
}

void tDemo::Stop() 
{ 
	m_gameClock->Stop();
};

void tDemo::InitDemo(int maxFps, int minFps)
{
	// construct necessary objects common to all Demos
	tGraphicsController* pGC = tGraphicsController::GetGraphicsController();
	pGC->Initialize(m_title.c_str(), m_width, m_height);
	// optimization: set graphics controller to member
	m_tGC = pGC;
	// textures
	m_tTM = tTextureManager::GetTextureManager();
	// now, set the default projection matrix
	// (for a different one, set it in the derived InitDemo())
	// 10 in each direction, and 1000 to the far plane
	// glm::mat4 proj = pGC->GetProjectionMatrix();
	glm::mat4 proj;
	float f = 100.0f;
	float n = 1.0f;
	float t = glm::tan(45.0f / 2) * n;
	float b = -t;
	float r = t * (4.0f / 3.0f);
	float l = -r;
	proj[0][0] = n / r;
	proj[1][1] = n / t;
	proj[2][2] = -(f + n) / (f - n);
	proj[3][2] = -(2 * n * f ) / (f - n);
	proj[2][3] = -1.0f;
	proj[3][3] = 0.0f;
	// pGC->SetProjectionMatrix(glm::perspective(45.0f, (float)(4.0f / 3.0f), 0.1f, 100.0f));
	pGC->SetProjectionMatrix(proj);
	// glDepthRange(-1, 1); // set the depth range
	// create input controller
	m_IC = tInputController::GetInputController();
	m_IC->SetMouseBounds(m_width, m_height);
	
	// create the game clock
	m_gameClock = new tGameClock(maxFps, minFps); // max fps, min fps
	m_gameClock->m_dynamicObjects = &m_dynamicObj;
	m_gameClock->m_geoContainer = &m_geoContainer;
	m_gameClock->m_vaoProgram = &m_vaoProgram;
	m_gameClock->m_quitStruct.m_argument = dynamic_cast<void*>(m_parentListBox);
	m_gameClock->m_quitStruct.m_StopFunction = StopDemoThread;
	m_gameClock->m_IC = m_IC;
	m_gameClock->m_GC = m_tGC;
}

void tDemo::EndDemo()
{
	// get rid of objects
	delete m_gameClock;
	delete m_tGC;
	delete m_IC;
	delete m_tTM;

	SDL_Quit(); // when done
}

unsigned __stdcall tDemo::StopDemoThread(void* list)
{
	tDemoList* pDemoList = static_cast<tDemoList*>(list);
	pDemoList->SendStopSignal(pDemoList->GetCurrentEntry());
	return 0;
}

string tDemo::ResString(char* c)
{
	string respath(m_settings.resourcePath.begin(), m_settings.resourcePath.end());
	respath = respath.append("\\");
	return respath.append(c);
}

string tDemo::ShadString(char* c)
{
	string respath(m_settings.shaderPath.begin(), m_settings.shaderPath.end());
	respath = respath.append("\\");
	return respath.append(c);
}

tDemo::~tDemo()
{
	// destruct all necessary objects
	// cycle through geometry container, destroying each object
	for (auto i = m_geoContainer.begin(); i != m_geoContainer.end(); i++)
	{
		for (auto j = i->second.begin(); j != i->second.begin();)
		{
			auto ptr = *j;
			j = i->second.erase(j);
			delete ptr;
		}
	}
}

void tSpinningBoxDemo::InitDemo(int maxFps, int minFps)
{
	// call original, with max fps of 50 and min fps of 25
	tDemo::InitDemo(50, 25);

	// load the program
	tProgramManager* pProgramManager = tProgramManager::GetProgramManager();
	int programID = pProgramManager->CreateProgram(ShadString("cubeV.vert"), ShadString("cubeF.frag"));
	string programName = "cubeProg";
	pProgramManager->AddProgram(programName, programID);

	// now, load geometry
	// batch 1 (and the only batch)
	int vaoId = -1; // for storing the vao
	vector<tGeometry*> tempGeo; // for storing geometry
	tMeshReader mr(ResString("cube.msh"));
	
	tVecf axis = tCubeMesh::RandomVector(1.0f, 2.0f);
	axis.normalize();
	// tCubeMesh* pCube = new tCubeMesh(tCubeMesh::RandomVector(1.0f, 3.0f), RandomNum(0.005f, 1.0f), axis, true, vaoId, mr);
	tCubeMesh* pCube = new tCubeMesh(tVecf(-0.5f, 0.5f, -2.0f), RandomNum(0.01f, 0.09f), tVecf(1.0f, 0.0f, 1.0f), true, vaoId, mr);
	// tMesh* tempMesh = dynamic_cast<tMesh*>(pCube);
	// mr.LoadMesh(*tempMesh);

	// append it
	tempGeo.push_back(dynamic_cast<tMesh*>(pCube));
	m_dynamicObj.push_back(pCube); // also to be updated

	// now make copies of it, each with a different distance
	for (int i = 0; i < 10; i++)
	{
		// make copies of the mesh with default copy constructor
		// default copy constructor doesn't mess with pointer values
		// which is fine in this case, since the vertex and index data
		// is the same.  in fact, it perserves memory
		tCubeMesh* pCm = new tCubeMesh(*pCube);
		// manually set the cube mesh's members
		pCm->SetDistance(tCubeMesh::RandomVector((float)(i + 2), (float)(i + 4)));
		
		// set the axis
		tVecf axis = tCubeMesh::RandomVector(1.0f, 2.0f);
		axis.normalize();
		glm::vec3 axisVec(axis.x, axis.y, axis.z);
		pCm->m_offset = glm::angleAxis(RandomNum(0.005f, 1.0f), axisVec);
		pCm->RandomFaceColors();
		// to random distance, random speed, and random rotation axis
		m_dynamicObj.push_back(pCm); // also to be updated
		tempGeo.push_back(pCm);
	}
	// now map it
	m_geoContainer[vaoId] = tempGeo;

	// vao to program
	m_vaoProgram[vaoId] = programName;

	// now intialize the camera
	m_Camera = new tCamera(tVecf(0.0f, 0.0f, 0.0f), tVecf(0.0f, 1.0f, 0.0f), tVecf(0.0f, 0.0f, -1.0f), 0.05f);
	m_dynamicObj.push_back(m_Camera);
	m_tGC->SetCam(m_Camera); // set the graphic controller's camera matrix

	// initialize the static uniforms for the program of batch 1
	tProgramManager* pPM = tProgramManager::GetProgramManager();
	GLint prog = pPM->GetProgram(programName);
	GLint projLoc = glGetUniformLocation(prog, "proj"); // projection matrix
	// glm::mat4 proj = glm::perspective(45.0f, (float)(4.0f / 3.0f), 0.1f, 100.0f);
	tGraphicsController* pgc = tGraphicsController::GetGraphicsController();
	glm::mat4 proj = pgc->GetProjectionMatrix();
	glProgramUniformMatrix4fv(prog, projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	GLint camLoc = glGetUniformLocation(prog, "camera");
	glProgramUniformMatrix4fv(prog, camLoc, 1, GL_FALSE, glm::value_ptr(m_Camera->m_transformationMat));
	// camera matrix
	// world matrix will be grabbed from each individual tCubeMesh as it is being drawn
	// end batch 1

	// attach objects to input controller
	vector<int> cameraKeys;
	cameraKeys.push_back(SDLK_UP);
	cameraKeys.push_back(SDLK_DOWN);
	cameraKeys.push_back(SDLK_LEFT);
	cameraKeys.push_back(SDLK_RIGHT);
	m_IC->AttachKObject(m_Camera, cameraKeys);
	m_IC->AttachMObject(m_Camera);
}

void tSpinningBoxDemo::EndDemo()
{
	delete m_Camera;
	// call base end demo
	tDemo::EndDemo();
}