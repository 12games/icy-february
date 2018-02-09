#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

enum class UserInputActions
{
    StartEngine,
    StopEngine,
    SpeedUp,
    SpeedDown,
    SteerLeft,
    SteerRight,
    Brake,

    Count
};

static const char *UserInputActionNames[] = {
    "StartEngine",
    "StopEngine",
    "SpeedUp",
    "SpeedDown",
    "SteerLeft",
    "SteerRight",
    "Brake",
};

struct UserInputEvent
{
    unsigned int source;
    int key;

    char const *toString();
};

class UserInput
{
    std::map<UserInputActions, bool> _actionStates;
    std::map<UserInputEvent, UserInputActions> _stateMapping;

public:
    bool _mappingMode;
    UserInputActions _actionToMap;

    void StartMappingAction(UserInputActions action);
    std::vector<UserInputEvent> GetMappedActionEvents(UserInputActions action);

    void ProcessEvent(UserInputEvent const &event, bool state);

    bool ActionState(UserInputActions action);

    void ReadKeyMappings(std::string const &filename);
    void WriteKeyMappings(std::string const &filename);
};

class Game
{
public:
    virtual ~Game() {}

    virtual bool Setup() = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void Update(int dt) = 0;
    virtual void Render() = 0;
    virtual void RenderUi() = 0;
    virtual void Destroy() = 0;

    static Game &Instantiate(int argc, char *argv[]);

    int _width, _height;
    UserInput _userInput;
};

#endif // GAME_H
