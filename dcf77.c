#include "dcf77.h"
#include "Arduino.h"

bool packet[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void packetGen(/*bool* packet, */int minute, int hour, int day, int month, int year) {

  int parity = 0;

  //first (0) packet always 0
  //packet 1-15 always 0 in our case

  //packet 16 summer time announcement

  //other packet blabla

  //packet 20 always 1
  packet[20] = 1;

  //packet 21-27 minutes, 28 even parity

  packet[21] = (((minute % 10) >> 0) & 0x1); //1
  packet[22] = (((minute % 10) >> 1) & 0x1); //2
  packet[23] = (((minute % 10) >> 2) & 0x1); //4
  packet[24] = (((minute % 10) >> 3) & 0x1); //8
  packet[25] = (((minute / 10) >> 0) & 0x1); //10
  packet[26] = (((minute / 10) >> 1) & 0x1); //20
  packet[27] = (((minute / 10) >> 2) & 0x1); //40

  //minutes parity
  parity = 0;
  for (int u = 21; u <= 27; ++u) {
    parity += packet[u];
  }
  packet[28] = parity % 2;

  //Serial.println(minute, BIN);
  //Serial.print(":");
  //packet[28] = packet[28];

  //packet 29-34 minutes, 35 even parity
  //int minute = 28;
  packet[29] = (((hour % 10) >> 0) & 0x1); //1
  packet[30] = (((hour % 10) >> 1) & 0x1); //2
  packet[31] = (((hour % 10) >> 2) & 0x1); //4
  packet[32] = (((hour % 10) >> 3) & 0x1); //8
  packet[33] = (((hour / 10) >> 0) & 0x1); //10
  packet[34] = (((hour / 10) >> 1) & 0x1); //20

  //Serial.println(hour);
  //hour parity
  parity = 0;
  for (int u = 29; u <= 34; ++u) {
    parity += packet[u];
  }
  packet[35] = parity % 2;
  //Serial.println(packet[35]);

  // ***** DAY *****
  packet[36] = (((day % 10) >> 0) & 0x1); //1
  packet[37] = (((day % 10) >> 1) & 0x1); //2
  packet[38] = (((day % 10) >> 2) & 0x1); //4
  packet[39] = (((day % 10) >> 3) & 0x1); //8
  packet[40] = (((day / 10) >> 0) & 0x1); //10
  packet[41] = (((day / 10) >> 1) & 0x1); //20

  int dof = dayofweek(day, month, year) + 1;

  // **** DAY OF WEEK *****
  packet[42] = (((dof % 10) >> 0) & 0x1); //1
  packet[43] = (((dof % 10) >> 1) & 0x1); //2
  packet[44] = (((dof % 10) >> 2) & 0x1); //4

  // ***** MONTH ******
  packet[45] = (((month % 10) >> 0) & 0x1); //1
  packet[46] = (((month % 10) >> 1) & 0x1); //2
  packet[47] = (((month % 10) >> 2) & 0x1); //4
  packet[48] = (((month % 10) >> 3) & 0x1); //8
  packet[49] = (((month / 10) >> 0) & 0x1); //10

  // ***** YEAR ******
  packet[50] = (((year % 10) >> 0) & 0x1); //1
  packet[51] = (((year % 10) >> 1) & 0x1); //2
  packet[52] = (((year % 10) >> 2) & 0x1); //4
  packet[53] = (((year % 10) >> 3) & 0x1); //8
  packet[54] = (((year / 10) >> 0) & 0x1); //10
  packet[55] = (((year / 10) >> 1) & 0x1); //20
  packet[56] = (((year / 10) >> 2) & 0x1); //40
  packet[57] = (((year / 10) >> 3) & 0x1); //80

  // packet[30] = 1; //2

  //date parity
  parity = 0;
  for (int u = 36; u <= 57; ++u) {
    parity += packet[u];
  }
  packet[58] = parity % 2;

}

int dayofweek(int d, int m, int y)
{
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  y -= m < 3;
  return ( y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

bool getPacket(int i) {
  return packet[i];
}
