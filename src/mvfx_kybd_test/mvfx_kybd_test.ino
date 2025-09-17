// MVFX KEYBOARD TEST
// for expression offline

// pin def

// 74lS156 3-8 decoder
#define A0 2
#define A1 3
#define A2 4

// 74HCT240 inverting buffer
#define D0 5
#define D1 6
#define D2 7
#define D3 8
#define D4 9
#define D5 10
#define D6 11
// output enable input; low to read
#define WR 12

// selectors
const int rowPins[3] = {A0, A1, A2};
// inputs
const int colPins[7] = {D0, D1, D2, D3, D4, D5, D6};

bool keyState[8][7];

const char *kybdLUT[8][7] = {
  { "S6",     "S5",   "S4",       "S3",     "S2",       "S1",     "FX" },
  { "CDHOLD", "CDGO", "CDCLEAR",  "FBACK",  "ABCLEAR",  "BO",     "NA" },
  { "ABHOLD", "ABGO", "SETUP",    "PATCH",  "BLIND",    "STAGE",  "PAGE" },
  { "DIM",    "CHAN", "9",        "8",      "7",        "CUE",    "BLANK" },
  { "REMDIM", "THRU", "6",        "5",      "4",        "SUB",    "NA" },
  { "REL",    "AND",  "3",        "2",      "1",        "LINK",   "NA" },
  { "HELP",   "AT",   "+",        "0",      "-",        "TIME",   "NA" },
  { "TRACK",  "FULL", "ENTER",    ".",      "CLEAR",    "REC",    "NA" }
};

const uint16_t keyLUT[8][7] = {
  { 0, 0, 0, 0, 0, 0, 0 },
  { KEY_V, KEY_B, KEY_Z, KEY_D, KEY_A, KEY_K, 0 },
  { KEY_F, KEY_G, KEY_F7, KEY_F5, KEY_F2, KEY_F1, KEY_J },
  { KEY_D, KEY_C, KEYPAD_9, KEYPAD_8, KEYPAD_7, KEY_Q, 0 },
  { 0, KEY_T, KEYPAD_6, KEYPAD_5, KEYPAD_4, KEY_S, 0 },
  { KEY_L, KEY_N, KEYPAD_3, KEYPAD_2, KEYPAD_1, KEY_K, 0 },
  { KEY_SLASH, KEY_A, KEYPAD_PLUS, KEY_0, KEYPAD_MINUS, KEY_I, 0 },
  { KEY_6, KEY_F, KEY_ENTER, KEY_PERIOD, KEY_BACKSPACE, KEY_R, 0 }
};

const uint16_t modLUT[8][7] = {
  { 0, 0, 0, 0, 0, 0, 0 },
  { MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, 0 },
  { MODIFIERKEY_CTRL, MODIFIERKEY_CTRL, MODIFIERKEY_SHIFT, MODIFIERKEY_SHIFT, MODIFIERKEY_SHIFT, MODIFIERKEY_SHIFT, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { MODIFIERKEY_SHIFT, 0, 0, 0, 0, 0, 0 },
  { MODIFIERKEY_SHIFT, 0, 0, 0, 0, 0, 0 }
};

void setup() 
{
  for (int i = 0; i < 3; i++) 
  {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);
  }

  for (int i = 0; i < 7; i++) 
  {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  pinMode(WR, OUTPUT);
  digitalWrite(WR, LOW);  

  //Serial.begin(9600);
} 

void loop() 
{
  for (int row = 0; row < 8; row++) 
  {
    // set binary row address on A0-A2
    for (int bit = 0; bit < 3; bit++) 
    {
      digitalWrite(rowPins[bit], (row >> bit) & 1);
    }

    delayMicroseconds(50);

    for (int col = 0; col < 7; col++) 
    {

      if (row == 0 && col == 6)
      {
        continue;
      }

      int val = digitalRead(colPins[col]);

      if (val == HIGH && keyState[row][col] == false)
      {
        //char str[32];
        //sprintf(str, "Pressed %dx%d = %s", row, col, kybdLUT[row][col]);
        //Serial.println(str);

        Keyboard.set_modifier(modLUT[row][col]);
        Keyboard.send_now();

        Keyboard.set_key1(keyLUT[row][col]);
        Keyboard.send_now();

        Keyboard.set_modifier(0);
        Keyboard.set_key1(0);
        Keyboard.send_now();
      }

      keyState[row][col] = (val == HIGH);
    }
  }

  delay(50);
}
