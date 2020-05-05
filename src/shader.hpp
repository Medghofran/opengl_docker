#pragma once
#include <GL/glew.h>
#include <array>
#include <iostream>
#include <string>

class shader {
private:
  unsigned int _id;

public:
  shader(const std::string &_vertex, const std::string &_frag) {

    // create the shader program
    _id = glCreateProgram();
    std::cout << "Shader ID: " << _id << std::endl;

    // compile vertex and framgment source
    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    char *_vsrcptr = const_cast<char *>(_vertex.c_str());
    glShaderSource(vert, 1, &_vsrcptr, nullptr);
    glCompileShader(vert);
    get_status(vert, false);

    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
    char *_fsrcptr = const_cast<char *>(_frag.c_str());
    glShaderSource(frag, 1, &_fsrcptr, nullptr);
    glCompileShader(frag);
    get_status(frag, false);

    glAttachShader(_id, vert);
    glAttachShader(_id, frag);
    glLinkProgram(_id);
    get_status(_id, true);

    // delete shaders as they are already linked in the current shader program
    glDeleteShader(vert);
    glDeleteShader(frag);
  }
  ~shader() { glDeleteShader(_id); }

  void use() { glUseProgram(_id); }

  void get_status(int _id, bool linking_status) {
    GLint _success;
    auto _test = linking_status ? GL_LINK_STATUS : GL_COMPILE_STATUS;
    glGetShaderiv(_id, _test, &_success);
    if (_success < 0) {
      std::array<char, 512> infoLog;
      glGetShaderInfoLog(_id, 512, nullptr, infoLog.data());
      if (!linking_status)
        printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED::%s\n",
               infoLog.data());
      else
        printf("ERROR::SHADER::PROGRAM::LINKEAGE_FAILED \n");
    }
  }

  void set_mat4(const std::string &_param,
                const std::array<float, 16> &_mat) const {
    glUniformMatrix4fv(glGetUniformLocation(_id, _param.c_str()), 1, GL_FALSE,
                       _mat.data());
  }

  void set_vec4(const std::string &_param,
                const std::array<float, 4> &_vec) const {
    glUniformMatrix4fv(glGetUniformLocation(_id, _param.c_str()), 1, GL_FALSE,
                       _vec.data());
  }

  void set_vec3(const std::string &_param,
                const std::array<float, 3> &_vec) const {
    glUniformMatrix3fv(glGetUniformLocation(_id, _param.c_str()), 1, GL_FALSE,
                       _vec.data());
  }

  void set_mat2x4(const std::string &_param,
                  const std::array<float, 8> &_mat) const {

    glUniformMatrix2x4fv(glGetUniformLocation(_id, _param.c_str()), 1, GL_FALSE,
                         _mat.data());
  }

  void set_int(const std::string &_param, const int &_int) const {
    glUniform1i(glGetUniformLocation(_id, _param.c_str()), _int);
  }

  void set_bool(const std::string &_param, const bool &_bool) const {
    glUniform1i(glGetUniformLocation(_id, _param.c_str()), _bool);
  }

  void set_float(const std::string &_param, const float &_float) const {
    glUniform1f(glGetUniformLocation(_id, _param.c_str()), _float);
  }

  inline void attach(unsigned int _shaderId) { glAttachShader(_id, _shaderId); }
};