#ifndef COGLU_H
#define COGLU_H

unsigned int coglu_shader_compile(const char *shader_path, int shader_type) {
    int length;

    // Read shader from file
    char *shader_source;
    FILE *shader_file = fopen(shader_path, "rb");
    if (shader_file) {
        fseek(shader_file, 0, SEEK_END);
        length = (int) ftell(shader_file);
        fseek (shader_file, 0, SEEK_SET);
        shader_source = malloc (length);
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

        char* info_log = malloc(sizeof(char) * info_log_length);
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

        fprintf(stderr, "Shader compilation error in file %s: %s\n", shader_path, info_log);
        free(info_log);
    }

    free(shader_source);

    return shader;
}

void coglu_shader_add_program(char *vertex_shader_path,
                              char *fragment_shader_path,
                              unsigned int *shader_program_ptr) {
    // Compile shaders
    unsigned int vertex_shader = coglu_shader_compile(vertex_shader_path,
                                                      GL_VERTEX_SHADER);
    unsigned int fragment_shader = coglu_shader_compile(fragment_shader_path,
                                                        GL_FRAGMENT_SHADER);

    // Link shader program
    *shader_program_ptr = glCreateProgram();
    glAttachShader(*shader_program_ptr, vertex_shader);
    glAttachShader(*shader_program_ptr, fragment_shader);
    glLinkProgram(*shader_program_ptr);
    glValidateProgram(*shader_program_ptr);

    // Clean up shaders, they're useless now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

#endif
