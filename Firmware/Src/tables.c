#include <stdint.h>

// TODO increase granulrity to 1 deg/setp? (from 2)
uint8_t const CosTable[] = {
         254,
         254,
         254,
         253,
         253,
         252,
         251,
         250,
         249,
         248,
         246,
         245,
         243,
         241,
         239,
         237,
         235,
         232,
         230,
         227,
         224,
         221,
         218,
         215,
         212,
         209,
         205,
         202,
         198,
         194,
         191,
         187,
         183,
         179,
         175,
         170,
         166,
         162,
         158,
         153,
         149,
         145,
         140,
         136,
         131,
         127,
         123,
         118,
         114,
         109,
         105,
         101,
         96,
         92,
         88,
         84,
         79,
         75,
         71,
         67,
         64,
         60,
         56,
         52,
         49,
         45,
         42,
         39,
         36,
         33,
         30,
         27,
         24,
         22,
         19,
         17,
         15,
         13,
         11,
         9,
         8,
         6,
         5,
         4,
         3,
         2,
         1,
         1,
         0,
         0,
         0
};
unsigned const CosTableSize = sizeof(CosTable);

uint8_t const CIETable[256] = {
   0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 
   1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 
   2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 
   3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 
   5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 
   7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 
   10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 
   13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 
   17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 
   22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 
   28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 
   34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 
   41, 42, 43, 43, 44, 45, 46, 47, 47, 48, 
   49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 
   58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 
   68, 70, 71, 72, 73, 74, 75, 76, 77, 79, 
   80, 81, 82, 83, 85, 86, 87, 88, 90, 91, 
   92, 94, 95, 96, 98, 99, 100, 102, 103, 105, 
   106, 108, 109, 110, 112, 113, 115, 116, 118, 120, 
   121, 123, 124, 126, 128, 129, 131, 132, 134, 136, 
   138, 139, 141, 143, 145, 146, 148, 150, 152, 154, 
   155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 
   175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 
   196, 198, 200, 202, 204, 207, 209, 211, 214, 216, 
   218, 220, 223, 225, 228, 230, 232, 235, 237, 240, 
   242, 245, 247, 250, 252, 255,
};
unsigned const CIETableSize = sizeof(CIETable);

