#include "tLightingDemo.h"
#include "tProgramManager.h"
#include "tMeshReader.h"
#include "Utility.h"
#include "tTextureManager.h"
#include "tCamera.h"
#include "tGraphicsController.h"
#include "tInputController.h"

tLight::tLight(tVecf color, tVecf pos) : m_color(color), m_position(pos)
{
}

void tLightingDemo::InitDemo(int maxFps, int minFps)
{
	// max and min fps
	tDemo::InitDemo(maxFps, minFps);

	// load the program
	tProgramManager* pProgramManager = tProgramManager::GetProgramManager();
	int programID = pProgramManager->CreateProgram(ShadString("cubeV.vert"), ShadString("cubeF.frag"));
	string programName = "cubeProg";
	pProgramManager->AddProgram(programName, programID);

	// load geometry
	// batch 1 (and the only batch)
	int vaoId = -1; // for storing the vao
	vector<tGeometry*> tempGeo; // for storing geometry
	tMeshReader mr(ResString("cube3.msh"));

	tVecf axis = tCubeMesh::RandomVector(1.0f, 2.0f);
	axis.normalize();
	// tCubeMesh* pCube = new tCubeMesh(tCubeMesh::RandomVector(1.0f, 3.0f), RandomNum(0.005f, 1.0f), axis, true, vaoId, mr);
	tTexturedCube* pCube = new tTexturedCube(tVecf(-0.5f, 0.5f, -2.0f), RandomNum(0.001f, 0.009f), axis, true, vaoId, mr);
	// tMesh* tempMesh = dynamic_cast<tMesh*>(pCube);
	// mr.LoadMesh(*tempMesh);

	// append it
	tempGeo.push_back(dynamic_cast<tMesh*>(pCube));
	m_dynamicObj.push_back(pCube); // also to be updated

	// now map it
	m_geoContainer[vaoId] = tempGeo;

	// vao to program
	m_vaoProgram[vaoId] = programName;

	// load textures
	tTextureManager* pTexManager = tTextureManager::GetTextureManager();
	pTexManager->AddTexture("CubeTexture", ResString("cube.tga"), "tga");

	//set the texture uniform once and for all, future demos using more than one
	// texture target will want to move this in SetUniforms
	glActiveTexture(0);
	GLint texLoc = glGetUniformLocation(programID, "tex");
	glProgramUniform1i(programID, texLoc, 0); // set the sampler to active texture 0

	// lights
	tLight light;
	
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
	// camera matrix
	GLint camLoc = glGetUniformLocation(prog, "camera");
	glProgramUniformMatrix4fv(prog, camLoc, 1, GL_FALSE, glm::value_ptr(m_Camera->m_transformationMat));
	// light uniforms
	GLint lightLoc = glGetUniformLocation(prog, "lightPos");
	glProgramUniform4fv(prog, lightLoc, 1, glm::value_ptr(glm::vec4(light.m_position.x, light.m_position.y, light.m_position.z, 1.0f)));
	GLint lightColorLoc = glGetUniformLocation(prog, "lightColor");
	glProgramUniform3fv(prog, lightColorLoc, 1, glm::value_ptr(glm::vec3(light.m_color.x, light.m_color.y, light.m_color.z)));
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

void tLightingDemo::EndDemo()
{
	delete m_Camera;
	// call base end demo
	tDemo::EndDemo();
}

tTexturedCube::tTexturedCube(tVecf distance, double rot, tVecf axis, bool batch, int& vaoId, tMeshReader& mr) : 
	tCubeMesh(distance, rot, axis, batch, vaoId, mr)
{
	// set the vao and buffers
	glBindVertexArray(vaoId);
	
	Init();
	
	glBindVertexArray(0); // all done with init, unbind
}

void tTexturedCube::Init()
{
	// texture coord array
	glEnableVertexAttribArray(1); // 1 will be textures
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(tVert), (GLvoid*)offsetof(tVert,m_uvData)); // textures

	// normal array
	glEnableVertexAttribArray(2); // 2 will be normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(2, 4, GL_INT_2_10_10_10_REV, GL_TRUE, sizeof(tVert), (GLvoid*)offsetof(tVert,m_normal));
}

void tTexturedCube::Draw(int prog)
{
	// 
	glEnable(GL_TEXTURE_2D);
	tTextureManager* pTexManager = tTextureManager::GetTextureManager();

	GLint texture = pTexManager->GetTexture("CubeTexture");
	// bind texture and draw
	glBindTexture(GL_TEXTURE_2D, texture);

	// draw six indices at a time
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(0));

	// cycle through faces
	float colors[4];
	GLint colorLoc = glGetUniformLocation(prog, "vertexColor");
	for (int i = 0; i < 6; i++)
	{
		// set the color uniform according to the face index
		m_colors[i].GetData(colors);
		glUniform4fv(colorLoc, 1, colors);
		// draw six indices at a time
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(sizeof(tTriangle) * 2 * i));
	}
}