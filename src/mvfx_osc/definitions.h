/********************************************************************************

    microVision FX OSC Controller - microEOS

 ********************************************************************************
    version history

    yyyy-mm-dd    Vxx     Who             Comment

    2025-07-16    1.0.1   John Godman     Write ADC interface and OSC routines.
    
    2025-07-18    1.0.2   John Godman     Add pins for Teensy 4.0 and write
                                          LED routines.

    2025-07-19    1.0.3   John Godman     Bugfixes.

 ********************************************************************************/

/****************************************
    pin def
    Teensy ver   4.1     4.0
****************************************/

// A lines for decoders

#define A0      2     //2
#define A1      3     //3
#define A2      4     //4

// D lines for buffer and ADC
#define D0      5     //5
#define D1      6     //6
#define D2      7     //7
#define D3      8     //8
#define D4      9     //9
#define D5      10    //10
#define D6      11    //11
#define D7      27    //14

// chip select; low to read
#define WR      12    //15    // keyboard chip enable
#define RD      24    //16    // fader ADC read
#define LEDWR   28    //17    // LED chip enable

// wheel
#define WHEEL_A 25    //18
#define WHEEL_B 26    //19

/****************************************
    look-up tables
****************************************/

const char *kybdLUT[8][7] = {
  { "softkey_6",    "softkey_5",    "softkey_4",  "softkey_3",      "softkey_2",  "softkey_1",  "effects" },
  { "",             "",             "macro",      "more_softkeys",  "load",       "blackout",   "" },
  { "stopback",     "go",           "park",       "patch",          "blind",      "live",       "focus_palette" },
  { "sneak",        "\\",           "-",          "thru",           "+",          "record",     "color_palette" },
  { "select_last",  "out",          "9",          "8",              "7",          "cue",        "" },
  { "enter",        "full",         "6",          "5",              "4",          "delete",     "" },
  { "about",        "@",            "3",          "2",              "1",          "group",      "" },
  { "time",         "cueonlytrack", ".",          "0",              "clear_cmd",  "shift",      "" }
};

const char *kybd_fn_LUT[8][7] = {
  { "fader_6",      "fader_5",      "fader_4",    "fader_3",        "fader_2",    "fader_1",    "fader_control" },
  { "",             "",             "learn",      "",               "",           "",           "" },
  { "",             "",             "undo",       "recall_from",    "copy_to",    "label",      "beam_palette" },
  { "home",         "rem_dim",      "last",       "",               "next",       "update",     "preset" },
  { "select_active","",             "displays",   "",               "",           "go_to_cue",  "" },
  { "",             "",             "select",     "arrow_up",       "escape",     "record_only","" },
  { "help",         "",             "arrow_right","arrow_down",     "arrow_left", "sub",        "" },
  { "delay",        "level",        "",           "",               "",           "",           "" }
};

const char *kybdLUT_fn_shift_LUT[8][7] = {
  { "",             "",             "",           "",               "",           "",           "" },
  { "",             "",             "",           "",               "",           "",           "" },
  { "",             "",             "capture",    "block",          "assert",     "query",      "intensity_palette" },
  { "",             "",             "",           "",               "",           "",           "path" },
  { "",             "",             "data",       "expand",         "format",     "part",       "" },
  { "",             "",             "page_mode",  "flexi_all",      "tab",        "",           "" },
  { "",             "",             "",           "",               "",           "",           "" },
  { "",             "level",        "",           "",               "",           "",           "" }
};
  
const char *faderLUT[3][8] = {
  { "D", "C", "0/2", "", "", "", "", "" },
  { "1/5", "1/6", "0/1", "1/4", "1/3", "99/1", "1/2", "1/1" },
  { "", "", "", "", "", "", "", "" } // effects returns; not used rn
};

// LED macros
#define LED_BO    0
#define LED_REC   2
#define LED_TIME  3
#define LED_SLASH 4
#define LED_SHIFT 5
#define LED_A_GO  6
#define LED_C_GO  7
