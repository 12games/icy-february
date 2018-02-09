#ifndef GLCOLORNORMALPOSITIONVERTEX_H
#define GLCOLORNORMALPOSITIONVERTEX_H

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

#if FALSE
namespace glm {

struct vec3
{
    vec3() : x(0), y(0), z(0) {}
    vec3(float v) : x(v), y(v), z(v) {}
    vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    union {
        float x;
        float r;
    };
    union {
        float y;
        float g;
    };
    union {
        float z;
        float b;
    };

    float const &operator[](int index) const
    {
        if (index == 0) return x;
        if (index == 1) return y;

        return z;
    }

    float &operator[](int index)
    {
        if (index == 0) return x;
        if (index == 1) return y;

        return z;
    }
};

vec3 operator+(vec3 const &v1, vec3 const &v2)
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

vec3 operator-(vec3 const &v1, vec3 const &v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

vec3 cross(vec3 const &v1, vec3 const &v2)
{
    return vec3(v1.y * v2.z - v1.z * v2.y,
                v1.z * v2.x - v1.x * v2.z,
                v1.x * v2.y - v1.y * v2.x);
}

float length(vec3 const &v)
{
    return float(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

vec3 normal(vec3 const &v)
{
    auto l = length(v);
    return vec3(v.x / l, v.y / l, v.z / l);
}

struct vec4
{
    vec4() : x(0), y(0), z(0), w(0) {}
    vec4(float v) : x(v), y(v), z(v), w(v) {}
    vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    union {
        float x;
        float r;
    };
    union {
        float y;
        float g;
    };
    union {
        float z;
        float b;
    };
    union {
        float w;
        float a;
    };

    float const &operator[](int index) const
    {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;

        return w;
    }

    float &operator[](int index)
    {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;

        return w;
    }
};

struct mat4
{
    mat4() {}
    mat4(float v)
    {
        // Identity
        values[0].x = v;
        values[1].y = v;
        values[2].z = v;
        values[3].w = v;
    }
    mat4(vec4 const &v0, vec4 const &v1, vec4 const &v2, vec4 const &v3)
    {
        values[0] = v0;
        values[1] = v1;
        values[2] = v2;
        values[3] = v3;
    }

    vec4 values[4];

    vec4 const &operator[](int index) const
    {
        return values[index];
    }

    vec4 &operator[](int index)
    {
        return values[index];
    }
};

vec4 operator*(mat4 const &m, vec4 const &v)
{
    return vec4(
        m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
        m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
        m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
        m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]);
}

vec4 operator*(vec4 const &v, mat4 const &m)
{
    return vec4(
        v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + v[3] * m[0][3],
        v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + v[3] * m[1][3],
        v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + v[3] * m[2][3],
        v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + v[3] * m[3][3]);
}

mat4 operator*(mat4 const &m1, mat4 const &m2)
{
    vec4 X = m1 * m2[0];
    vec4 Y = m1 * m2[1];
    vec4 Z = m1 * m2[2];
    vec4 W = m1 * m2[3];

    return mat4(X, Y, Z, W);
}

float radians(float degrees)
{
    return degrees * 0.01745329251994329576923690768489f;
}

float const *value_ptr(mat4 const &m)
{
    return &m.values[0].x;
}

mat4 perspective(float fovy, float aspect, float zNear, float zFar)
{
    float const tanHalfFovy = tan(fovy / static_cast<float>(2));

    mat4 m(static_cast<float>(0));
    m[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
    m[1][1] = static_cast<float>(1) / (tanHalfFovy);
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -static_cast<float>(1);
    m[3][2] = -(static_cast<float>(2) * zFar * zNear) / (zFar - zNear);

    return m;
}

mat4 lookAt(vec3 const &eye, vec3 const &target, vec3 const &up)
{
    vec3 zaxis = normal(eye - target);
    vec3 xaxis = normal(cross(up, zaxis));
    vec3 yaxis = cross(zaxis, xaxis);

    mat4 orientation = {
        vec4(xaxis.x, yaxis.x, zaxis.x, 0),
        vec4(xaxis.y, yaxis.y, zaxis.y, 0),
        vec4(xaxis.z, yaxis.z, zaxis.z, 0),
        vec4(0, 0, 0, 1)};

    mat4 translation = {
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(-eye.x, -eye.y, -eye.z, 1)};

    return (orientation * translation);
}

std::string to_string(vec4 const &x)
{
    std::stringstream ss;

    ss << "(" << x.x << ", " << x.y << ", " << x.z << ", " << x.w << ")";

    return ss.str();
}

std::string to_string(mat4 const &x)
{
    std::stringstream ss;

    ss << "mat4x4(" << to_string(x.values[0]) << ", "
       << to_string(x.values[1]) << ", "
       << to_string(x.values[2]) << ", "
       << to_string(x.values[3]) << ")";

    return ss.str();
}

} // namespace glm

#endif // GLM

class VertexType
{
public:
    glm::vec3 pos;
    glm::vec4 col;
    glm::vec3 nor;
};

class ShaderType
{
    GLuint _shaderId;
    GLuint _projectionUniformId;
    GLuint _viewUniformId;
    GLuint _modelUniformId;

    std::string _projectionUniformName;
    std::string _viewUniformName;
    std::string _modelUniformName;

    std::string _vertexAttributeName;
    std::string _colorAttributeName;
    std::string _normalAttributeName;

public:
    ShaderType()
        : _shaderId(0), _projectionUniformId(0), _viewUniformId(0), _modelUniformId(0),
          _projectionUniformName("u_projection"), _viewUniformName("u_view"), _modelUniformName("u_model"),
          _vertexAttributeName("vertex"), _colorAttributeName("color"), _normalAttributeName("normal")
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
                "in vec3 normal;\n"

                "uniform mat4 u_projection;\n"
                "uniform mat4 u_view;\n"
                "uniform mat4 u_model;\n"

                "out vec4 f_color;\n"

                "void main()\n"
                "{\n"
                "    gl_Position = u_projection * u_view * u_model * vec4(vertex.xyz, 1.0);\n"
                "    f_color = color;\n"

                "    vec3 vertexPosition_cameraspace  = (u_view * u_model * vec4(vertex, 0)).xyz;\n"
                "    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"
                "    vec3 LightPosition_cameraspace = (u_view * vec4(-500.0, -500.0, 500.0,1)).xyz;\n"
                "    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;\n"
                "    vec3 Normal_cameraspace = (u_view * u_model * vec4(normal, 0)).xyz;\n"
                "    vec3 n = normalize( Normal_cameraspace );\n"
                "    vec3 l = normalize( LightDirection_cameraspace );\n"
                "    float cosTheta = clamp(dot(n, l), 0.3, 1);\n"

                "    f_color = (cosTheta * color) + (color * vec4(0.8, 0.8, 0.8, 1.0));\n"
                "}\n");

            std::string const fshader(
                "#version 150\n"

                "in vec4 f_color;\n"
                "out vec4 color;\n"

                "void main()\n"
                "{\n"
                "   color = f_color;\n"
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

    void setupAttributes() const
    {
        auto vertexSize = sizeof(VertexType);

        auto vertexAttrib = glGetAttribLocation(_shaderId, _vertexAttributeName.c_str());
        glVertexAttribPointer(GLuint(vertexAttrib), sizeof(VertexType::pos) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(GLuint(vertexAttrib));

        auto colorAttrib = glGetAttribLocation(_shaderId, _colorAttributeName.c_str());
        glVertexAttribPointer(GLuint(colorAttrib), sizeof(VertexType::col) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid *>(sizeof(VertexType::pos)));
        glEnableVertexAttribArray(GLuint(colorAttrib));

        auto normalAttrib = glGetAttribLocation(_shaderId, _normalAttributeName.c_str());
        glVertexAttribPointer(GLuint(normalAttrib), sizeof(VertexType::nor) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid *>(sizeof(VertexType::pos) + sizeof(VertexType::col)));
        glEnableVertexAttribArray(GLuint(normalAttrib));
    }
};

class BufferType
{
    const ShaderType &_shader;
    int _vertexCount;
    std::vector<VertexType> _verts;
    glm::vec4 _nextColor;
    glm::vec3 _nextNormal;
    unsigned int _vertexArrayId;
    unsigned int _vertexBufferId;
    GLenum _drawMode;
    std::map<int, int> _faces;

public:
    BufferType(ShaderType const &shader)
        : _shader(shader), _vertexCount(0), _vertexArrayId(0), _vertexBufferId(0), _drawMode(GL_TRIANGLES),
          _nextColor(glm::vec4(1.0f))
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
        _verts.push_back(VertexType({position, _nextColor, _nextNormal}));

        _vertexCount = _verts.size();

        return *this;
    }

    BufferType &color(glm::vec4 const &color)
    {
        _nextColor = color;

        return *this;
    }

    BufferType &normal(glm::vec3 const &normal)
    {
        _nextNormal = normal;

        return *this;
    }

    BufferType &colorVertex(glm::vec4 const &color, glm::vec3 const &position)
    {
        _nextColor = color;

        return vertex(position);
    }

    BufferType &colorNormalVertex(glm::vec4 const &color, glm::vec4 const &normal, glm::vec3 const &position)
    {
        _nextColor = color;
        _nextNormal = normal;

        return vertex(position);
    }

    BufferType &planeTriangleFan()
    {
        _drawMode = GL_TRIANGLE_FAN;

        return (*this)
            .vertex(glm::vec3(-0.5f, -0.5f, 0.0f))
            .vertex(glm::vec3(-0.5f, 0.5f, 0.0f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.0f))
            .vertex(glm::vec3(0.5f, -0.5f, 0.0f));
    }

    BufferType &cubeTriangles()
    {
        _drawMode = GL_TRIANGLES;

        return (*this)
            .normal(glm::vec3(0.0f, 0.0f, 1.0f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, 0.5f))

            .normal(glm::vec3(0.0f, 0.0f, -1.0f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, -0.5f))

            .normal(glm::vec3(0.0f, 1.0f, 0.0f))
            .vertex(glm::vec3(-0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, -0.5f))

            .normal(glm::vec3(0.0f, -1.0f, 0.0f))
            .vertex(glm::vec3(0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))

            .normal(glm::vec3(-1.0f, 0.0f, 0.0f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(-0.5f, 0.5f, -0.5f))

            .normal(glm::vec3(1.0f, 0.0f, 0.0f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, -0.5f))
            .vertex(glm::vec3(0.5f, 0.5f, 0.5f))
            .vertex(glm::vec3(0.5f, -0.5f, -0.5f));
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

    BufferType &fillColor(glm::vec4 const &color)
    {
        for (VertexType &v : _verts)
        {
            v.col = color;
        }

        return (*this);
    }

#ifdef TINY_OBJ_LOADER_H_

    BufferType &loadObj(std::string const &filename, std::string const &materialPath, std::string const &shapeName)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), materialPath.c_str()))
        {
            std::cerr << "LoadObj failed" << std::endl;

            return *this;
        }

        for (size_t s = 0; s < shapes.size(); s++)
        {
            if (shapes[s].name == shapeName)
            {
                // Loop over faces(polygon)
                size_t index_offset = 0;
                int faceCount = shapes[s].mesh.num_face_vertices.size();
                for (int f = 0; f < faceCount; f++)
                {
                    if (materials.size() > 0 && shapes[s].mesh.material_ids[f] >= 0)
                    {
                        // per-face material
                        auto m = materials[shapes[s].mesh.material_ids[f]];

                        this->color(glm::vec4(m.diffuse[0], m.diffuse[1], m.diffuse[2], 1.0f));
                    }

                    int fv = shapes[s].mesh.num_face_vertices[f];

                    // Loop over vertices in the face.
                    for (size_t v = 0; v < fv; v++)
                    {
                        // access to vertex
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                        tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                        tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                        tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                        tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                        tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                        // tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                        // tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                        // Optional: vertex colors
                        // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                        // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                        // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

                        this->normal(glm::vec3(nx, ny, nz))
                            .vertex(glm::vec3(vx, vy, vz));
                    }
                    index_offset += fv;
                }
            }
        }

        return *this;
    }

#endif

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

#endif // GLCOLORNORMALPOSITIONVERTEX_H
