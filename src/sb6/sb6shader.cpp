/*
 * Copyright ?2012-2013 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define _CRT_SECURE_NO_WARNINGS 1

#include "GL/gl3w.h"

#include <cstdio>
#include <string>

namespace sb6
{

namespace shader
{

extern
GLuint load(const char * filename, GLenum shader_type, bool check_errors)
{
    FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return 0;
	}

    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* data = new char[filesize + 1];
	if (!data) {
		return 0;
	}

    fread(data, 1, filesize, fp);
    data[filesize] = 0;
    fclose(fp);

    GLuint nShader = glCreateShader(shader_type);
	if (nShader == 0) {
		return nShader;
	}
    glShaderSource(nShader, 1, &data, NULL);

    delete [] data;

    glCompileShader(nShader);

	// Check the shader error
    if (check_errors) {
        GLint status = 0;
        glGetShaderiv(nShader, GL_COMPILE_STATUS, &status);

        if (status == GL_FALSE) {
			GLint nLength = 0;
			glGetShaderiv(nShader, GL_INFO_LOG_LENGTH, &nLength);

			char buffer[1024] = { 0 };
            glGetShaderInfoLog(nShader, nLength, NULL, buffer);
            fprintf(stderr, "%s: %s\n", filename, buffer);

			glDeleteShader(nShader);
        }
    }

    return nShader;
}

}

namespace program
{

GLuint link_from_shaders(const GLuint* shaders, int shader_count, bool bDeleteShader, bool bCheckError)
{
    GLuint nProgram = glCreateProgram();
    for (int i = 0; i < shader_count; i++) {
        glAttachShader(nProgram, shaders[i]);
    }

    glLinkProgram(nProgram);

    if (bCheckError) {
        GLint status;
        glGetProgramiv(nProgram, GL_LINK_STATUS, &status);

        if (!status) {
			char buffer[1024] = { 0 };
            glGetProgramInfoLog(nProgram, 1024, NULL, buffer);
            fprintf(stderr, "link error: %s\n", buffer);
            glDeleteProgram(nProgram);
            return 0;
        }
    }

    if (bDeleteShader) {
        for (int i = 0; i < shader_count; i++) {
            glDeleteShader(shaders[i]);
        }
    }

    return nProgram;
}

}

}
