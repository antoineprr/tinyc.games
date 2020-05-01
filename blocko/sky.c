#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern int check_shader_errors(unsigned int, char *);
extern int check_program_errors(unsigned int, char *);

unsigned int sun_prog_id;
GLuint sun_vbo, sun_vao;

void sun_init()
{
        const char *vertex_code = "\
#version 330 core                                                               \n\
layout (location = 0) in vec3 pos;                                              \n\
                                                                                \n\
uniform mat4 proj;                                                              \n\
uniform mat4 view;                                                              \n\
uniform mat4 model;                                                             \n\
                                                                                \n\
void main()                                                                     \n\
{                                                                               \n\
    gl_Position = proj * view * model * vec4(pos, 1);                           \n\
}                                                                               \n\
";

        const char *fragment_code = "\
#version 330 core                                                               \n\
out vec4 color;                                                                 \n\
                                                                                \n\
void main()                                                                     \n\
{                                                                               \n\
    color = vec4(1);                                                            \n\
}                                                                               \n\
";

        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, (const char *const *)&vertex_code, NULL);
        glCompileShader(vertex);
        check_shader_errors(vertex, "sun vertex");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, (const char *const *)&fragment_code, NULL);
        glCompileShader(fragment);
        check_shader_errors(fragment, "sun fragment");

        sun_prog_id = glCreateProgram();
        glAttachShader(sun_prog_id, vertex);
        glAttachShader(sun_prog_id, fragment);
        glLinkProgram(sun_prog_id);
        check_program_errors(sun_prog_id, "sun");
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        fprintf(stderr, "sun_prog_id = %d\n", sun_prog_id);

        glGenVertexArrays(1, &sun_vao);
        glGenBuffers(1, &sun_vbo);
}

void sun_draw(float *proj, float *view, float time_of_day)
{
        float sun_buf[] = {
                -1000, -10000, -1000,
                 1000, -10000, -1000,
                -1000, -10000,  1000,
                 1000, -10000,  1000,
        };
        float a = time_of_day * 3.14159f;
        float model[] = {
                 cosf(a), sinf(a), 0, 0,
                -sinf(a), cosf(a), 0, 0,
                       0,       0, 1, 0,
                       0,       0, 0, 1,
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_FALSE);
        glUseProgram(sun_prog_id);

        glUniformMatrix4fv(glGetUniformLocation(sun_prog_id, "proj"), 1, GL_FALSE, proj);
        glUniformMatrix4fv(glGetUniformLocation(sun_prog_id, "view"), 1, GL_FALSE, view);
        glUniformMatrix4fv(glGetUniformLocation(sun_prog_id, "model"), 1, GL_FALSE, model);

        glBindVertexArray(sun_vao);
        glBindBuffer(GL_ARRAY_BUFFER, sun_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof sun_buf, sun_buf, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
