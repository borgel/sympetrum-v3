#include "pattern.h"

#include "ir.h"
#include "iprintf.h"

#include <stdint.h>

struct State {
};
static struct State state = {0};

void pattern_Init(void) {
   IRInit();
}

void pattern_Timeslice(uint32_t const timeMS) {
   // check for new messages
   if(IRDataReady()) {
      iprintf("Got Incoming IR Message ");

      uint32_t bytes = 0;
      uint8_t* buf = IRGetBuff(&bytes);
      iprintf("%d bytes: [%s]\n", bytes, (char*)buf);
   }
}
