#pragma once
#include <Windows.h>
#include <stdint.h>
#include <d3d11.h>
#include <directxmath.h>

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef bool		bool8;

typedef float		real32;
typedef double		real64;

union ivec2
{
	struct
	{
		int32 x;
		int32 y;
	};
	int32 coords[2];
};

union ivec3
{
	struct
	{
		int32 x;
		int32 y;
		int32 z;
	};
	int32 coords[3];
};

union ivec4
{
	struct
	{
		int32 x;
		int32 y;
		int32 z;
		int32 w;
	};
	int32 coords[4];
};

union fvec2
{
	struct
	{
		real32 x;
		real32 y;
	};
	real32 coords[2];
};

union fvec3
{
	struct
	{
		real32 x;
		real32 y;
		real32 z;
	};

	real32 coords[3];
};

union fvec4
{
	struct
	{
		real32 x;
		real32 y;
		real32 z;
		real32 w;
	};
	real32 coords[4];
};

struct vertex
{
	fvec3 position;
	fvec2 texcoords;
	fvec3 normal;
};

struct OBJ_Face
{
	ivec3 vertices[3];
};

struct OBJ_Model
{
	enum
	{
		MTLLIB_PRESENT = 1,
	};
	uint64 vertex_positions_count;
	uint64 vertex_texcoords_count;
	uint64 vertex_normals_count;
	uint64 face_count;

	fvec3* vertex_positions;
	fvec3* vertex_texcoords;
	fvec3* vertex_normals;
	OBJ_Face* faces;
	uint32 flags;
};

struct Mesh
{
	enum
	{
		TRIANGULATED = 1,
		BACKFACE_CULLING = 2,
		TRIANGLE_LIST = 4,
		TRIANGLE_STRIP = 8,
	};
	uint64 vertex_count;
	vertex* vertices;
	uint32 flags;
};

struct Entity
{
	Mesh* mesh;
};

struct World
{
	Entity* entities;
	uint64 entity_count;
};

struct Display_Properties
{
	uint32 horizontal_pixel_count;
	uint32 vertical_pixel_count;
	uint32 refresh_rate;
};

struct Window_Properties
{
	uint32 window_width;
	uint32 window_height;
	HWND window_handle;
};

struct Mouse_State
{
	uint8 button_state_info[5];
	int32 cursor_position_x;
	int32 cursor_position_y;
};

enum MOUSEBUTTON
{
	LEFT_MOUSE_BUTTON,
	RIGHT_MOUSE_BUTTON,
	MIDDLE_MOUSE_BUTTON,
	X1_MOUSE_BUTTON,
	X2_MOUSE_BUTTON
};

#ifdef win32_compilation
// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
enum KEYBOARD_KEYCODE
{
	// editing
	KC_BACKSPACE = VK_BACK,
	KC_TAB = VK_TAB,
	KC_ENTER = VK_RETURN,
	KC_SHIFT = VK_SHIFT,
	KC_CONTROL = VK_CONTROL,
	KC_ALT = VK_MENU,
	KC_PAUSE = VK_PAUSE,
	KC_CAPSLOCK = VK_CAPITAL,
	KC_ESCAPE = VK_ESCAPE,
	KC_SPACE = VK_SPACE,
	KC_PAGEUP = VK_PRIOR,
	KC_PAGEDOWN = VK_NEXT,
	KC_END = VK_END,
	KC_HOME = VK_HOME,
	KC_LEFTARROW = VK_LEFT,
	KC_UPARROW = VK_UP,
	KC_RIGHTARROW = VK_RIGHT,
	KC_DOWNARROW = VK_DOWN,
	KC_PRINT = VK_PRINT,
	KC_PRINTSCR = VK_SNAPSHOT,
	KC_INSERT = VK_INSERT,
	KC_DELETE = VK_DELETE,
	KC_LEFTWIN = VK_LWIN,
	KC_RIGHTWIN = VK_RWIN,
	KC_SLEEP = VK_SLEEP,
	KC_NUMLOCK = VK_NUMLOCK,
	KC_SCROLL = VK_SCROLL,
	KC_LEFTSHIFT = VK_LSHIFT,
	KC_RIGHTSHIFT = VK_RSHIFT,
	KC_LCONTROL = VK_LCONTROL,
	KC_RCONTROL = VK_RCONTROL,
	// TODO
	// KC_ALTGR = ?;

	// signs
	KC_SEMICOLON = VK_OEM_1,
	KC_PLUS = VK_OEM_PLUS,
	KC_MINUS = VK_OEM_MINUS,
	KC_COMMA = VK_OEM_COMMA,
	KC_PERIOD = VK_OEM_PERIOD,
	KC_FORWSLASH = VK_OEM_2,
	KC_BACKSLASH = VK_OEM_5,
	KC_GRAVEACCENT = VK_OEM_3,
	KC_BRAC_OPEN = VK_OEM_4,
	KC_BRAC_CLOSED = VK_OEM_6,
	KC_QUOTE = VK_OEM_7,

	// browser
	KC_BRWSR_BACK = VK_BROWSER_BACK,
	KC_BRWSR_FORW = VK_BROWSER_FORWARD,
	KC_BRWSR_REFR = VK_BROWSER_REFRESH,
	KC_BRWSR_STOP = VK_BROWSER_STOP,
	KC_BRWSR_SEAR = VK_BROWSER_SEARCH,
	KC_BRWSR_FAV = VK_BROWSER_FAVORITES,
	KC_BRWSR_HOME = VK_BROWSER_HOME,

	// volume and media
	KC_VOLMUTE = VK_VOLUME_MUTE,
	KC_VOLDOWN = VK_VOLUME_DOWN,
	KC_VOLUP = VK_VOLUME_UP,
	KC_NEXTTRACK = VK_MEDIA_NEXT_TRACK,
	KC_PREVTRACK = VK_MEDIA_PREV_TRACK,
	KC_MEDIASTOP = VK_MEDIA_STOP,
	KC_PLAYPAUSE = VK_MEDIA_PLAY_PAUSE,

	// alphanumerical
	KC_0 = 0x30,
	KC_1 = 0x31,
	KC_2 = 0x32,
	KC_3 = 0x33,
	KC_4 = 0x34,
	KC_5 = 0x35,
	KC_6 = 0x36,
	KC_7 = 0x37,
	KC_8 = 0x38,
	KC_9 = 0x39,
	KC_A = 0x41,
	KC_B = 0x42,
	KC_C = 0x43,
	KC_D = 0x44,
	KC_E = 0x45,
	KC_F = 0x46,
	KC_G = 0x47,
	KC_H = 0x48,
	KC_I = 0x49,
	KC_J = 0x4A,
	KC_K = 0x4B,
	KC_L = 0x4C,
	KC_M = 0x4D,
	KC_N = 0x4E,
	KC_O = 0x4F,
	KC_P = 0x50,
	KC_Q = 0x51,
	KC_R = 0x52,
	KC_S = 0x53,
	KC_T = 0x54,
	KC_U = 0x55,
	KC_V = 0x56,
	KC_W = 0x57,
	KC_X = 0x58,
	KC_Y = 0x59,
	KC_Z = 0x5A,

	// numpad
	KC_NUMPAD_0 = VK_NUMPAD0,
	KC_NUMPAD_1 = VK_NUMPAD1,
	KC_NUMPAD_2 = VK_NUMPAD2,
	KC_NUMPAD_3 = VK_NUMPAD3,
	KC_NUMPAD_4 = VK_NUMPAD3,
	KC_NUMPAD_5 = VK_NUMPAD5,
	KC_NUMPAD_6 = VK_NUMPAD6,
	KC_NUMPAD_7 = VK_NUMPAD7,
	KC_NUMPAD_8 = VK_NUMPAD8,
	KC_NUMPAD_9 = VK_NUMPAD9,
	KC_NUMPAD_MULTIPLY = VK_MULTIPLY,
	KC_NUMPAD_ADD = VK_ADD,
	KC_NUMPAD_SEPARATOR = VK_SEPARATOR,
	KC_NUMPAD_SUBSTRACT = VK_SUBTRACT,
	KC_NUMPAD_DECIMAL = VK_DECIMAL,
	KC_NUMPAD_DIVIDE = VK_DIVIDE,

	// function
	KC_F1 = VK_F1,
	KC_F2 = VK_F2,
	KC_F3 = VK_F3,
	KC_F4 = VK_F4,
	KC_F5 = VK_F5,
	KC_F6 = VK_F6,
	KC_F7 = VK_F7,
	KC_F8 = VK_F8,
	KC_F9 = VK_F9,
	KC_F10 = VK_F10,
	KC_F11 = VK_F11,
	KC_F12 = VK_F12,
	KC_F13 = VK_F13,
	KC_F14 = VK_F14,
	KC_F15 = VK_F15,
	KC_F16 = VK_F16,
	KC_F17 = VK_F17,
	KC_F18 = VK_F18,
	KC_F19 = VK_F19,
	KC_F20 = VK_F20,
	KC_F21 = VK_F21,
	KC_F22 = VK_F22,
	KC_F23 = VK_F23,
	KC_F24 = VK_F24
};
// TODO include linux key mapping
#endif
