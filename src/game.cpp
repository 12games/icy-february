#include "game.h"
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>

static std::mutex mappingsMutex;

bool operator < (UserInputEvent const &a, UserInputEvent const &b)
{
    if (a.source == b.source)
    {
        return a.key < b.key;
    }

    return a.source < b.source;
}

void UserInput::StartMappingAction(UserInputActions action)
{
    _mappingMode = true;
    _actionToMap = action;
}

std::vector<UserInputEvent> UserInput::GetMappedActionEvents(UserInputActions action)
{
    std::vector<UserInputEvent> result;

    for (auto pair : _stateMapping)
    {
        if (pair.second == action)
        {
            result.push_back(pair.first);
        }
    }

    return result;
}

void UserInput::ProcessEvent(UserInputEvent const &event, bool state)
{
    if (_mappingMode)
    {
        if (state)
        {
            // We only change mappings on the release of a button
            return;
        }

        std::lock_guard<std::mutex> lock(mappingsMutex);

        auto mapping = _stateMapping.find(event);
        if (mapping == _stateMapping.end())
        {
            _stateMapping.insert(std::make_pair(event, _actionToMap));
        }
        else
        {
            mapping->second = _actionToMap;
        }

        _mappingMode = false;
        return;
    }

    auto mapping = _stateMapping.find(event);
    if (mapping == _stateMapping.end())
    {
        return;
    }

    if (_actionStates.find(mapping->second) == _actionStates.end())
    {
        std::lock_guard<std::mutex> lock(mappingsMutex);

        _actionStates.insert(std::make_pair(mapping->second, false));
    }

    _actionStates[mapping->second] = state;
}

bool UserInput::ActionState(UserInputActions action)
{
    if (_actionStates.find(action) == _actionStates.end())
    {
        _actionStates.insert(std::make_pair(action, false));
    }

    return _actionStates[action];
}

void UserInput::ReadKeyMappings(std::string const &filename)
{
    // we are threading this to make sure it will not freeze the menu or something
    std::thread t([this, filename]() {
        std::lock_guard<std::mutex> lock(mappingsMutex);

        std::ifstream infile(filename);

        if (!infile.is_open())
        {
            // TODO log this somewhere
            std::cerr << "could not open \"" << filename << "\" for reading" << std::endl;
            return;
        }

        _stateMapping.clear();

        std::string line;
        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::string action;
            unsigned int source;
            int key;
            iss >> action >> source >> key;
            UserInputActions enumAction;

            for (int i = 0; i < int(UserInputActions::Count); ++i)
            {
                if (action == UserInputActionNames[i])
                {
                    enumAction = (UserInputActions)i;
                }
            }

            UserInputEvent uie = { source, key };
            _stateMapping.insert(std::make_pair(uie, enumAction));
        }

        infile.close();
    });

    t.detach();
}

void UserInput::WriteKeyMappings(std::string const &filename)
{
    // we are threading this to make sure it will not freeze the menu or something
    std::thread t([this, filename]() {
        std::lock_guard<std::mutex> lock(mappingsMutex);

        std::ofstream outfile(filename);

        if (!outfile.is_open())
        {
            // TODO log this somewhere
            std::cerr << "could not open \"" << filename << "\" for writing" << std::endl;
            return;
        }

        for (auto pair : _stateMapping)
        {
            outfile << UserInputActionNames[int(pair.second)] << " " << pair.first.source << " " << pair.first.key << std::endl;
        }

        outfile.close();
    });

    t.detach();
}
