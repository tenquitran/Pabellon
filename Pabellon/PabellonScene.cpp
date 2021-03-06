#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PabellonApp;

//////////////////////////////////////////////////////////////////////////


PabellonScene::PabellonScene()
{
}

PabellonScene::~PabellonScene()
{
}

bool PabellonScene::initialize(GLfloat aspectRatio, const OpenGLInfo& openGlInfo)
{
	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f;

	m_spCamera = std::make_unique<Camera>(aspectRatio, CameraScaleFactor,
		openGlInfo.FieldOfView, openGlInfo.FrustumNear, openGlInfo.FrustumFar);

	// Take the model in focus.
#if 1
	m_spCamera->translateZ(-50.0);
	//m_spCamera->translateZ(-120.0);
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue
	//glClearColor(0.0f, 0.64f, 0.91f, 1.0f);    // light blue

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\pabellon.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\pabellon.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

#if _DEBUG
	DWORD tick0 = GetTickCount();
#endif

	bool res = m_pabellon.initialize(m_spProgram->getProgram(), "data//pavillon_barcelone_v1.2.obj", m_spCamera);

#if _DEBUG
	DWORD ticksElapsed = GetTickCount() - tick0;    // about 34000 [milliseconds]
#endif

	if (!res)
	{
		assert(false); return false;
	}

	return true;
}

void PabellonScene::resize(GLfloat aspectRatio)
{
	m_spCamera->resize(aspectRatio);
}

void PabellonScene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);

	m_pabellon.updateMatrices();
}

void PabellonScene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);

	m_pabellon.updateMatrices();
}

void PabellonScene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);

	m_pabellon.updateMatrices();
}

void PabellonScene::rotateCameraX(GLfloat angleDegrees)
{
	m_spCamera->rotateX(angleDegrees);

	m_pabellon.updateMatrices();
}

void PabellonScene::rotateCameraY(GLfloat angleDegrees)
{
	m_spCamera->rotateY(angleDegrees);

	m_pabellon.updateMatrices();
}

void PabellonScene::rotateCameraZ(GLfloat angleDegrees)
{
	m_spCamera->rotateZ(angleDegrees);

	m_pabellon.updateMatrices();
}

void PabellonScene::rotateCameraXY(GLfloat xAngleDegrees, GLfloat yAngleDegrees)
{
	m_spCamera->rotateXY(xAngleDegrees, yAngleDegrees);

	m_pabellon.updateMatrices();
}

void PabellonScene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);

	m_pabellon.updateMatrices();
}

GLfloat PabellonScene::getCameraScale() const
{
	return m_spCamera->getScale();
}

void PabellonScene::render() const
{
#if 0
	updateViewMatrices();
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(m_spProgram);

	glUseProgram(m_spProgram->getProgram());

	m_pabellon.render();

	glUseProgram(0);
}
