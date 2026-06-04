#pragma once

#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Signals/Signal.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    struct MouseMove
    {
        float PosRelX;
        float PosRelY;
        float PosX;
        float PosY;
    };

    struct MouseWheel
    {
        float Delta;
    };

    enum class Button : uint8_t
    {
        Left = 1,
        Middle = 2,
        Right = 3,
        XButton1 = 4,
        XButton2 = 5
    };

    enum class VirtualKey : uint8_t
    {
        None = 0,

        Escape = 0x07,
        Back = 0x08,
        Tab = 0x09,
        Clear = 0x0C,
        Return = 0x0D,
        CapsLock = 0x14,
        Space = 0x20,
        PageUp = 0x21,
        PageDown = 0x22,
        End = 0x23,
        Home = 0x24,

        Period = 0xBE,
        Minus = 0xBD,
        Plus = 0xBB,
        Comma = 0xBC,

        Left = 0x25,
        Up = 0x26,
        Right = 0x27,
        Down = 0x28,

        Select = 0x29,
        Print = 0x2A,
        Snapshot = 0x2C,
        Insert = 0x2D,
        Delete = 0x2E,
        Help = 0x2F,

        Zero = 0x30,
        One = 0x31,
        Two = 0x32,
        Three = 0x33,
        Four = 0x34,
        Five = 0x35,
        Six = 0x36,
        Seven = 0x37,
        Eight = 0x38,
        Nine = 0x39,

        A = int32_t('A'),
        B = int32_t('B'),
        C = int32_t('C'),
        D = int32_t('D'),
        E = int32_t('E'),
        F = int32_t('F'),
        G = int32_t('G'),
        H = int32_t('H'),
        I = int32_t('I'),
        J = int32_t('J'),
        K = int32_t('K'),
        L = int32_t('L'),
        M = int32_t('M'),
        N = int32_t('N'),
        O = int32_t('O'),
        P = int32_t('P'),
        Q = int32_t('Q'),
        R = int32_t('R'),
        S = int32_t('S'),
        T = int32_t('T'),
        U = int32_t('U'),
        V = int32_t('V'),
        W = int32_t('W'),
        X = int32_t('X'),
        Y = int32_t('Y'),
        Z = int32_t('Z'),

        F1 = 0x70,
        F2 = 0x71,
        F3 = 0x72,
        F4 = 0x73,
        F5 = 0x74,
        F6 = 0x75,
        F7 = 0x76,
        F8 = 0x77,
        F9 = 0x78,
        F10 = 0x79,
        F11 = 0x7A,
        F12 = 0x7B,

        KP_NumLock = 0x90,
        KP_Divide = 0x6F,
        KP_Multiply = 0x6A,
        KP_Plus = 0x6B,
        KP_Minus = 0x6D,
        KP_Enter = 110,
        KP_1 = 0x61,
        KP_2 = 0x62,
        KP_3 = 0x63,
        KP_4 = 0x64,
        KP_5 = 0x65,
        KP_6 = 0x66,
        KP_7 = 0x67,
        KP_8 = 0x68,
        KP_9 = 0x69,
        KP_0 = 0x60,
        KP_Period = 97,
        KP_Separator = 0x6C,
        KP_Decimal = 0x6E,

        KApp = 0x5D,

        Mute = 0xAD,
        VolumeDown = 0xAE,
        VolumeUp = 0xAF,

        LeftShift = 0xA0,
        RightShift = 0xA1,

        LeftControl = 0xA2,
        RightControl = 0xA3,

        LeftAlt = 0xA4,
        RightAlt = 0xA5,
    };

    WL_CORE_API StringRef key_to_string(VirtualKey key);

    class WL_CORE_API Input
    {
    public:
        LSIGNAL_STATIC(OnKeyDown, VirtualKey);
        LSIGNAL_STATIC(OnKeyUp, VirtualKey);
        LSIGNAL_STATIC(OnKeyPressed, VirtualKey);
        LSIGNAL_STATIC(OnKeyRelease, VirtualKey);

        LSIGNAL_STATIC(OnMouseMove, const MouseMove&);
        LSIGNAL_STATIC(OnMouseWheel, const MouseWheel&);

        LSIGNAL_STATIC(OnButtonDown, Button);
        LSIGNAL_STATIC(OnButtonUp, Button);
        LSIGNAL_STATIC(OnButtonPressed, Button);
        LSIGNAL_STATIC(OnButtonRelease, Button);

        static bool KeyIsDown(VirtualKey key);
        static bool KeyIsUp(VirtualKey key);
        static bool KeyIsPressed(VirtualKey key);
        static bool KeyIsRelease(VirtualKey key);

        static bool ButtonIsDown(Button button);
        static bool ButtonIsUp(Button button);
        static bool ButtonIsPressed(Button button);
        static bool ButtonIsRelease(Button button);

        static HashSet<VirtualKey> s_KeysDow;
        static HashSet<VirtualKey> s_KeysUp;
        static HashSet<VirtualKey> s_KeysRelease;
        static HashSet<VirtualKey> s_KeysPressed;

        static HashSet<Button> s_ButtonsDown;
        static HashSet<Button> s_ButtonsUp;
        static HashSet<Button> s_ButtonsRelease;
        static HashSet<Button> s_ButtonsPressed;
    };

}// namespace Wl
