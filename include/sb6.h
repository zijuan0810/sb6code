#ifndef __SB6_H__
#define __SB6_H__

#ifdef WIN32
    #pragma once
    #define _CRT_SECURE_NO_WARNINGS 1

    #define WIN32_LEAN_AND_MEAN 1
    #include <Windows.h>

    #ifdef _DEBUG
        #ifdef _WIN64
            #pragma comment (lib, "GLFW_d64.lib")
            #ifndef IN_SB6_LIB
                #pragma comment (lib, "sb6_d64.lib")
            #endif
        #else
            #pragma comment (lib, "GLFW_d32.lib")
            #ifndef IN_SB6_LIB
                #pragma comment (lib, "sb6_d32.lib")
            #endif
        #endif
    #else
        #ifdef _WIN64
            #pragma comment (lib, "GLFW_r64.lib")
            #ifndef IN_SB6_LIB
                #pragma comment (lib, "sb6_r64.lib")
            #endif
        #else
            #pragma comment (lib, "GLFW_r32.lib")
            #ifndef IN_SB6_LIB
                #pragma comment (lib, "sb6_r32.lib")
            #endif
        #endif
    #endif

    #pragma comment (lib, "OpenGL32.lib")
#endif

#include "GL/gl3w.h"

#define GLFW_NO_GLU 1
#define GLFW_INCLUDE_GLCOREARB 1
#include "GL/glfw.h"

#include "sb6ext.h"

#include <stdio.h>
#include <string.h>

#include <string>
#include <iostream>
using namespace std;

#define NS_SB6_BEGIN namespace sb6 {
#define NS_SB6_END }

NS_SB6_BEGIN

class application
{
public:
	application() {}
	virtual ~application() {}

	virtual void run(sb6::application* the_app);

	virtual void init();

	virtual void startup() {}
	virtual void render(double currentTime) {}
	virtual void shutdown() {}
	virtual void onKeyChar(int key, int action) {}
	virtual void onResize(int w, int h)
	{
		info.windowWidth = w;
		info.windowHeight = h;
	}

	static void getMousePosition(int& x, int& y);

	virtual void onKey(int key, int action) {}
	virtual void onMouseButton(int button, int action) {}
	virtual void onMouseMove(int x, int y) {}
	virtual void onMouseWheel(int pos) {}
	virtual void onDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message);

public:
	struct APPINFO
	{
		char title[128];
		int windowWidth;
		int windowHeight;
		int majorVersion;
		int minorVersion;
		int samples;
		union
		{
			struct
			{
				unsigned int    fullscreen : 1;
				unsigned int    vsync : 1;
				unsigned int    cursor : 1;
				unsigned int    stereo : 1;
				unsigned int    debug : 1;
			};
			unsigned int        all;
		} flags;
	};

protected:
	APPINFO     info;

	void setVsync(bool enable)
	{
		info.flags.vsync = enable ? 1 : 0;
		glfwSwapInterval((int)info.flags.vsync);
	}

	static void APIENTRY debug_callback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		GLvoid* userParam)
	{
		reinterpret_cast<application *>(userParam)->onDebugMessage(source, type, id, severity, length, message);
	}
};

NS_SB6_END

// sb6::application *app = 0;                          \

#if defined _WIN32
#define DECLARE_MAIN(a)                             \
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\
	AllocConsole(); \
	freopen("CONIN$", "r", stdin); \	freopen("CONOUT$", "w", stdout); \	freopen("CONOUT$", "w", stderr); \
	a *app = new a; \
	app->run(app); \
	delete app; \
	FreeConsole(); \
	return 0; \
}
#elif defined _LINUX || defined __APPLE__
#define DECLARE_MAIN(a)                             \
	int main(int argc, const char ** argv)              \
{                                                   \
	a *app = new a;                                 \
	app->run(app);                                  \
	delete app;                                     \
	return 0;                                       \
}
#else
#error Undefined platform!
#endif

#endif /* __SB6_H__ */

