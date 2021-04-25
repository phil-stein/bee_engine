#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "global.h"
#include "window.h"


// "keystate" mapps directly to glfws key definitions
enum keystate { KEY_RELEASED, KEY_PRESS };
typedef enum keystate keystate;

// "key" mapps directly to glfws key definitions
enum key
{
    KEY_Unknown = -1,
    KEY_Space = 32,
    KEY_Apostrophe = 39,
    KEY_Comma = 44,
    KEY_Minus = 45,
    KEY_Period = 46,
    KEY_Slash = 47,
    KEY_Alpha0 = 48,
    KEY_Alpha1 = 49,
    KEY_Alpha2 = 50,
    KEY_Alpha3 = 51,
    KEY_Alpha4 = 52,
    KEY_Alpha5 = 53,
    KEY_Alpha6 = 54,
    KEY_Alpha7 = 55,
    KEY_Alpha8 = 56,
    KEY_Alpha9 = 57,
    KEY_SemiColon = 59,
    KEY_Equal = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LeftBracket = 91,
    KEY_Backslash = 92,
    KEY_RightBracket = 93,
    KEY_GraveAccent = 96,
    KEY_World1 = 161,
    KEY_World2 = 162,
    KEY_Escape = 256,
    KEY_Enter = 257,
    KEY_Tab = 258,
    KEY_Backspace = 259,
    KEY_Insert = 260,
    KEY_Delete = 261,
    KEY_RightArrow = 262,
    KEY_LeftArrow = 263,
    KEY_DownArrow = 264,
    KEY_UpArrow = 265,
    KEY_PageUp = 266,
    KEY_PageDown = 267,
    KEY_Home = 268,
    KEY_End = 269,
    KEY_CapsLock = 280,
    KEY_ScrollLock = 281,
    KEY_NumLock = 282,
    KEY_PrintScreen = 283,
    KEY_Pause = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_F13 = 302,
    KEY_F14 = 303,
    KEY_F15 = 304,
    KEY_F16 = 305,
    KEY_F17 = 306,
    KEY_F18 = 307,
    KEY_F19 = 308,
    KEY_F20 = 309,
    KEY_F21 = 310,
    KEY_F22 = 311,
    KEY_F23 = 312,
    KEY_F24 = 313,
    KEY_F25 = 314,
    KEY_Numpad0 = 320,
    KEY_Numpad1 = 321,
    KEY_Numpad2 = 322,
    KEY_Numpad3 = 323,
    KEY_Numpad4 = 324,
    KEY_Numpad5 = 325,
    KEY_Numpad6 = 326,
    KEY_Numpad7 = 327,
    KEY_Numpad8 = 328,
    KEY_Numpad9 = 329,
    KEY_NumpadDecimal = 330,
    KEY_NumpadDivide = 331,
    KEY_NumpadMultiply = 332,
    KEY_NumpadSubtract = 333,
    KEY_NumpadAdd = 334,
    KEY_NumpadEnter = 335,
    KEY_NumpadEqual = 336,
    KEY_LeftShift = 340,
    KEY_LeftControl = 341,
    KEY_LeftAlt = 342,

    /// <summary> The key with the Mac, Windows, etc. Symbol. </summary>
    KEY_LeftSuper = 343,
    /// <summary> 
    /// The key with the Mac, Windows, etc. Symbol. 
    /// <para>Same as <see cref="LeftSuper" />.</para>
    /// </summary>
    KEY_LeftWinMacSymbol = KEY_LeftSuper,

    KEY_RightShift = 344,
    KEY_RightControl = 345,
    KEY_RightAlt = 346,

    /// <summary> The key with the Mac, Windows, etc. Symbol. </summary>
    KEY_RightSuper = 347,
    /// <summary> 
    /// The key with the Mac, Windows, etc. Symbol. 
    /// <para>Same as <see cref="RightSuper" />.</para>
    /// </summary>
    KEY_RightWinMacSymbol = KEY_RightSuper,

    KEY_Menu = 348
};
typedef enum key key;

enum mouse_btn
{
    /// <summary>
    ///     Mouse button 1.
    ///     <para>Same as <see cref="Left" />.</para>
    /// </summary>
    Button1 = 0,

    /// <summary>
    ///     Mouse button 2.
    ///     <para>Same as <see cref="Right" />.</para>
    /// </summary>
    Button2 = 1,

    /// <summary>
    ///     Mouse button 3.
    ///     <para>Same as <see cref="Middle" />.</para>
    /// </summary>
    Button3 = 2,

    /// <summary>
    ///     Mouse button 4.
    /// </summary>
    Button4 = 3,

    /// <summary>
    ///     Mouse button 4.
    /// </summary>
    Button5 = 4,

    /// <summary>
    ///     Mouse button 5.
    /// </summary>
    Button6 = 5,

    /// <summary>
    ///     Mouse button 6.
    /// </summary>
    Button7 = 6,

    /// <summary>
    ///     Mouse button 7.
    /// </summary>
    Button8 = 7,

    /// <summary>
    ///     The left mouse button.
    ///     <para>Same as <see cref="Button1" />.</para>
    /// </summary>
    Left = Button1,

    /// <summary>
    ///     The right mouse button.
    ///     <para>Same as <see cref="Button2" />.</para>
    /// </summary>
    Right = Button2,

    /// <summary>
    ///     The middle mouse button.
    ///     <para>Same as <see cref="Button3" />.</para>
    /// </summary>
    Middle = Button3
};
typedef enum mouse_btn mouse_btn;


void input_init();
void input_update();

// returns the state of the key [KEY_PRESS, KEY_RELEASED]
keystate get_key_state(key _key);

// returns true if the key is pressed, false it it is released
bee_bool is_key_down(key _key);

// returns true if the key is released, false it it is pressed
bee_bool is_key_released(key _key);

// check whether a key is pressed and not held
// returns true the first frame the Key is held/pressed
bee_bool is_key_pressed(key _key);

// sets the key states for the last frame
void key_callback(GLFWwindow* window, key _key, int scancode, keystate state, int mods);

// gets the state of the key last frame 
bee_bool get_last_key_state(key _key);

#endif
