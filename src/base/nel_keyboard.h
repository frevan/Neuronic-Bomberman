#pragma once



namespace nel {

// type
enum {
	KEY_UP = 0,
	KEY_DOWN
};

// state
enum
{
	KEY_PRESSED = 0,
	KEY_RELEASED
};

// modifiers
enum {
	KEYMOD_LALT		= 1 << 0,
	KEYMOD_RALT		= 1 << 1,
	KEYMOD_LCTRL	= 1 << 2,
	KEYMOD_RCTRL	= 1 << 3,
	KEYMOD_LSHIFT	= 1 << 4,
	KEYMOD_RSHIFT	= 1 << 5,
	KEYMOD_NUMLOCK	= 1 << 6,
	KEYMOD_CAPSLOCK	= 1 << 7,
	KEYMOD_MODE		= 1 << 8,
	KEYMOD_LGUI		= 1 << 9,
	KEYMOD_RGUI		= 1 << 10
};

// virtual key constants
enum {
	VKEY_UNKNOWN = 0,
	VKEY_RETURN = '\r',
	VKEY_ESCAPE = '\033',
	VKEY_BACKSPACE = '\b',
	VKEY_TAB = '\t',
	VKEY_SPACE = ' ',
	VKEY_EXCLAIM = '!',
	VKEY_QUOTEDBL = '"',
	VKEY_HASH = '#',
	VKEY_PERCENT = '%',
	VKEY_DOLLAR = '$',
	VKEY_AMPERSAND = '&',
	VKEY_QUOTE = '\'',
	VKEY_LEFTPAREN = '(',
	VKEY_RIGHTPAREN = ')',
	VKEY_ASTERISK = '*',
	VKEY_PLUS = '+',
	VKEY_COMMA = ',',
	VKEY_MINUS = '-',
	VKEY_PERIOD = '.',
	VKEY_SLASH = '/',
	VKEY_0 = '0',
	VKEY_1 = '1',
	VKEY_2 = '2',
	VKEY_3 = '3',
	VKEY_4 = '4',
	VKEY_5 = '5',
	VKEY_6 = '6',
	VKEY_7 = '7',
	VKEY_8 = '8',
	VKEY_9 = '9',
	VKEY_COLON = ':',
	VKEY_SEMICOLON = ';',
	VKEY_LESS = '<',
	VKEY_EQUALS = '=',
	VKEY_GREATER = '>',
	VKEY_QUESTION = '?',
	VKEY_AT = '@',
	/*
	Skip uppercase letters
	*/
	VKEY_LEFTBRACKET = '[',
	VKEY_BACKSLASH = '\\',
	VKEY_RIGHTBRACKET = ']',
	VKEY_CARET = '^',
	VKEY_UNDERSCORE = '_',
	VKEY_BACKQUOTE = '`',
	VKEY_a = 'a',
	VKEY_b = 'b',
	VKEY_c = 'c',
	VKEY_d = 'd',
	VKEY_e = 'e',
	VKEY_f = 'f',
	VKEY_g = 'g',
	VKEY_h = 'h',
	VKEY_i = 'i',
	VKEY_j = 'j',
	VKEY_k = 'k',
	VKEY_l = 'l',
	VKEY_m = 'm',
	VKEY_n = 'n',
	VKEY_o = 'o',
	VKEY_p = 'p',
	VKEY_q = 'q',
	VKEY_r = 'r',
	VKEY_s = 's',
	VKEY_t = 't',
	VKEY_u = 'u',
	VKEY_v = 'v',
	VKEY_w = 'w',
	VKEY_x = 'x',
	VKEY_y = 'y',
	VKEY_z = 'z',
	
	VKEY_CAPSLOCK = 0x010000,

	VKEY_F1  = 0x010001,
	VKEY_F2  = 0x010002,
	VKEY_F3  = 0x010003,
	VKEY_F4  = 0x010004,
	VKEY_F5  = 0x010005,
	VKEY_F6  = 0x010006,
	VKEY_F7  = 0x010007,
	VKEY_F8  = 0x010008,
	VKEY_F9  = 0x010009,
	VKEY_F10 = 0x01000A,
	VKEY_F11 = 0x01000B,
	VKEY_F12 = 0x01000C,
	
	VKEY_PRINTSCREEN = 0x01000D,
	VKEY_SCROLLLOCK = 0x01000E,
	VKEY_PAUSE = 0x01000F,
	VKEY_INSERT = 0x010010,
	VKEY_HOME = 0x010011,
	VKEY_PAGEUP = 0x010012,
	VKEY_DELETE = '\177',
	VKEY_END = 0x010013,
	VKEY_PAGEDOWN = 0x010014,
	VKEY_RIGHT = 0x010015,
	VKEY_LEFT = 0x010016,
	VKEY_DOWN = 0x010017,
	VKEY_UP = 0x010018,
	
	VKEY_NUMLOCKCLEAR = 0x010019,
	VKEY_KP_DIVIDE = 0x01001A,
	VKEY_KP_MULTIPLY = 0x01001B,
	VKEY_KP_MINUS = 0x01001C,
	VKEY_KP_PLUS = 0x01001D,
	VKEY_KP_ENTER = 0x01001E,
	VKEY_KP_1 = 0x01001F,
	VKEY_KP_2 = 0x010020,
	VKEY_KP_3 = 0x010021,
	VKEY_KP_4 = 0x010022,
	VKEY_KP_5 = 0x010023,
	VKEY_KP_6 = 0x010024,
	VKEY_KP_7 = 0x010025,
	VKEY_KP_8 = 0x010026,
	VKEY_KP_9 = 0x010027,
	VKEY_KP_0 = 0x010028,
	VKEY_KP_PERIOD = 0x010029,
	
	VKEY_APPLICATION = 0x01002A,
	VKEY_POWER = 0x01002B,
	VKEY_KP_EQUALS = 0x01002C,
	VKEY_F13 = 0x01002D,
	VKEY_F14 = 0x01002E,
	VKEY_F15 = 0x01002F,
	VKEY_F16 = 0x010030,
	VKEY_F17 = 0x010031,
	VKEY_F18 = 0x010032,
	VKEY_F19 = 0x010033,
	VKEY_F20 = 0x010034,
	VKEY_F21 = 0x010035,
	VKEY_F22 = 0x010036,
	VKEY_F23 = 0x010037,
	VKEY_F24 = 0x010038,
	VKEY_EXECUTE = 0x010039,
	VKEY_HELP = 0x01003A,
	VKEY_MENU = 0x01003B,
	VKEY_SELECT = 0x01003C,
	VKEY_STOP = 0x01003D,
	VKEY_AGAIN = 0x01003E,
	VKEY_UNDO = 0x01003F,
	VKEY_CUT = 0x010040,
	VKEY_COPY = 0x010041,
	VKEY_PASTE = 0x010042,
	VKEY_FIND = 0x010043,
	VKEY_MUTE = 0x010044,
	VKEY_VOLUMEUP = 0x010045,
	VKEY_VOLUMEDOWN = 0x010046,
	VKEY_KP_COMMA = 0x010047,
	VKEY_KP_EQUALSAS400 = 0x010048,

	VKEY_ALTERASE = 0x010049,
	VKEY_SYSREQ = 0x01004A,
	VKEY_CANCEL = 0x01004B,
	VKEY_CLEAR = 0x01004C,
	VKEY_PRIOR = 0x01004D,
	VKEY_RETURN2 = 0x01004E,
	VKEY_SEPARATOR = 0x01004F,
	VKEY_OUT = 0x010050,
	VKEY_OPER = 0x010051,
	VKEY_CLEARAGAIN = 0x010052,
	VKEY_CRSEL = 0x010053,
	VKEY_EXSEL = 0x010054,
	
	VKEY_KP_00 = 0x010055,
	VKEY_KP_000 = 0x010056,
	VKEY_THOUSANDSSEPARATOR = 0x010057,
	VKEY_DECIMALSEPARATOR = 0x010058,
	VKEY_CURRENCYUNIT = 0x010059,
	VKEY_CURRENCYSUBUNIT = 0x01005A,
	VKEY_KP_LEFTPAREN = 0x01005B,
	VKEY_KP_RIGHTPAREN = 0x01005C,
	VKEY_KP_LEFTBRACE = 0x01005D,
	VKEY_KP_RIGHTBRACE = 0x01005E,
	VKEY_KP_TAB = 0x01005F,
	VKEY_KP_BACKSPACE = 0x010060,
	VKEY_KP_A = 0x010061,
	VKEY_KP_B = 0x010062,
	VKEY_KP_C = 0x010063,
	VKEY_KP_D = 0x010064,
	VKEY_KP_E = 0x010065,
	VKEY_KP_F = 0x010066,
	VKEY_KP_XOR = 0x010067,
	VKEY_KP_POWER = 0x010068,
	VKEY_KP_PERCENT = 0x010069,
	VKEY_KP_LESS = 0x01006A,
	VKEY_KP_GREATER = 0x01006B,
	VKEY_KP_AMPERSAND = 0x01006C,
	VKEY_KP_DBLAMPERSAND = 0x01006D,
	VKEY_KP_VERTICALBAR = 0x01006E,
	VKEY_KP_DBLVERTICALBAR = 0x01006F,
	VKEY_KP_COLON = 0x010070,
	VKEY_KP_HASH = 0x010071,
	VKEY_KP_SPACE = 0x010072,
	VKEY_KP_AT = 0x010073,
	VKEY_KP_EXCLAM = 0x010074,
	VKEY_KP_MEMSTORE = 0x010075,
	VKEY_KP_MEMRECALL = 0x010076,
	VKEY_KP_MEMCLEAR = 0x010077,
	VKEY_KP_MEMADD = 0x010078,
	VKEY_KP_MEMSUBTRACT = 0x010079,
	VKEY_KP_MEMMULTIPLY = 0x01007A,
	VKEY_KP_MEMDIVIDE = 0x01007B,
	VKEY_KP_PLUSMINUS = 0x01007C,
	VKEY_KP_CLEAR = 0x01007D,
	VKEY_KP_CLEARENTRY = 0x01007E,
	VKEY_KP_BINARY = 0x01007F,
	VKEY_KP_OCTAL = 0x010080,
	VKEY_KP_DECIMAL = 0x010081,
	VKEY_KP_HEXADECIMAL = 0x010082,

	VKEY_LCTRL = 0x010083,
	VKEY_LSHIFT = 0x010084,
	VKEY_LALT = 0x010085,
	VKEY_LGUI = 0x010086,
	VKEY_RCTRL = 0x010087,
	VKEY_RSHIFT = 0x010088,
	VKEY_RALT = 0x010089,
	VKEY_RGUI = 0x01008A,

	VKEY_MODE = 0x01008B,

	VKEY_AUDIONEXT = 0x01008C,
	VKEY_AUDIOPREV = 0x01008D,
	VKEY_AUDIOSTOP = 0x01008E,
	VKEY_AUDIOPLAY = 0x01008F,
	VKEY_AUDIOMUTE = 0x010090,
	VKEY_MEDIASELECT = 0x010091,
	VKEY_WWW = 0x010092,
	VKEY_MAIL = 0x010093,
	VKEY_CALCULATOR = 0x010094,
	VKEY_COMPUTER = 0x010095,
	VKEY_AC_SEARCH = 0x010096,
	VKEY_AC_HOME = 0x010097,
	VKEY_AC_BACK = 0x010098,
	VKEY_AC_FORWARD = 0x010099,
	VKEY_AC_STOP = 0x01009A,
	VKEY_AC_REFRESH = 0x01009B,
	VKEY_AC_BOOKMARKS = 0x01009C,
	
	VKEY_BRIGHTNESSDOWN = 0x01009D,
	VKEY_BRIGHTNESSUP = 0x01009E,
	VKEY_DISPLAYSWITCH = 0x01009F,
	VKEY_KBDILLUMTOGGLE = 0x0100A0,
	VKEY_KBDILLUMDOWN = 0x0100A1,
	VKEY_KBDILLUMUP = 0x0100A2,
	VKEY_EJECT = 0x0100A3,
	VKEY_SLEEP = 0x0100A4 
};

};	// namespace nel