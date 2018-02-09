#ifndef GLMASKEDTEXTURES_H
#define GLMASKEDTEXTURES_H

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

namespace MaskedTexturesBuffer {

class VertexType
{
public:
    glm::vec3 pos;
    glm::vec4 col;
    glm::vec4 uvs;
};

class ShaderType
{
    GLuint _shaderId;
    GLuint _projectionUniformId;
    GLuint _viewUniformId;
    GLuint _modelUniformId;
    GLuint _textureUniform1Id;
    GLuint _textureUniform2Id;
    GLuint _textureUniform3Id;
    GLuint _textureUniformMaskId;

    std::string _projectionUniformName;
    std::string _viewUniformName;
    std::string _modelUniformName;
    std::string _textureUniform1Name;
    std::string _textureUniform2Name;
    std::string _textureUniform3Name;
    std::string _textureUniformMaskName;

    std::string _vertexAttributeName;
    std::string _colorAttributeName;
    std::string _uvsAttributeName;

public:
    ShaderType()
        : _shaderId(0), _projectionUniformId(0), _viewUniformId(0), _modelUniformId(0),
          _projectionUniformName("u_projection"), _viewUniformName("u_view"), _modelUniformName("u_model"),
          _textureUniform1Name("u_texture1"), _textureUniform2Name("u_texture2"), _textureUniform3Name("u_texture3"), _textureUniformMaskName("u_mask"),
          _vertexAttributeName("vertex"), _colorAttributeName("color"), _uvsAttributeName("uvs")
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
        static GLuint defaultShader = 0;

        if (defaultShader == 0)
        {
            std::string const vshader(
                "#version 150\n"

                "in vec3 vertex;\n"
                "in vec4 color;\n"
                "in vec4 uvs;\n"

                "uniform mat4 u_projection;\n"
                "uniform mat4 u_view;\n"
                "uniform mat4 u_model;\n"

                "out vec4 f_color;\n"
                "out vec4 f_uvs;\n"

                "void main()\n"
                "{\n"
                "    gl_Position = u_projection * u_view * u_model * vec4(vertex.xyz, 1.0);\n"
                "    f_color = color;\n"
                "    f_uvs = vec4(uvs.st, vertex.x > 0 ? 1.0f : 0.0f, vertex.y > 0 ? 1.0f : 0.0f);\n"
                "}\n");

            std::string const fshader(
                "#version 150\n"

                "uniform sampler2D u_texture1;\n"
                "uniform sampler2D u_texture2;\n"
                "uniform sampler2D u_texture3;\n"
                "uniform sampler2D u_mask;\n"

                "in vec4 f_color;\n"
                "in vec4 f_uvs;\n"
                "out vec4 color;\n"

                "void main()\n"
                "{\n"
                "   vec4 mask = texture(u_mask, f_uvs.zw);\n"
                "   vec4 color1 = (texture(u_texture1, f_uvs.st) * mask.x)\n"
                "               + (texture(u_texture2, f_uvs.st) * (1.0 - mask.x));\n"
                "   vec4 color2 = (color1 * mask.y)\n"
                "               + (texture(u_texture3, f_uvs.st) * (1.0 - mask.y));\n"
                "   color = color2;\n"
                "}\n");

            if (compile(vshader, fshader))
            {
                defaultShader = _shaderId;

                return true;
            }

            return false;
        }

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

        _textureUniform1Id = glGetUniformLocation(_shaderId, _textureUniform1Name.c_str());
        _textureUniform2Id = glGetUniformLocation(_shaderId, _textureUniform2Name.c_str());
        _textureUniform3Id = glGetUniformLocation(_shaderId, _textureUniform3Name.c_str());
        _textureUniformMaskId = glGetUniformLocation(_shaderId, _textureUniformMaskName.c_str());

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

    void setupTextures(unsigned int texture0, unsigned int texture1, unsigned int texture2, unsigned int mask) const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glUniform1i(_textureUniform1Id, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(_textureUniform2Id, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(_textureUniform3Id, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mask);
        glUniform1i(_textureUniformMaskId, 3);
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

        auto uvsAttrib = glGetAttribLocation(_shaderId, _uvsAttributeName.c_str());
        glVertexAttribPointer(GLuint(uvsAttrib), sizeof(VertexType::uvs) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid *>(sizeof(VertexType::pos) + sizeof(VertexType::col)));
        glEnableVertexAttribArray(GLuint(uvsAttrib));
    }
};

class BufferType
{
    const ShaderType &_shader;
    int _vertexCount;
    std::vector<VertexType> _verts;
    glm::vec4 _nextColor, _nextUvs;
    unsigned int _vertexArrayId;
    unsigned int _vertexBufferId;
    GLenum _drawMode;
    std::map<int, int> _faces;

public:
    BufferType(ShaderType const &shader)
        : _shader(shader), _vertexCount(0), _vertexArrayId(0), _vertexBufferId(0), _drawMode(GL_TRIANGLES)
    {}

    virtual ~BufferType() {}

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
                                     _nextColor,
                                     _nextUvs}));

        _vertexCount = _verts.size();

        return *this;
    }

    BufferType &color(glm::vec4 const &color)
    {
        _nextColor = color;

        return *this;
    }

    BufferType &uvs(glm::vec4 const &uvs)
    {
        _nextUvs = uvs;

        return *this;
    }

    BufferType &colorVertex(glm::vec4 const &color, glm::vec3 const &position)
    {
        _nextColor = color;

        return vertex(position);
    }

    BufferType &colorUvsVertex(glm::vec4 const &color, glm::vec4 const &uvs, glm::vec3 const &position)
    {
        _nextColor = color;
        _nextUvs = uvs;

        return vertex(position);
    }

    BufferType &planeTriangleFan(glm::vec2 const &size, glm::vec2 const &textureSize)
    {
        _drawMode = GL_TRIANGLE_FAN;

        auto uvAspect = glm::vec2(size.x / textureSize.x, size.y / textureSize.y);

        return (*this)
            .colorUvsVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec3(size.x * -0.5f, size.y * -0.5f, 0.0f))           // mint
            .colorUvsVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, uvAspect.y, 0.0f, 0.0f), glm::vec3(size.x * -0.5f, size.y * 0.5f, 0.0f))      // geel
            .colorUvsVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(uvAspect.x, uvAspect.y, 0.0f, 0.0f), glm::vec3(size.x * 0.5f, size.y * 0.5f, 0.0f)) // paars
            .colorUvsVertex(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(uvAspect.x, 0.0f, 0.0f, 0.0f), glm::vec3(size.x * 0.5f, size.y * -0.5f, 0.0f))      // wit
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

    bool setup()
    {
        return setup(_drawMode);
    }

    bool setup(GLenum mode)
    {
        _drawMode = mode;
        _vertexCount = _verts.size();

        glGenVertexArrays(1, &_vertexArrayId);
        glGenBuffers(1, &_vertexBufferId);

        glBindVertexArray(_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(_verts.size() * sizeof(VertexType)), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(_verts.size() * sizeof(VertexType)), reinterpret_cast<const GLvoid *>(&_verts[0]));

        _shader.setupAttributes();

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
};

} // namespace MaskedTexturesBuffer

#endif // GLMASKEDTEXTURES_H
