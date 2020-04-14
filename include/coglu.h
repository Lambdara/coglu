#ifndef COGLU_H
#define COGLU_H

#include <stdio.h>
#include <stdlib.h>

#ifdef COGLU_USE_GLEW
#include <GL/glew.h>
#endif

unsigned int coglu_shader_compile(const char *shader_path, int shader_type) {
    int length;

    // Read shader from file
    char *shader_source;
    FILE *shader_file = fopen(shader_path, "rb");
    if (shader_file) {
        fseek(shader_file, 0, SEEK_END);
        length = (int) ftell(shader_file);
        fseek (shader_file, 0, SEEK_SET);
        shader_source = malloc(length);
        fread(shader_source, 1, length, shader_file);
        fclose(shader_file);
    } else {
        fprintf(stderr, "Could not open vertex shader file\n");
        exit(1);
    }

    // Compile shader
    const char *shader_content = shader_source;
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_content, &length);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

        char *info_log = malloc(sizeof(char) * info_log_length);
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

        fprintf(stderr, "Shader compilation error in file %s: %s\n", shader_path, info_log);
        free(info_log);
    }

    free(shader_source);

    return shader;
}

unsigned int coglu_shader_load_program(const char *vertex_shader_path,
                              const char *fragment_shader_path) {
    // Compile shaders
    unsigned int vertex_shader = coglu_shader_compile(vertex_shader_path,
                                                      GL_VERTEX_SHADER);
    unsigned int fragment_shader = coglu_shader_compile(fragment_shader_path,
                                                        GL_FRAGMENT_SHADER);

    // Link shader program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    // Link shader program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

void coglu_shader_add_program(const char *vertex_shader_path,
                              const char *fragment_shader_path,
                              unsigned int *shader_program_ptr) {
    // Load shader program and assign result to provided shader program pointer.
    *shader_program_ptr = coglu_shader_load_program(vertex_shader_path, fragment_shader_path);
}

#endif
