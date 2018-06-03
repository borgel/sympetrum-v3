#pragma once

enum BeaconStatus {
   BS_None,
   BS_Received,
   BS_Special,
};

void beacon_Init(void);
void beacon_Send(void);
enum BeaconStatus beacon_HaveReceived(void);

