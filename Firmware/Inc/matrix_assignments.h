#pragma once

#include <stdint.h>

#include "platform_hw.h"

// should be 36 * 4 = 144
#define TOTAL_CHANNELS     (LED_CHANNELS * LED_BANKS)

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

   // logical
   uint8_t              row;
   uint8_t              col;
   enum matrixMapColor  color;
};

//NOTE: These offsets are one LOWER than the schematic. In the schematic the nets
//start at 1 and go through 36. Here they go from 0 through 35.
const struct matrixMap MatrixMap[TOTAL_CHANNELS] = {
   // bank, channel, row, col, color
   // bank 1
   {0,   25, 0,  0,	MMC_RED   },
   {0,   26, 0,  0,	MMC_GREEN },
   {0,   28, 0,  0,	MMC_BLUE  },
   {0,    1, 0,  1,	MMC_RED   },
   {0,   34, 0,  1,	MMC_GREEN },
   {0,    0, 0,  1,	MMC_BLUE  },
   {0,   24, 0,  2,	MMC_RED   },
   {0,   27, 0,  2,	MMC_GREEN },
   {0,   23, 0,  2,	MMC_BLUE  },
   {0,   2,	 0,  3,	MMC_RED   },
   {0,   4,	 0,  3,	MMC_GREEN },
   {0,   3,	 0,  3,	MMC_BLUE  },
   {0,   8,	 0,  4,	MMC_RED   },
   {0,   9,	 0,  4,	MMC_GREEN },
   {0,   5,	 0,  4,	MMC_BLUE  },
   {0,  33,	 0,  5,	MMC_RED   },
   {0,  29,	 0,  5,	MMC_GREEN },
   {0,  35,	 0,  5,	MMC_BLUE  },
   {0,  10,	 0,  6,	MMC_RED   },
   {0,  11,	 0,  6,	MMC_GREEN },
   {0,  12,	 0,  6,	MMC_BLUE  },
   {0,  16,	 0,  7,	MMC_RED   },
   {0,  17,	 0,  7,	MMC_GREEN },
   {0,  7,	 0,  7,	MMC_BLUE  },
   {0,  31,	 0,  8,	MMC_RED   },
   {0,  32,	 0,  8,	MMC_GREEN },
   {0,  30,	 0,  8,	MMC_BLUE  },
   {0,  20,	 0,  9,	MMC_RED   },
   {0,  22,	 0,  9,	MMC_GREEN },
   {0,  21,	 0,  9,	MMC_BLUE  },
   {0,  6,	 0, 10,	MMC_RED   },
   {0,  19,	 0, 10,	MMC_GREEN },
   {0,  18,	 0, 10,	MMC_BLUE  },
   {0,  13,	 0, 11,	MMC_RED   },
   {0,  15,	 0, 11,	MMC_GREEN },
   {0,  14,	 0, 11,	MMC_BLUE  },
   // bank 2
   {1,   0,	 1,  0,	MMC_RED   },
   {1,   1,	 1,  0,	MMC_GREEN },
   {1,   2,	 1,  0,	MMC_BLUE  },
   {1,   3,	 1,  1,	MMC_RED   },
   {1,   4,	 1,  1,	MMC_GREEN },
   {1,   5,	 1,  1,	MMC_BLUE  },
   {1,   6,	 1,  2,	MMC_RED   },
   {1,   7,	 1,  2,	MMC_GREEN },
   {1,   8,	 1,  2,	MMC_BLUE  },
   {1,   9,	 1,  3,	MMC_RED   },
   {1,  10,	 1,  3,	MMC_GREEN },
   {1,  11,	 1,  3,	MMC_BLUE  },
   {1,  12,	 1,  4,	MMC_RED   },
   {1,  13,	 1,  4,	MMC_GREEN },
   {1,  14,	 1,  4,	MMC_BLUE  },
   {1,  15,	 1,  5,	MMC_RED   },
   {1,  16,	 1,  5,	MMC_GREEN },
   {1,  17,	 1,  5,	MMC_BLUE  },
   {1,  18,	 1,  6,	MMC_RED   },
   {1,  19,	 1,  6,	MMC_GREEN },
   {1,  20,	 1,  6,	MMC_BLUE  },
   {1,  21,	 1,  7,	MMC_RED   },
   {1,  22,	 1,  7,	MMC_GREEN },
   {1,  23,	 1,  7,	MMC_BLUE  },
   {1,  24,	 1,  8,	MMC_RED   },
   {1,  25,	 1,  8,	MMC_GREEN },
   {1,  26,	 1,  8,	MMC_BLUE  },
   {1,  27,	 1,  9,	MMC_RED   },
   {1,  28,	 1,  9,	MMC_GREEN },
   {1,  29,	 1,  9,	MMC_BLUE  },
   {1,  30,	 1, 10,	MMC_RED   },
   {1,  31,	 1, 10,	MMC_GREEN },
   {1,  32,	 1, 10,	MMC_BLUE  },
   {1,  33,	 1, 11,	MMC_RED   },
   {1,  34,	 1, 11,	MMC_GREEN },
   {1,  35,	 1, 11,	MMC_BLUE  },
   // bank 3
   {2,   0,	 2,  0,	MMC_RED   },
   {2,   1,	 2,  0,	MMC_GREEN },
   {2,   2,	 2,  0,	MMC_BLUE  },
   {2,   3,	 2,  1,	MMC_RED   },
   {2,   4,	 2,  1,	MMC_GREEN },
   {2,   5,	 2,  1,	MMC_BLUE  },
   {2,   6,	 2,  2,	MMC_RED   },
   {2,   7,	 2,  2,	MMC_GREEN },
   {2,   8,	 2,  2,	MMC_BLUE  },
   {2,   9,	 2,  3,	MMC_RED   },
   {2,  10,	 2,  3,	MMC_GREEN },
   {2,  11,	 2,  3,	MMC_BLUE  },
   {2,  12,	 2,  4,	MMC_RED   },
   {2,  13,	 2,  4,	MMC_GREEN },
   {2,  14,	 2,  4,	MMC_BLUE  },
   {2,  15,	 2,  5,	MMC_RED   },
   {2,  16,	 2,  5,	MMC_GREEN },
   {2,  17,	 2,  5,	MMC_BLUE  },
   {2,  18,	 2,  6,	MMC_RED   },
   {2,  19,	 2,  6,	MMC_GREEN },
   {2,  20,	 2,  6,	MMC_BLUE  },
   {2,  21,	 2,  7,	MMC_RED   },
   {2,  22,	 2,  7,	MMC_GREEN },
   {2,  23,	 2,  7,	MMC_BLUE  },
   {2,  24,	 2,  8,	MMC_RED   },
   {2,  25,	 2,  8,	MMC_GREEN },
   {2,  26,	 2,  8,	MMC_BLUE  },
   {2,  27,	 2,  9,	MMC_RED   },
   {2,  28,	 2,  9,	MMC_GREEN },
   {2,  29,	 2,  9,	MMC_BLUE  },
   {2,  30,	 2, 10,	MMC_RED   },
   {2,  31,	 2, 10,	MMC_GREEN },
   {2,  32,	 2, 10,	MMC_BLUE  },
   {2,  33,	 2, 11,	MMC_RED   },
   {2,  34,	 2, 11,	MMC_GREEN },
   {2,  35,	 2, 11,	MMC_BLUE  },
   // bank 4
   {3,   0,	 3,  0,	MMC_RED   },
   {3,   1,	 3,  0,	MMC_GREEN },
   {3,   2,	 3,  0,	MMC_BLUE  },
   {3,   3,	 3,  1,	MMC_RED   },
   {3,   4,	 3,  1,	MMC_GREEN },
   {3,   5,	 3,  1,	MMC_BLUE  },
   {3,   6,	 3,  2,	MMC_RED   },
   {3,   7,	 3,  2,	MMC_GREEN },
   {3,   8,	 3,  2,	MMC_BLUE  },
   {3,   9,	 3,  3,	MMC_RED   },
   {3,  10,	 3,  3,	MMC_GREEN },
   {3,  11,	 3,  3,	MMC_BLUE  },
   {3,  12,	 3,  4,	MMC_RED   },
   {3,  13,	 3,  4,	MMC_GREEN },
   {3,  14,	 3,  4,	MMC_BLUE  },
   {3,  15,	 3,  5,	MMC_RED   },
   {3,  16,	 3,  5,	MMC_GREEN },
   {3,  17,	 3,  5,	MMC_BLUE  },
   {3,  18,	 3,  6,	MMC_RED   },
   {3,  19,	 3,  6,	MMC_GREEN },
   {3,  20,	 3,  6,	MMC_BLUE  },
   {3,  21,	 3,  7,	MMC_RED   },
   {3,  22,	 3,  7,	MMC_GREEN },
   {3,  23,	 3,  7,	MMC_BLUE  },
   {3,  24,	 3,  8,	MMC_RED   },
   {3,  25,	 3,  8,	MMC_GREEN },
   {3,  26,	 3,  8,	MMC_BLUE  },
   {3,  27,	 3,  9,	MMC_RED   },
   {3,  28,	 3,  9,	MMC_GREEN },
   {3,  29,	 3,  9,	MMC_BLUE  },
   {3,  30,	 3, 10,	MMC_RED   },
   {3,  31,	 3, 10,	MMC_GREEN },
   {3,  32,	 3, 10,	MMC_BLUE  },
   {3,  33,	 3, 11,	MMC_RED   },
   {3,  34,	 3, 11,	MMC_GREEN },
   {3,  35,	 3, 11,	MMC_BLUE  },
};

