#!/usr/local/bin/python

CHANNELS=144

ROWS=4
COLS=12

colors = [  "MMC_RED   ",
            "MMC_GREEN ",
            "MMC_BLUE  "]

bank = 0
channel = 0

for r in range(0, ROWS):
   for c in range(0, COLS):
      for color in colors:
         # bank, channel, row, col, color
         # {0 ,  0,    0, 0, MMC_RED     },
         print("{%d,  %2d,\t%2d, %2d,\t%s}," % (bank, channel, r, c, color))

         channel += 1
         if channel > 35:
            channel = 0
            bank += 1

            print("// bank %d" % bank)

