#include "icyfebruary.h"
#include <capabilityguard.h>
#include <glad/glad.h>
#include <imgui.h>

#define SYSTEM_IO_FILEINFO_IMPLEMENTATION
#include <system.io.fileinfo.h>
#include <system.io.path.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define KEYMAP_FILE "icyfebruary.keymap"

ColorPosition::ShaderType CreationObject::_shader;
GLuint ColorPosition::ShaderType::defaultShader = 0;

void CreationObject::rebuildBuffer()
{
    _buffer.cleanup();
    _buffer.setDrawMode(GL_LINES);
    _buffer.color(glm::vec4(255.0f));

    _buffer.vertex(glm::vec3(_size.x, _size.y, _size.z));
    _buffer.vertex(glm::vec3(_size.x, _size.y, -_size.z));

    _buffer.vertex(glm::vec3(_size.x, -_size.y, _size.z));
    _buffer.vertex(glm::vec3(_size.x, -_size.y, -_size.z));

    _buffer.vertex(glm::vec3(-_size.x, _size.y, _size.z));
    _buffer.vertex(glm::vec3(-_size.x, _size.y, -_size.z));

    _buffer.vertex(glm::vec3(-_size.x, -_size.y, _size.z));
    _buffer.vertex(glm::vec3(-_size.x, -_size.y, -_size.z));


    _buffer.vertex(glm::vec3(_size.x,  _size.y, _size.z));
    _buffer.vertex(glm::vec3(_size.x, -_size.y, _size.z));

    _buffer.vertex(glm::vec3(-_size.x,  _size.y, _size.z));
    _buffer.vertex(glm::vec3(-_size.x, -_size.y, _size.z));

    _buffer.vertex(glm::vec3(_size.x,  _size.y, -_size.z));
    _buffer.vertex(glm::vec3(_size.x, -_size.y, -_size.z));

    _buffer.vertex(glm::vec3(-_size.x,  _size.y, -_size.z));
    _buffer.vertex(glm::vec3(-_size.x, -_size.y, -_size.z));


    _buffer.vertex(glm::vec3( _size.x, _size.y, _size.z));
    _buffer.vertex(glm::vec3(-_size.x, _size.y, _size.z));

    _buffer.vertex(glm::vec3( _size.x, -_size.y, _size.z));
    _buffer.vertex(glm::vec3(-_size.x, -_size.y, _size.z));

    _buffer.vertex(glm::vec3( _size.x, _size.y, -_size.z));
    _buffer.vertex(glm::vec3(-_size.x, _size.y, -_size.z));

    _buffer.vertex(glm::vec3( _size.x, -_size.y, -_size.z));
    _buffer.vertex(glm::vec3(-_size.x, -_size.y, -_size.z));

    _buffer.setup(&_shader);
}

Game &Game::Instantiate(int argc, char *argv[])
{
    static IcyFebruary game(argc, argv);

    return game;
}

IcyFebruary::IcyFebruary(int argc, char *argv[])
    : _showPhysicsDebug(true), _create(nullptr)
{
    System::IO::FileInfo exe(argv[0]);
    _settingsDir = exe.Directory().FullName();
}

unsigned int IcyFebruary::uploadTexture(std::string const &filename)
{
    int x, y, comp;
    auto pixels = stbi_load(filename.c_str(), &x, &y, &comp, 3);
    if (pixels == nullptr)
    {
        return 0;
    }

    unsigned int texture = 0;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, comp == 4 ? GL_RGBA : GL_RGB, x, y, 0, comp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

bool IcyFebruary::Setup()
{
    _camOffset[0] = 0.0f;
    _camOffset[1] = 30.0f;
    _camOffset[2] = 10.0f;

    _userInput.ReadKeyMappings(System::IO::Path::Combine(_settingsDir, KEYMAP_FILE));

    ImGuiIO &io = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\tahoma.ttf", 18.0f, NULL);

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowPadding = ImVec2(15.0f, 15.0f);
    style.ItemSpacing = ImVec2(15.0f, 15.0f);
    style.ItemInnerSpacing = ImVec2(16.0f, 8.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.1f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.1f, 0.2f, 0.2f, 1.0f);

    glClearColor(0.56f, 0.7f, 0.67f, 1.0f);

    // Setting up the shaders
    _boxShader.compileDefaultShader();

    //    CreationObject::_shader.compileDefaultShader();

    _floorObject = PhysicsObjectBuilder(_physics)
                       .Box(glm::vec3(40.0f, 20.0f, 0.1f))
                       .Mass(0.0f)
                       .Build();

    _characterObject = PhysicsObjectBuilder(_physics)
                           .Capsule(1.0f, 2.2f, glm::vec3(0.0f, -1.5f, 0.0f))
                           .InitialPosition(glm::vec3(0.0f, 0.0f, 1.0f))
                           .Mass(10.0f)
                           .BuildCharacter();

    PhysicsObjectBuilder(_physics)
        .Box(glm::vec3(2.0f, 10.0f, 2.0f))
        .InitialPosition(glm::vec3(-5.0f, 0.0f, 1.0f))
        .Mass(0.0f)
        .Build();

    _character.loadObj("../02-icy-february/assets/hjmediastudios_-_office_drone.obj", "../02-icy-february/assets/", "Drone_Skin_Drone")
        .setup(&_boxShader);

    _fridge.loadObj("../02-icy-february/assets/fridge.obj", "../02-icy-february/assets/", "Cube")
        .setup(&_boxShader);

    _physics.InitDebugDraw();

    CreationObject::_shader.compileDefaultShader();

    return true;
}

void IcyFebruary::Resize(int width, int height)
{
    _width = width;
    _height = height;

    // Calculate the projection and view matrix
    _proj = glm::perspective(glm::radians(90.0f), float(width) / float(height), 0.1f, 4096.0f);
    _view = glm::lookAt(_pos + glm::vec3(5.0f, 5.0f, 0.0f), _pos, glm::vec3(0.0f, 0.0f, 1.0f));
}

void IcyFebruary::Update(int dt)
{
    if (_menuMode != MenuModes::NoMenu)
    {
        return;
    }

    if (_userInput.ActionState(UserInputActions::SpeedUp))
    {
        _characterObject->Forward(1.0f);
    }
    else if (_userInput.ActionState(UserInputActions::SpeedDown))
    {
        _characterObject->Forward(-1.0f);
    }
    else
    {
        _characterObject->Forward(0.0f);
    }

    if (_userInput.ActionState(UserInputActions::SteerLeft))
    {
        _characterObject->Left(-1.0f);
    }
    else if (_userInput.ActionState(UserInputActions::SteerRight))
    {
        _characterObject->Left(1.0f);
    }
    else
    {
        _characterObject->Left(0.0f);
    }

    static bool isJumping = false;
    if (isJumping == false && _userInput.ActionState(UserInputActions::Jump))
    {
        _characterObject->Jump();
        isJumping = true;
    }
    else
    {
        isJumping = _characterObject->IsJumping();
    }

    _characterObject->Update();

    _physics.Step(dt / 1000.0f);

    _pos.x = _characterObject->getMatrix()[3].x;
    _view = glm::lookAt(_pos + glm::vec3(_camOffset[0], _camOffset[1], _camOffset[2]), _pos, glm::vec3(0.0f, 0.0f, 1.0f));
}

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void IcyFebruary::Render()
{
    glViewport(0, 0, _width, _height);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
    {
        CapabilityGuard depthTest(GL_DEPTH_TEST, true);
        // Select shader
        _boxShader.use();

        glFrontFace(GL_CW);
        _boxShader.setupMatrices(_proj, _view, _characterObject->getMatrix());
        _character.render();
        _boxShader.setupMatrices(_proj, _view, glm::mat4(1.0f));
        _fridge.render();
        glFrontFace(GL_CCW);
    }

    if (_showPhysicsDebug)
    {
        CapabilityGuard depthTest(GL_DEPTH_TEST, false);
        _physics.DebugDraw(_proj, _view);
    }

    if (_create != nullptr)
    {
        _create->_shader.use();
        _create->_shader.setupMatrices(_proj, _view, glm::translate(glm::mat4(1.0f), _create->_pos));

        glDisable(GL_DEPTH_TEST);
        _create->_shader.setupColor(glm::vec4(0.0f));
        _create->_buffer.render();

        glEnable(GL_DEPTH_TEST);
        _create->_shader.setupColor(glm::vec4(255.0f));
        _create->_buffer.render();
    }
}

void IcyFebruary::RenderUi()
{
    static bool show_gui = true;

    float panelWidth = _width > 1024 ? 512 : 275;

    if (_menuMode == MenuModes::NoMenu)
    {
        ImGui::Begin("Settings", &show_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        {
            ImGui::SetWindowPos(ImVec2(0, 0));
            //            ImGui::SetWindowSize(ImVec2(160, 64));
            ImGui::SetWindowSize(ImVec2(panelWidth, _height));
            if (ImGui::Button("Pause", ImVec2(120, 36)))
            {
                _menuMode = MenuModes::MainMenu;
            }
            ImGui::SliderFloat("Cam X", &(_camOffset[0]), -30.0f, 30.0f);
            ImGui::SliderFloat("Cam Y", &(_camOffset[1]), -30.0f, 30.0f);
            ImGui::SliderFloat("Cam Z", &(_camOffset[2]), -30.0f, 30.0f);
            ImGui::Checkbox("Show Physics Debug", &_showPhysicsDebug);

            if (_create != nullptr)
            {
                bool creatChanged = false;
                ImGui::Text("Create Object");
                ImGui::SliderFloat("pos X", &(_create->_pos[0]), -40.0f, 40.0f);
                ImGui::SliderFloat("pos Y", &(_create->_pos[1]), -30.0f, 30.0f);
                ImGui::SliderFloat("pos Z", &(_create->_pos[2]), -20.0f, 80.0f);
                if (ImGui::SliderFloat("size X", &(_create->_size[0]), 0.1f, 50.0f)) creatChanged = true;
                if (ImGui::SliderFloat("size Y", &(_create->_size[1]), 0.1f, 50.0f)) creatChanged = true;
                if (ImGui::SliderFloat("size Z", &(_create->_size[2]), 0.1f, 50.0f)) creatChanged = true;

                if (creatChanged)
                {
                    _create->rebuildBuffer();
                }
                else
                {
                    if (ImGui::Button("Create"))
                    {
                        PhysicsObjectBuilder(_physics)
                            .Box(_create->_size * 2.0f)
                            .InitialPosition(_create->_pos)
                            .Mass(0.0f)
                            .Build();

                        _createdObjects.push_back(_create);
                        _create = nullptr;
                    }
                }
            }
            else
            {
                if (ImGui::Button("New object"))
                {
                    _create = new CreationObject();
                    _create->_size = glm::vec3(5.0f);
                    _create->rebuildBuffer();
                }
            }

            ImGui::End();
        }
        return;
    }
    else
    {
        ImGui::Begin("Settings", &show_gui, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        {
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(panelWidth, _height));

            if (_menuMode == MenuModes::MainMenu)
            {
                if (ImGui::Button("Play!", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::NoMenu;
                }
                if (ImGui::Button("Change keys", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::KeyMappingMenu;
                }

                ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            }
            if (_menuMode == MenuModes::KeyMappingMenu)
            {
                if (ImGui::Button("Back", ImVec2(100, 36)))
                {
                    _menuMode = MenuModes::MainMenu;
                    _userInput.WriteKeyMappings(System::IO::Path::Combine(_settingsDir, KEYMAP_FILE));
                }
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120);

                ImGui::Text("Action");
                ImGui::NextColumn();
                ImGui::Text("Bound keys");
                ImGui::NextColumn();

                ImGui::Separator();

                for (int i = 0; i < int(UserInputActions::Count); ++i)
                {
                    if (ImGui::Button(UserInputActionNames[i], ImVec2(100, 36)))
                    {
                        _userInput.StartMappingAction((UserInputActions)i);
                    }
                    ImGui::NextColumn();

                    auto mappedEvents = _userInput.GetMappedActionEvents((UserInputActions)i);
                    bool first = true;
                    for (auto e : mappedEvents)
                    {
                        if (!first)
                        {
                            ImGui::Text("or");
                            ImGui::SameLine();
                        }
                        ImGui::Text(e.toString());
                        ImGui::SameLine();
                        first = false;
                    }
                    if (mappedEvents.size() == 0)
                    {
                        ImGui::Text("<unbound>");
                    }
                    ImGui::NextColumn();
                }
            }

            ImGui::End();
        }
    }
}

void IcyFebruary::Destroy()
{
}
