//Author Dr Matthew Harvey, United Kingdoms, 2018
#include "Arduino.h"
#include "MicroTimer.h"

MicroTimer::MicroTimer(){
  timeOut=1000000;
  start=micros();
  current=start;
}

MicroTimer::MicroTimer(unsigned long t){
  timeOut=t;
  start=micros();
  current=start;
}

void MicroTimer::init(unsigned long t){
  timeOut=t;
  start=micros();
  current=start;
}

unsigned long MicroTimer::elapsed(){
  current=micros();
  return current-start;
}

bool MicroTimer::timedOut(){
  if(elapsed() >= timeOut){
    return true;
  }
  else{
    return false;
  }
}

bool MicroTimer::timedOut(bool RESET){
  if(elapsed() >= timeOut){
    if(RESET) reset();
    return true;
  }
	else{
    return false;
  }
}

void MicroTimer::updateTimeOut(unsigned long t){
  timeOut=t;
}

void MicroTimer::reset(){
  start=micros();
  current=start;
}

unsigned long MicroTimer::getTimeOut(){
  return timeOut;
}
