#include "stm32l476xx.h"

#define CLK 16000000
#define PRESCALE 10

#define C3  131
#define C_3 138
#define D3  147
#define D_3 156
#define E3  165
#define F3  175
#define F_3 185
#define G3  196
#define G_3 208
#define A3  220
#define A_3 233
#define B3  247
#define C4  262
#define C_4 277
#define D4  294 
#define D_4 311
#define E4  330
#define F4  349
#define F_4 370
#define G4  392
#define G_4 415
#define A4  440
#define A_4 466
#define B4  494
#define C5  523

static int NOTES[] = { C3, C_3, D3, D_3, E3, F3, F_3, G3, G_3, A3, A_3, B3, C4, C_4, D4, D_4, E4, F4, F_4, G4, G_4, A4, A_4, B4, C5 };
static char* NOTE_NAMES[] = { "C3 ", "C#3", "D3 ", "D#3", "E3 ", "F3 ", "F#3", "G3 ", "G#3", "A3 ", "A#3", "B3 ", "C4 ", "C#4", "D4 ", "D#4", "E4 ", "F4 ", "F#4", "G4 ", "G#4", "A4 ", "A#4", "B4 ", "C5 " };

void vTimerManagerSetT4(int steps, int freq);
void vTimerManagerInitT4(int steps, int freq);