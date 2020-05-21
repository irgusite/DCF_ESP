#ifndef __DCF77_H
#define __DCF77_H

#include "Arduino.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

void packetGen(/*bool* packet, */int minute, int hour, int day, int month, int year);
bool getPacket(int i);

#ifdef __cplusplus
}
#endif