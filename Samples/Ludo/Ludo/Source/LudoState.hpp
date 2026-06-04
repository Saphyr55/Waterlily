#pragma once

namespace Wl
{

    namespace LudoState
    {

        enum class Enum
        {
            Paused,
            Running,
        };

        inline bool IsPaused(Enum state)
        {
            return state == Enum::Paused;
        }

        inline bool IsRunning(Enum state)
        {
            return state == Enum::Paused;
        }

        inline Enum TooglePause(Enum state)
        {
            if (state == Enum::Running)
            {
                return Enum::Paused;
            }

            if (state == Enum::Paused)
            {
                return Enum::Running;
            }

            return state;
        }

    }// namespace LudoState

}// namespace Wl