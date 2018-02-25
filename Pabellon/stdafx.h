// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Additional headers our program requires.

#include <cassert>

// GLEW
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")

// GLM
#pragma warning(push) 
#pragma warning(disable : 4201)
#define GLM_FORCE_RADIANS    // enforce all angles to be in radians, not in degrees
#include <glm/glm.hpp>       // the most common GLM headers
#include <glm/gtc/matrix_transform.hpp>    // matrix transformations: glm::perspective(), etc.
#include <glm/gtc/type_ptr.hpp>            // glm::value_ptr() to pass matrices to shaders, etc.
#pragma warning(pop)

// WGL (Windows OpenGL)
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// SOIL
#include <soil/src/SOIL.h>
#pragma comment(lib, "SOIL.lib")

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment(lib, "assimp-vc120-mt.lib")


// Common OpenGL and related functionality.
#include "../../CommonLibOgl/CommonLibOgl/CommonLibOgl.h"


namespace PabellonApp
{
// Macro to work with OpenGL buffers.
#define BUFFER_OFFSET(offset) ((void *)(offset))
}


#include "TextureInfo.h"
#include "MaterialInfo.h"
#include "Mesh.h"
#include "Model.h"

#include "PabellonScene.h"
#include "MainWindow.h"