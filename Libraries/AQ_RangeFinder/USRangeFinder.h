/*
*/

#ifndef _AEROQUAD_GENERIC_SONAR_RANGE_FINDER_H_
#define _AEROQUAD_GENERIC_SONAR_RANGE_FINDER_H_

#if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(BOARD_aeroquad32)

#include "RangeFinder.h"
#include <Serial.h>

#define SPIKE_FILTER_MARGIN 500 // mm ; changes bigger than this need two samples to take effect

// last reading used for 'spike' filter
unsigned short lastRange;

#define MAXWAIT 100    // do not wait for echo more than 100ms
unsigned int pingStarted = 0;

void startPing() {
  byte cmd = 0x55;
  Serial2.write(&cmd, 1);
  pingStarted = millis();
}

void inititalizeRangeFinders() {
  Serial2.begin(9600);
  startPing();
  lastRange = 32000; 
}

void updateRangeFinders() {
  unsigned int now = millis();
  
  if ( Serial2.available() ) {
    byte resp[2];
    unsigned short range;
    Serial2.readBytes((char*)resp, 2);
    range = resp[0]*256+resp[1];

    if ( range > maxRangeFinderRange*1000 ) {
        range = INVALID_RANGE;
    }
    // Following will accept the sample if it's either within "spike margin" of last raw reading or previous accepted reading
    // otherwise it's ignored as noise
    if ((abs(range - lastRange) < SPIKE_FILTER_MARGIN) ||
      (abs(range - rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX] * 1000) < SPIKE_FILTER_MARGIN)) {
        rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX] = (float)range / 1000;
    }
    lastRange = range;
    
    startPing();
  }
  else if ( now - pingStarted > MAXWAIT) {
    rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX] = INVALID_RANGE;
    lastRange = INVALID_RANGE;
    
    startPing();
  }
}

#endif 
#endif








