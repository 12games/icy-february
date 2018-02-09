#ifndef CAPABILITYGUARD_H
#define CAPABILITYGUARD_H

#include <glad/glad.h>

class CapabilityGuard
{
    GLenum _cap;
    short _prevValue;

public:
    CapabilityGuard(GLenum cap, bool enable)
        : _cap(cap)
    {
        _prevValue = glIsEnabled(cap) ? 1 : 0;
        if (enable != (_prevValue == 1))
        {
            if (enable)
            {
                glEnable(_cap);
            }
            else
            {
                glDisable(_cap);
            }
        }
        else
        {
            _prevValue = -1;
        }
    }
    virtual ~CapabilityGuard()
    {
        if (_prevValue == 1)
        {
            glEnable(_cap);
        }
        else if (_prevValue == 0)
        {
            glDisable(_cap);
        }
    }
};

#endif // CAPABILITYGUARD_H
