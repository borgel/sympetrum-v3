#pragma once

#include <stdint.h>

#include "platform_hw.h"

// should be 36 * 4 = 144
#define TOTAL_CHANNELS     (LED_CHANNELS * LED_BANKS)

// used to mark when an LED is absent from a map
#define NO_LED    (0xFF)

/*
 * This file maps remaps physical RGB channels and bank to friendly logical
 * ordered R G and Bs.
 */

enum matrixMapColor {
   MMC_RED,
   MMC_GREEN,
   MMC_BLUE,
};

// Represents a single LED channel
struct matrixMap {
   // physical
   uint8_t              bank;
   uint8_t              ch;

   // logical 4 x 12
   uint8_t              row;
   uint8_t              col;

   enum matrixMapColor  color;

   // logical linear
   uint8_t              linear;
};

// the amount to add to each bank's 0-11 linear offset to get it's logical offset.
// The linear string goes bank 1, 2, 4, 3 so this is slightly more complex than
// intended.
const uint8_t LinearMatrixBankOffsets[] = {0, 12, 36, 24};

//NOTE: These offsets are one LOWER than the schematic. In the schematic the nets
//start at 1 and go through 36. Here they go from 0 through 35.
const struct matrixMap MatrixMap[TOTAL_CHANNELS] = {
   // bank, channel, row, col, color
   // bank 1, linearly 1st
   {0,  28,  0,  0,	MMC_RED   , 1},
   {0,  25,  0,  0,	MMC_GREEN , 1},
   {0,  26,  0,  0,	MMC_BLUE  , 1},
   {0,   0,  0,  1,	MMC_RED   , 8},
   {0,   1,  0,  1,	MMC_GREEN , 8},
   {0,  34,  0,  1,	MMC_BLUE  , 8},
   {0,  23,  0,  2,	MMC_RED   , 2},
   {0,  24,  0,  2,	MMC_GREEN , 2},
   {0,  27,  0,  2,	MMC_BLUE  , 2},
   {0,   3,	 0,  3,	MMC_RED   , 6},
   {0,   2,	 0,  3,	MMC_GREEN , 6},
   {0,   4,	 0,  3,	MMC_BLUE  , 6},
   {0,   5,	 0,  4,	MMC_RED   , 4},
   {0,   8,	 0,  4,	MMC_GREEN , 4},
   {0,   9,	 0,  4,	MMC_BLUE  , 4},
   {0,  35,	 0,  5,	MMC_RED   , 7},
   {0,  33,	 0,  5,	MMC_GREEN , 7},
   {0,  29,	 0,  5,	MMC_BLUE  , 7},
   {0,  12,	 0,  6,	MMC_RED   , 5},
   {0,  10,	 0,  6,	MMC_GREEN , 5},
   {0,  11,	 0,  6,	MMC_BLUE  , 5},
   {0,  7,	 0,  7,	MMC_RED   , 3},
   {0,  16,	 0,  7,	MMC_GREEN , 3},
   {0,  17,	 0,  7,	MMC_BLUE  , 3},
   {0,  30,	 0,  8,	MMC_RED   , 9},
   {0,  31,	 0,  8,	MMC_GREEN , 9},
   {0,  32,	 0,  8,	MMC_BLUE  , 9},
   {0,  21,	 0,  9,	MMC_RED   , 0},
   {0,  20,	 0,  9,	MMC_GREEN , 0},
   {0,  22,	 0,  9,	MMC_BLUE  , 0},
   {0,  18,	 0, 10,	MMC_RED   ,10},
   {0,   6,	 0, 10,	MMC_GREEN ,10},
   {0,  19,	 0, 10,	MMC_BLUE  ,10},
   {0,  14,	 0, 11,	MMC_RED   ,11},
   {0,  13,	 0, 11,	MMC_GREEN ,11},
   {0,  15,	 0, 11,	MMC_BLUE  ,11},
   // bank 2, linearly 2nd
   {1,   9,	 1,  0,	MMC_RED   , 1},
   {1,   8,	 1,  0,	MMC_GREEN , 1},
   {1,  10,	 1,  0,	MMC_BLUE  , 1},
   {1,   6,	 1,  1,	MMC_RED   , 5},
   {1,   5,	 1,  1,	MMC_GREEN , 5},
   {1,   7,	 1,  1,	MMC_BLUE  , 5},
   {1,   1,	 1,  2,	MMC_RED   , 3},
   {1,  35,	 1,  2,	MMC_GREEN , 3},
   {1,   0,	 1,  2,	MMC_BLUE  , 3},
   {1,  24,	 1,  3,	MMC_RED   ,10},
   {1,  28,	 1,  3,	MMC_GREEN ,10},
   {1,  23,	 1,  3,	MMC_BLUE  ,10},
   {1,   4,	 1,  4,	MMC_RED   , 4},
   {1,   2,	 1,  4,	MMC_GREEN , 4},
   {1,   3,	 1,  4,	MMC_BLUE  , 4},
   {1,  21,	 1,  5,	MMC_RED   ,11},
   {1,  20,	 1,  5,	MMC_GREEN ,11},
   {1,  22,	 1,  5,	MMC_BLUE  ,11},
   {1,  25,	 1,  6,	MMC_RED   , 9},
   {1,  27,	 1,  6,	MMC_GREEN , 9},
   {1,  26,	 1,  6,	MMC_BLUE  , 9},
   {1,  17,	 1,  7,	MMC_RED   , 7},
   {1,  14,	 1,  7,	MMC_GREEN , 7},
   {1,  15,	 1,  7,	MMC_BLUE  , 7},
   {1,  33,	 1,  8,	MMC_RED   , 2},
   {1,  29,	 1,  8,	MMC_GREEN , 2},
   {1,  34,	 1,  8,	MMC_BLUE  , 2},
   {1,  19,	 1,  9,	MMC_RED   , 8},
   {1,  16,	 1,  9,	MMC_GREEN , 8},
   {1,  18,	 1,  9,	MMC_BLUE  , 8},
   {1,  31,	 1, 10,	MMC_RED   , 0},
   {1,  30,	 1, 10,	MMC_GREEN , 0},
   {1,  32,	 1, 10,	MMC_BLUE  , 0},
   {1,  12,	 1, 11,	MMC_RED   , 6},
   {1,  11,	 1, 11,	MMC_GREEN , 6},
   {1,  13,	 1, 11,	MMC_BLUE  , 6},
   // bank 3 linearly 4th
   {2,   9,	 2,  0,	MMC_RED   , 2},
   {2,   6,	 2,  0,	MMC_GREEN , 2},
   {2,   8,	 2,  0,	MMC_BLUE  , 2},
   {2,   7,	 2,  1,	MMC_RED   , 7},
   {2,  17,	 2,  1,	MMC_GREEN , 7},
   {2,  16,	 2,  1,	MMC_BLUE  , 7},
   {2,   4,	 2,  2,	MMC_RED   , 3},
   {2,   3,	 2,  2,	MMC_GREEN , 3},
   {2,   5,	 2,  2,	MMC_BLUE  , 3},
   {2,  30,	 2,  3,	MMC_RED   ,11},
   {2,  29,	 2,  3,	MMC_GREEN ,11},
   {2,  31,	 2,  3,	MMC_BLUE  ,11},
   {2,  12,	 2,  4,	MMC_RED   , 1},
   {2,  15,	 2,  4,	MMC_GREEN , 1},
   {2,  13,	 2,  4,	MMC_BLUE  , 1},
   {2,  24,	 2,  5,	MMC_RED   , 8},
   {2,  23,	 2,  5,	MMC_GREEN , 8},
   {2,  27,	 2,  5,	MMC_BLUE  , 8},
   {2,  32,	 2,  6,	MMC_RED   , 6},
   {2,  18,	 2,  6,	MMC_GREEN , 6},
   {2,  19,	 2,  6,	MMC_BLUE  , 6},
   {2,  10,	 2,  7,	MMC_RED   , 0},
   {2,  14,	 2,  7,	MMC_GREEN , 0},
   {2,  11,	 2,  7,	MMC_BLUE  , 0},
   {2,  25,	 2,  8,	MMC_RED   , 9},
   {2,  28,	 2,  8,	MMC_GREEN , 9},
   {2,  26,	 2,  8,	MMC_BLUE  , 9},
   {2,  33,	 2,  9,	MMC_RED   , 5},
   {2,  35,	 2,  9,	MMC_GREEN , 5},
   {2,  34,	 2,  9,	MMC_BLUE  , 5},
   {2,   2,	 2, 10,	MMC_RED   , 4},
   {2,   1,	 2, 10,	MMC_GREEN , 4},
   {2,   0,	 2, 10,	MMC_BLUE  , 4},
   {2,  20,	 2, 11,	MMC_RED   ,10},
   {2,  21,	 2, 11,	MMC_GREEN ,10},
   {2,  22,	 2, 11,	MMC_BLUE  ,10},
   // bank 4, linearlly 3rd
   {3,  24,	 3,  0,	MMC_RED   , 1},
   {3,  23,	 3,  0,	MMC_GREEN , 1},
   {3,  28,	 3,  0,	MMC_BLUE  , 1},
   {3,  21,	 3,  1,	MMC_RED   , 0},
   {3,  22,	 3,  1,	MMC_GREEN , 0},
   {3,  20,	 3,  1,	MMC_BLUE  , 0},
   {3,  10,	 3,  2,	MMC_RED   , 8},
   {3,   0,	 3,  2,	MMC_GREEN , 8},
   {3,   3,	 3,  2,	MMC_BLUE  , 8},
   {3,   7,	 3,  3,	MMC_RED   , 4},
   {3,  29,	 3,  3,	MMC_GREEN , 4},
   {3,  30,	 3,  3,	MMC_BLUE  , 4},
   {3,  18,	 3,  4,	MMC_RED   , 3},
   {3,  19,	 3,  4,	MMC_GREEN , 3},
   {3,  13,	 3,  4,	MMC_BLUE  , 3},
   {3,  32,	 3,  5,	MMC_RED   ,10},
   {3,  15,	 3,  5,	MMC_GREEN ,10},
   {3,  12,	 3,  5,	MMC_BLUE  ,10},
   {3,   4,	 3,  6,	MMC_RED   , 6},
   {3,   2,	 3,  6,	MMC_GREEN , 6},
   {3,  11,	 3,  6,	MMC_BLUE  , 6},
   {3,  31,	 3,  7,	MMC_RED   , 7},
   {3,  14,	 3,  7,	MMC_GREEN , 7},
   {3,   9,	 3,  7,	MMC_BLUE  , 7},
   {3,  25,	 3,  8,	MMC_RED   , 2},
   {3,  26,	 3,  8,	MMC_GREEN , 2},
   {3,  27,	 3,  8,	MMC_BLUE  , 2},
   {3,   5,	 3,  9,	MMC_RED   ,11},
   {3,   8,	 3,  9,	MMC_GREEN ,11},
   {3,   6,	 3,  9,	MMC_BLUE  ,11},
   {3,   1,	 3, 10,	MMC_RED   , 5},
   {3,  16,	 3, 10,	MMC_GREEN , 5},
   {3,  17,	 3, 10,	MMC_BLUE  , 5},
   {3,  35,	 3, 11,	MMC_RED   , 9},
   {3,  33,	 3, 11,	MMC_GREEN , 9},
   {3,  34,	 3, 11,	MMC_BLUE  , 9},
   //143
};

// use a more convenient macro for a moment
#define NO        (NO_LED)
// 31 wide, 30 tall
uint8_t const MatrixMapSparse[30][31]  = {
   // put the linear ch number (0-47) in if it exists, and NO if it is blank
   // divided into 5x5 grids                        \/
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,23,NO,NO,NO, NO,NO,NO,NO,24, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //0
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,22, NO,25,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //1
   {NO,NO,16,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,21,NO, NO,NO,26,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,30,NO, NO}, //2
   {15,NO,NO,NO,NO, 17,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, 29,NO,NO,NO,NO, 31}, //3
   {NO,NO,NO,14,NO, NO,NO,NO,NO,18, NO,NO,19,NO,NO, NO,NO,NO,27,NO, NO,28,NO,NO,NO, NO,NO,32,NO,NO, NO}, //4
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO,NO, NO,13,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,33, NO,NO,NO,NO,NO, NO}, //5
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, 20,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //       <
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,12, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,34,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO, 9,NO,NO,NO, NO,NO,NO,11,NO, NO,NO,42,NO,NO, NO,NO,NO,NO,35, NO,NO,NO,NO,NO, NO}, //9
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO, 8, NO,NO,NO,NO,NO, NO,10,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,36,NO,NO,NO, NO}, //10
   {NO, 7,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, 41,NO,NO,NO,NO, NO,NO,NO,NO,37, NO},
   {NO,NO,NO, 6,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,38,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO, 5,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,39, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO, 4,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,40,NO,NO, NO,NO,NO,NO,NO, NO}, //14
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO, 3,NO, NO,43,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //15
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,44,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO, 2,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //19
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //20
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,45,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO, 1,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //24
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,46,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //25
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO, 0,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO},
   {NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, 47,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO,NO,NO,NO,NO, NO}, //29
//  0  1  2  3  4   5  6  7  8  9   10 11 12 13 14  15 16 17 18 19  20 21 22 23 24  25 26 27 28 29  30
};

// FIXME reset 2nd bound to the max number in one ring
uint8_t const MatrixMapPolar[18][5] = {
   // terminate each row with a NO
   {20, NO},
   {NO}, //blank
   {11, 19, 27, 42, NO},
   {21, 26, NO},
   //TODO finish
};

// and we're done
#ifdef NO
#undef NO
#endif

