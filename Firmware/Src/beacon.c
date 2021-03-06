#include "beacon.h"

#include "ir.h"
#include "board_id.h"
#include "iprintf.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BEACON_STR_LEN_LONG     (12)    //<<########>>
#define BEACON_STR_LEN_SHORT    (3)    //B##

//TODO use these
static char const * const SpecialBeaconIDs[] = {
   "12345678",
   "12345704",
};

void beacon_Init(void) {
   IRInit();
}

void beacon_Send(void) {
   char beacon[BEACON_STR_LEN_SHORT + 1] = {0};
   //snprintf(beacon, sizeof(beacon), "<<%08lX>>", bid_GetID());
   snprintf(beacon, sizeof(beacon), "B%02X", bid_GetIDCrc());

   iprintf("Sending [%s]\n", beacon);

   // don't send the trailing null
   IRTxBuff((uint8_t*)beacon, sizeof(beacon) - 1);
}

enum BeaconStatus beacon_HaveReceived(void) {
   if(IRDataReady()) {
      iprintf("Incoming IR Message ");

      return BS_Received;

      uint32_t bytes = 0;
      uint8_t* buf = IRGetBuff(&bytes);

      // check if it's valid at all beacon
      if(bytes == 0 || buf == NULL) {
         return BS_None;
      }

      char safebuf[BEACON_STR_LEN_LONG + 1] = {'\0'};
      memcpy(safebuf, buf, bytes);
      iprintf("%d bytes [%s]\n", bytes, safebuf);

      // TODO switch on first char to see what it is (draw string, etc)

      // check if it's a valid beacon
      if(bytes < BEACON_STR_LEN_SHORT || bytes > BEACON_STR_LEN_LONG) {
         iprintf("Got %d bytes, which is too many or to few\n");
         return BS_None;
      }
      /*
      if(!(buf[0] == '<' && buf[1] == '<' && buf[10] == '>' && buf[11] == '>')) {
         iprintf("Beacon string formatted incorrectly\n");
         return BS_None;
      }
      */
      if(buf[0] != 'B') {
         iprintf("Beacon string formatted incorrectly\n");
         return BS_None;
      }

      //TODO check if it's a special ID

      return BS_Received;
   }

   return BS_None;
}

