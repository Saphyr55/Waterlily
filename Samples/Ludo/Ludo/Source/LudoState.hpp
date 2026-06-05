#pragma once

namespace Wl
{

    namespace LudoState
    {

        enum class Type
        {
            Paused,
            Running,
        };

        inline bool IsPaused(Type state)
        {
            return state == Type::Paused;
        }

        inline bool IsRunning(Type state)
        {
            return state == Type::Paused;
        }

        inline Type TooglePause(Type state)
        {
            if (state == Type::Running)
            {
                return Type::Paused;
            }

            if (state == Type::Paused)
            {
                return Type::Running;
            }

            return state;
        }

    }// namespace LudoState

}// namespace Wl