#ifndef wordclock_h
#define wordclock_h

#include <Arduino.h>

// include library to read and write from flash memory
#include <EEPROM.h>

// =============================================================================
// Configuration
// =============================================================================
#define DEBUG
#define SERIAL_BAUD 115200
#define DEBOUNCE_DELAY 100 // Milliseconden
#define DIM_DELAY 5 // Seconden

#define PIN_CLOCK_INTERRUPT 16 // Interrupt
#define PIN_LEDmatrix 30
#define PIN_BUZZER 14

// matrix configuration
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define TOTAL_PIXELS MATRIX_WIDTH * MATRIX_HEIGHT

// =============================================================================
// KITT 2000
// =============================================================================
#define KITT_STAART 7 
#define KITT_VANRIJ 0 
#define KITT_TOTRIJ 15 

// =============================================================================
// The Matrix
// =============================================================================
#define UPDATE_MATRIX 50
#define MATRIX_BIRTH_RATIO 75
#define MATRIX_SPEED_MIN 4
#define MATRIX_SPEED_MAX 2
#define MATRIX_LENGTH_MIN 5
#define MATRIX_LENGTH_MAX 20
#define MATRIX_LIFE_MIN 10
#define MATRIX_LIFE_MAX 40
#define MATRIX_MAX_RAYS 60
#define STICKY_COUNT 1000
#define STICKY_PAUSE 5000 // Zolang blijft de tijd in beeld
#define STICKY_MAX 500

// =============================================================================
// Vuurwerk
// =============================================================================
#define UPDATE_VUURWERK 50
#define VUURWERK_BIRTH_RATIO 75
#define VUURWERK_LIFE_MIN 100
#define VUURWERK_LIFE_MAX 500
#define VUURWERK_MAX_RAYS 100

#define DEFAULT_BRIGHTNESS 12

// modes
#define MODE_OFF 0
#define MODE_CLOCK 1
#define MODE_TEMPERATUUR 2
#define MODE_MATRIX 3
#define MODE_DIGICLOCK 4
#define MODE_KITT2000 7
#define MODE_SYNC 8
#define MODE_BAROMETER 9
#define MODE_TEST 10
#define MODE_VUURWERK 11
#define MODE_BINAIR 12

#define Y_OFFSET_DIGICLOCK_SEC 2 
#define Y_OFFSET_DIGICLOCK_SECUIT 5
#define KL_DIGIT_BREEDTE 3
#define KL_DIGIT_HOOGTE 5
#define GR_DIGIT_BREEDTE 5
#define GR_DIGIT_HOOGTE 7

#define LUCHTDRUK_LAAG 988
#define LUCHTDRUK_HOOG 1028

const uint8_t ledLookupTable[256] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,
4,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,
19,19,20,20,21,21,22,23,23,24,24,25,26,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,38,38,39,
40,41,42,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,56,57,58,59,60,61,62,63,64,65,66,67,68,
69,70,71,73,74,75,76,77,78,79,80,81,82,84,85,86,87,88,89,91,92,93,94,95,97,98,99,100,102,103,104,
105,107,108,109,111,112,113,115,116,117,119,120,121,123,124,126,127,128,130,131,133,134,136,137,
139,140,142,143,145,146,148,149,151,152,154,155,157,158,160,162,163,165,166,168,170,171,173,175,
176,178,180,181,183,185,186,188,190,192,193,195,197,199,200,202,204,206,207,209,211,213,215,217,
218,220,222,224,226,228,230,232,233,235,237,239,241,243,245,247,249,251,253,255};

struct color_struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// =============================================================================
// Kleine digits
// =============================================================================
const uint8_t NR_EEN_KL[]   = {0b00000001,0b00000011,0b00000001,0b00000001,0b00000001};
const uint8_t NR_TWEE_KL[]  = {0b00000111,0b00000001,0b00000111,0b00000100,0b00000111};
const uint8_t NR_DRIE_KL[]  = {0b00000111,0b00000001,0b00000111,0b00000001,0b00000111};
const uint8_t NR_VIER_KL[]  = {0b00000101,0b00000101,0b00000111,0b00000001,0b00000001};
const uint8_t NR_VIJF_KL[]  = {0b00000111,0b00000100,0b00000111,0b00000001,0b00000111};
const uint8_t NR_ZES_KL[]   = {0b00000100,0b00000100,0b00000111,0b00000101,0b00000111};
const uint8_t NR_ZEVEN_KL[] = {0b00000111,0b00000001,0b00000001,0b00000001,0b00000001};
const uint8_t NR_ACHT_KL[]  = {0b00000111,0b00000101,0b00000111,0b00000101,0b00000111};
const uint8_t NR_NEGEN_KL[] = {0b00000111,0b00000101,0b00000111,0b00000001,0b00000001};
const uint8_t NR_NUL_KL[]   = {0b00000111,0b00000101,0b00000101,0b00000101,0b00000111};

// =============================================================================
// Grote digits
// =============================================================================
const uint8_t NR_EEN[]      = {0b00000100,0b00001100,0b00000100,0b00000100,0b00000100,0b00000100,0b00001110};
const uint8_t NR_TWEE[]     = {0b00001110,0b00010001,0b00000001,0b00000010,0b00000100,0b00001000,0b00011111};
const uint8_t NR_DRIE[]     = {0b00011111,0b00000010,0b00000100,0b00000010,0b00000001,0b00010001,0b00001110};
const uint8_t NR_VIER[]     = {0b00000010,0b00000110,0b00001010,0b00010010,0b00011111,0b00000010,0b00000010};
const uint8_t NR_VIJF[]     = {0b00011111,0b00010000,0b00011110,0b00000001,0b00000001,0b00010001,0b00001110};
const uint8_t NR_ZES[]      = {0b00000110,0b00001000,0b00010000,0b00011110,0b00010001,0b00010001,0b00001110};
const uint8_t NR_ZEVEN[]    = {0b00011111,0b00000001,0b00000010,0b00000100,0b00000100,0b00000100,0b00000100};
const uint8_t NR_ACHT[]     = {0b00001110,0b00010001,0b00010001,0b00001110,0b00010001,0b00010001,0b00001110};
const uint8_t NR_NEGEN[]    = {0b00001110,0b00010001,0b00010001,0b00001111,0b00000001,0b00000010,0b00001100};
const uint8_t NR_NUL[]      = {0b00001110,0b00010001,0b00010011,0b00010101,0b00011001,0b00010001,0b00001110};

// =============================================================================
// Diversen
// =============================================================================
const uint8_t NR_GRAAD[]    = {0b00000110,0b00001001,0b00001001,0b00000110};
const uint8_t happyface[]   = {0b00111100,0b01000010,0b10100101,0b10000001,0b10100101,0b10011001,0b01000010,0b00111100};
const uint8_t neutralface[] = {0b00111100,0b01000010,0b10100101,0b10000001,0b10111101,0b10000001,0b01000010,0b00111100};
const uint8_t sadface[]     = {0b00111100,0b01000010,0b10100101,0b10000001,0b10011001,0b10100101,0b01000010,0b00111100};

// =============================================================================
// Tekst
// =============================================================================
// const uint8_t _a[]           = {0b00000000,0b00000000,0b00001110,0b00000001,0b00001111,0b00010001,0b00001111};
// const uint8_t _b[]           = {0b00010000,0b00010000,0b00010110,0b00011001,0b00010001,0b00010001,0b00011110};
// const uint8_t _c[]           = {0b00000000,0b00000000,0b00001110,0b00010000,0b00010000,0b00010001,0b00001110};
// const uint8_t _d[]           = {0b00000001,0b00000001,0b00001101,0b00010011,0b00010001,0b00010001,0b00001111};
// const uint8_t _e[]           = {0b00000000,0b00000000,0b00001110,0b00010001,0b00011111,0b00010000,0b00001110};
// const uint8_t _f[]           = {0b00000110,0b00001001,0b00001000,0b00011100,0b00001000,0b00001000,0b00001000};
// const uint8_t _g[]           = {0b00000000,0b00000000,0b00001111,0b00010001,0b00001111,0b00000001,0b00001110};
// const uint8_t _h[]           = {0b00010000,0b00010000,0b00010110,0b00011001,0b00010001,0b00010001,0b00010001};
// const uint8_t _i[]           = {0b00000100,0b00000000,0b00000100,0b00001100,0b00000100,0b00000100,0b00001110};
// const uint8_t _j[]           = {0b00000001,0b00000000,0b00000011,0b00000001,0b00000001,0b00001001,0b00000110};
// const uint8_t _k[]           = {0b00010000,0b00010000,0b00010010,0b00010100,0b00011000,0b00010100,0b00010010};
// const uint8_t _l[]           = {0b00001100,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00001110};
// const uint8_t _m[]           = {0b00000000,0b00000000,0b00011010,0b00010101,0b00010101,0b00010101,0b00010101};
// const uint8_t _n[]           = {0b00000000,0b00000000,0b00010110,0b00011001,0b00010001,0b00010001,0b00010001};
// const uint8_t _o[]           = {0b00000000,0b00000000,0b00001110,0b00010001,0b00010001,0b00010001,0b00001110};
// const uint8_t _p[]           = {0b00000000,0b00000000,0b00011110,0b00010001,0b00011110,0b00010000,0b00010000};
// const uint8_t _q[]           = {0b00000000,0b00000000,0b00001101,0b00010011,0b00001111,0b00000001,0b00000001};
// const uint8_t _r[]           = {0b00000000,0b00000000,0b00010110,0b00011001,0b00010000,0b00010000,0b00010000};
// const uint8_t _s[]           = {0b00000000,0b00000000,0b00001110,0b00010000,0b00001110,0b00000001,0b00011110};
// const uint8_t _t[]           = {0b00001000,0b00001000,0b00011100,0b00001000,0b00001000,0b00001001,0b00000110};
// const uint8_t _u[]           = {0b00000000,0b00000000,0b00010001,0b00010001,0b00010001,0b00010011,0b00001101};
// const uint8_t _v[]           = {0b00000000,0b00000000,0b00010001,0b00010001,0b00010001,0b00001010,0b00000100};
// const uint8_t _w[]           = {0b00000000,0b00000000,0b00010001,0b00010001,0b00010101,0b00010101,0b00001010};
// const uint8_t _x[]           = {0b00000000,0b00000000,0b00010001,0b00001010,0b00000100,0b00001010,0b00010001};
// const uint8_t _y[]           = {0b00000000,0b00000000,0b00010001,0b00010001,0b00001111,0b00000001,0b00001110};
// const uint8_t _z[]           = {0b00000000,0b00000000,0b00011111,0b00000010,0b00000100,0b00001000,0b00011111};

// const uint8_t _A[]           = {0b00000100,0b00001010,0b00010001,0b00010001,0b00011111,0b00010001,0b00010001};
// const uint8_t _B[]           = {0b00011110,0b00001001,0b00001001,0b00001110,0b00001001,0b00001001,0b00011110};
// const uint8_t _C[]           = {0b00001110,0b00010001,0b00010000,0b00010000,0b00010000,0b00010001,0b00001110};
// const uint8_t _D[]           = {0b00011110,0b00001001,0b00001001,0b00001001,0b00001001,0b00001001,0b00011110};
// const uint8_t _E[]           = {0b00011111,0b00010000,0b00010000,0b00011110,0b00010000,0b00010000,0b00011111};
// const uint8_t _F[]           = {0b00011111,0b00010000,0b00010000,0b00011110,0b00010000,0b00010000,0b00010000};
// const uint8_t _G[]           = {0b00001110,0b00010001,0b00010000,0b00010011,0b00010001,0b00010001,0b00001111};
// const uint8_t _H[]           = {0b00010001,0b00010001,0b00010001,0b00011111,0b00010001,0b00010001,0b00010001};
// const uint8_t _I[]           = {0b00001110,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00001110};
// const uint8_t _J[]           = {0b00000111,0b00000010,0b00000010,0b00000010,0b00000010,0b00010010,0b00001100};
// const uint8_t _K[]           = {0b00010001,0b00010010,0b00010100,0b00011000,0b00010100,0b00010010,0b00010001};
// const uint8_t _L[]           = {0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00011111};
// const uint8_t _M[]           = {0b00010001,0b00011011,0b00010101,0b00010101,0b00010001,0b00010001,0b00010001};
// const uint8_t _N[]           = {0b00010001,0b00010001,0b00011001,0b00010101,0b00010011,0b00010001,0b00010001};
// const uint8_t _O[]           = {0b00001110,0b00010001,0b00010001,0b00010001,0b00010001,0b00010001,0b00001110};
// const uint8_t _P[]           = {0b00011110,0b00010001,0b00010001,0b00011110,0b00010000,0b00010000,0b00010000};
// const uint8_t _Q[]           = {0b00001110,0b00010001,0b00010001,0b00010001,0b00010101,0b00010010,0b00001101};
// const uint8_t _R[]           = {0b00011110,0b00010001,0b00010001,0b00011110,0b00010100,0b00010010,0b00010001};
// const uint8_t _S[]           = {0b00001110,0b00010001,0b00010000,0b00001110,0b00000001,0b00010001,0b00001110};
// const uint8_t _T[]           = {0b00011111,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100};
// const uint8_t _U[]           = {0b00010001,0b00010001,0b00010001,0b00010001,0b00010001,0b00010001,0b00001110};
// const uint8_t _V[]           = {0b00010001,0b00010001,0b00010001,0b00010001,0b00010001,0b00001010,0b00000100};
// const uint8_t _W[]           = {0b00010001,0b00010001,0b00010001,0b00010101,0b00010101,0b00010101,0b00001010};
// const uint8_t _X[]           = {0b00010001,0b00010001,0b00001010,0b00000100,0b00001010,0b00010001,0b00010001};
// const uint8_t _Y[]           = {0b00010001,0b00010001,0b00010001,0b00001010,0b00000100,0b00000100,0b00000100};
// const uint8_t _Z[]           = {0b00011111,0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00011111};

struct clockword {
  uint8_t row;
  unsigned int positions;
};

struct MyObject {
  uint8_t mode;
  uint8_t minBrightness;
  uint8_t maxBrightness;
  uint8_t stepBrightness;
  uint8_t startBrightness;
  uint8_t bRood; 
  uint8_t bGroen; 
  uint8_t bBlauw; 
  uint8_t bSRood; 
  uint8_t bSGroen; 
  uint8_t bSBlauw; 
  uint8_t uuruit; 
  uint8_t uuraan; 
  uint8_t nachtsterkte; 
  uint8_t bWordclock; 
  uint8_t bEffect; 
  uint8_t maan; 
  uint8_t desOchtends; 
  uint8_t hetIs; 
  uint8_t autoZomertijd; 
};

struct ray_struct {
  uint8_t x;
  uint8_t y;
  uint8_t speed;
  uint8_t length;
  uint8_t life;
};

struct vuurwerk_struct {
  uint8_t x;
  uint8_t y;
  uint8_t length;
  uint8_t life;
  uint8_t rood;
  uint8_t groen;
  uint8_t blauw;
};

struct kitt_struct {
  int x;
  int y;
  int richtingX;
  int richtingY;
};

#endif
