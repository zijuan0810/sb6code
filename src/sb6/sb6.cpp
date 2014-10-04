#include "sb6.h"
#include <GL/glext.h>
#include <string.h>

void * sb6GetProcAddress(const char * funcname)
{
    return gl3wGetProcAddress(funcname);
}

int sb6IsExtensionSupported(const char * extname)
{
    GLint numExtensions;
    GLint i;

    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    for (i = 0; i < numExtensions; i++)
    {
        const GLubyte * e = glGetStringi(GL_EXTENSIONS, i);
        if (!strcmp((const char *)e, extname))
        {
            return 1;
        }
    }

    return 0;
}

NS_SB6_BEGIN

static application* app = nullptr;

static void GLFWCALL glfw_onResize(int w, int h)
{
	app->onResize(w, h);
}

static void GLFWCALL glfw_onKey(int key, int action)
{
	app->onKey(key, action);
}

static void GLFWCALL glfw_onMouseButton(int button, int action)
{
	app->onMouseButton(button, action);
}

static void GLFWCALL glfw_onMouseMove(int x, int y)
{
	app->onMouseMove(x, y);
}

static void GLFWCALL glfw_onMouseWheel(int pos)
{
	app->onMouseWheel(pos);
}

static void GLFWCALL glfw_onKeyChar(int key, int action)
{
	app->onKeyChar(key, action);
}

void sb6::application::run(sb6::application* the_app)
{
	bool running = true;
	app = the_app;

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	init();

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, info.majorVersion);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, info.minorVersion);

#ifdef _DEBUG
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif /* _DEBUG */
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, info.samples);
	glfwOpenWindowHint(GLFW_STEREO, info.flags.stereo ? GL_TRUE : GL_FALSE);
	if (info.flags.fullscreen)
	{
		if (info.windowWidth == 0 || info.windowHeight == 0)
		{
			GLFWvidmode mode;
			glfwGetDesktopMode(&mode);
			info.windowWidth = mode.Width;
			info.windowHeight = mode.Height;
		}
		glfwOpenWindow(info.windowWidth, info.windowHeight, 8, 8, 8, 0, 32, 0, GLFW_FULLSCREEN);
		glfwSwapInterval((int)info.flags.vsync);
	}
	else
	{
		if (!glfwOpenWindow(info.windowWidth, info.windowHeight, 8, 8, 8, 0, 32, 0, GLFW_WINDOW))
		{
			fprintf(stderr, "Failed to open window\n");
			return;
		}
	}

	glfwSetWindowTitle(info.title);
	glfwSetWindowSizeCallback(glfw_onResize);
	glfwSetKeyCallback(glfw_onKey);
	glfwSetMouseButtonCallback(glfw_onMouseButton);
	glfwSetMousePosCallback(glfw_onMouseMove);
	glfwSetMouseWheelCallback(glfw_onMouseWheel);
	(info.flags.cursor ? glfwEnable : glfwDisable)(GLFW_MOUSE_CURSOR);

	info.flags.stereo = (glfwGetWindowParam(GLFW_STEREO) ? 1 : 0);

	gl3wInit();

#ifdef _DEBUG
	fprintf(stderr, "VENDOR: %s\n", (char *)glGetString(GL_VENDOR));
	fprintf(stderr, "VERSION: %s\n", (char *)glGetString(GL_VERSION));
	fprintf(stderr, "RENDERER: %s\n", (char *)glGetString(GL_RENDERER));
#endif

	if (info.flags.debug)
	{
		if (gl3wIsSupported(4, 3))
		{
			glDebugMessageCallback(debug_callback, this);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
		else if (sb6IsExtensionSupported("GL_ARB_debug_output"))
		{
			glDebugMessageCallbackARB(debug_callback, this);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}
	}

	startup();

	do
	{
		render(glfwGetTime());

		glfwSwapBuffers();

		running &= (glfwGetKey(GLFW_KEY_ESC) == GLFW_RELEASE);
		running &= (glfwGetWindowParam(GLFW_OPENED) != GL_FALSE);
	} while (running);

	shutdown();

		glfwTerminate();
}


void application::init()
{
		strcpy(info.title, "SuperBible6 Example");
		info.windowWidth = 800;
		info.windowHeight = 600;
#ifdef __APPLE__
		info.majorVersion = 3;
		info.minorVersion = 2;
#else
		info.majorVersion = 4;
		info.minorVersion = 3;
#endif
		info.samples = 0;
		info.flags.all = 0;
		info.flags.cursor = 1;
#ifdef _DEBUG
		info.flags.debug = 1;
#endif
}

void application::onDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, 
	GLsizei length, const GLchar* message)
{
#ifdef _WIN32
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
#endif /* _WIN32 */
}

void application::getMousePosition(int& x, int& y)
{
	glfwGetMousePos(&x, &y);
}

NS_SB6_END
