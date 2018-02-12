#ifndef GLCOLORPOSITIONVERTEX_H
#define GLCOLORPOSITIONVERTEX_H

#include <cmath>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

namespace ColorPosition {

class VertexType
{
public:
    glm::vec3 pos;
    glm::vec4 col;
};

class ShaderType
{
    GLuint _shaderId;
    GLuint _projectionUniformId;
    GLuint _viewUniformId;
    GLuint _modelUniformId;
    GLuint _colorUniformId;

    std::string _projectionUniformName;
    std::string _viewUniformName;
    std::string _modelUniformName;
    std::string _colorUniformName;

    std::string _vertexAttributeName;
    std::string _colorAttributeName;

    static GLuint defaultShader;

public:
    ShaderType()
        : _shaderId(0), _projectionUniformId(0), _viewUniformId(0), _modelUniformId(0), _colorUniformId(0),
          _projectionUniformName("u_projection"), _viewUniformName("u_view"), _modelUniformName("u_model"), _colorUniformName("u_color"),
          _vertexAttributeName("vertex"), _colorAttributeName("color")
    {}

    virtual ~ShaderType() {}

    GLuint id() const
    {
        return _shaderId;
    }

    void use() const
    {
        glUseProgram(_shaderId);
    }

    virtual bool compileFromFile(std::string const &vertShaderFile, std::string const &fragShaderFile)
    {
        std::ifstream vertShaderFileStream(vertShaderFile.c_str());
        std::string vertShaderStr((std::istreambuf_iterator<char>(vertShaderFileStream)),
                                  std::istreambuf_iterator<char>());

        std::ifstream fragShaderFileStream(fragShaderFile.c_str());
        std::string fragShaderStr((std::istreambuf_iterator<char>(fragShaderFileStream)),
                                  std::istreambuf_iterator<char>());

        return compile(vertShaderStr, fragShaderStr);
    }

    bool compileDefaultShader()
    {
        if (ShaderType::defaultShader != 0)
        {
            _shaderId = ShaderType::defaultShader;

            _projectionUniformId = glGetUniformLocation(_shaderId, _projectionUniformName.c_str());
            _viewUniformId = glGetUniformLocation(_shaderId, _viewUniformName.c_str());
            _modelUniformId = glGetUniformLocation(_shaderId, _modelUniformName.c_str());
            _colorUniformId = glGetUniformLocation(_shaderId, _colorUniformName.c_str());

            return true;
        }

        std::string const vshader(
            "#version 150\n"

            "in vec3 vertex;"
            "in vec4 color;"

            "uniform mat4 u_projection;"
            "uniform mat4 u_view;"
            "uniform mat4 u_model;"

            "out vec4 f_color;"

            "void main()"
            "{"
            "    gl_Position = u_projection * u_view * u_model * vec4(vertex.xyz, 1.0);"
            "    f_color = color;"
            "}");

        std::string const fshader(
            "#version 150\n"

            "uniform vec4 u_color;\n"

            "in vec4 f_color;"
            "out vec4 color;"

            "void main()"
            "{"
            "   color = f_color * u_color;"
            "}");

        if (!compile(vshader, fshader))
        {
            return false;
        }

        ShaderType::defaultShader = _shaderId;

        return true;
    }

    virtual bool compile(std::string const &vertShaderStr, std::string const &fragShaderStr)
    {
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *vertShaderSrc = vertShaderStr.c_str();
        const char *fragShaderSrc = fragShaderStr.c_str();

        GLint result = GL_FALSE;
        GLint logLength;

        // Compile vertex shader
        glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
        glCompileShader(vertShader);

        // Check vertex shader
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> vertShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
            std::cout << &vertShaderError[0] << std::endl;

            return false;
        }

        // Compile fragment shader
        glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
        glCompileShader(fragShader);

        // Check fragment shader
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> fragShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
            std::cout << &fragShaderError[0] << std::endl;

            return false;
        }

        _shaderId = glCreateProgram();
        glAttachShader(_shaderId, vertShader);
        glAttachShader(_shaderId, fragShader);
        glLinkProgram(_shaderId);

        glGetProgramiv(_shaderId, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetProgramiv(_shaderId, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> programError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetProgramInfoLog(_shaderId, logLength, NULL, &programError[0]);
            std::cout << &programError[0] << std::endl;

            return false;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        _projectionUniformId = glGetUniformLocation(_shaderId, _projectionUniformName.c_str());
        _viewUniformId = glGetUniformLocation(_shaderId, _viewUniformName.c_str());
        _modelUniformId = glGetUniformLocation(_shaderId, _modelUniformName.c_str());
        _colorUniformId = glGetUniformLocation(_shaderId, _colorUniformName.c_str());

        return true;
    }

    void setupMatrices(glm::mat4 const &projection, glm::mat4 const &view, glm::mat4 const &model)
    {
        use();

        glUniformMatrix4fv(_projectionUniformId, 1, false, glm::value_ptr(projection));
        glUniformMatrix4fv(_viewUniformId, 1, false, glm::value_ptr(view));
        glUniformMatrix4fv(_modelUniformId, 1, false, glm::value_ptr(model));
    }

    void setupMatrices(glm::mat4 const &projectionView, glm::mat4 const &model)
    {
        use();

        glUniformMatrix4fv(_projectionUniformId, 1, false, glm::value_ptr(projectionView));
        glUniformMatrix4fv(_modelUniformId, 1, false, glm::value_ptr(model));
    }

    void setupColor(glm::vec4 const &color)
    {
        use();

        glUniform4fv(_colorUniformId, 1, glm::value_ptr(color));
    }

    void setupAttributes() const
    {
        auto vertexSize = sizeof(VertexType);

        auto vertexAttrib = glGetAttribLocation(_shaderId, _vertexAttributeName.c_str());
        glVertexAttribPointer(GLuint(vertexAttrib), sizeof(VertexType::pos) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(GLuint(vertexAttrib));

        auto colorAttrib = glGetAttribLocation(_shaderId, _colorAttributeName.c_str());
        glVertexAttribPointer(GLuint(colorAttrib), sizeof(VertexType::col) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid *>(sizeof(VertexType::pos)));
        glEnableVertexAttribArray(GLuint(colorAttrib));
    }
};

//GLuint ShaderType::defaultShader = 0;

class BufferType
{
    int _vertexCount;
    std::vector<VertexType> _verts;
    glm::vec4 _nextColor;
    unsigned int _vertexArrayId;
    unsigned int _vertexBufferId;
    GLenum _drawMode;
    std::map<int, int> _faces;

public:
    BufferType()
        : _vertexCount(0), _vertexArrayId(0), _vertexBufferId(0), _drawMode(GL_TRIANGLES)
    {}

    virtual ~BufferType() {}

    bool setup(ShaderType const *shader)
    {
        return setup(shader, _drawMode);
    }

    bool setup(ShaderType const *shader, GLenum mode)
    {
        if (shader == nullptr)
        {
            return false;
        }

        _drawMode = mode;
        _vertexCount = _verts.size();

        glGenVertexArrays(1, &_vertexArrayId);
        glGenBuffers(1, &_vertexBufferId);

        glBindVertexArray(_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(_verts.size() * sizeof(VertexType)), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(_verts.size() * sizeof(VertexType)), reinterpret_cast<const GLvoid *>(&_verts[0]));

        shader->setupAttributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        _verts.clear();

        return true;
    }

    void render()
    {
        glBindVertexArray(_vertexArrayId);
        if (_faces.empty())
        {
            glDrawArrays(_drawMode, 0, _vertexCount);
        }
        else
        {
            for (auto pair : _faces)
            {
                glDrawArrays(_drawMode, pair.first, pair.second);
            }
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void cleanup()
    {
        if (_vertexBufferId != 0)
        {
            glDeleteBuffers(1, &_vertexBufferId);
            _vertexBufferId = 0;
        }
        if (_vertexArrayId != 0)
        {
            glDeleteVertexArrays(1, &_vertexArrayId);
            _vertexArrayId = 0;
        }
    }

    std::vector<VertexType> &verts()
    {
        return _verts;
    }

    BufferType &operator<<(VertexType const &vertex)
    {
        _verts.push_back(vertex);
        _vertexCount = _verts.size();

        return *this;
    }

    void setDrawMode(GLenum mode)
    {
        _drawMode = mode;
    }

    void addFace(int start, int count)
    {
        _faces.insert(std::make_pair(start, count));
    }

    int vertexCount() const
    {
        return _vertexCount;
    }

    BufferType &vertex(glm::vec3 const &position)
    {
        _verts.push_back(VertexType({position,
                                     _nextColor}));

        _vertexCount = _verts.size();

        return *this;
    }

    BufferType &color(glm::vec4 const &color)
    {
        _nextColor = color;

        return *this;
    }

    BufferType &colorVertex(glm::vec4 const &color, glm::vec3 const &position)
    {
        _nextColor = color;

        return vertex(position);
    }

    BufferType &planeTriangleFan()
    {
        _drawMode = GL_TRIANGLE_FAN;

        return (*this)
            .colorVertex(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.0f)) // mint
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, 0.0f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.0f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.0f))  // wit
            ;
    }

    BufferType &cubeTriangles()
    {
        _drawMode = GL_TRIANGLES;

        return (*this)
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, 0.5f))  // geel
            .colorVertex(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.5f)) // mint

            .colorVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.5f))  // wit
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.5f)) // mint

            .colorVertex(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // mint
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, -0.5f))   // paars

            .colorVertex(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // mint
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, -0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.5f, -0.5f, -0.5f))  // wit

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f)) // geel
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, 0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f)) // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, -0.5f))  // geel

            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // geel

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, -0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // geel

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // geel
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, 0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(-0.5f, 0.5f, 0.5f))   // paars

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(-0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f))  // geel

            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, -0.5f)) // geel

            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, -0.5f))  // geel
            .colorVertex(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f))   // paars
            .colorVertex(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, -0.5f)) // geel
            ;
    }

    BufferType &scale(glm::vec3 const &amount)
    {
        for (VertexType &v : _verts)
        {
            v.pos *= amount;
        }

        return (*this);
    }

    BufferType &move(glm::vec3 const &amount)
    {
        for (VertexType &v : _verts)
        {
            v.pos += amount;
        }

        return (*this);
    }
};

} // namespace ColorPosition

#endif // GLCOLORPOSITIONVERTEX_H
