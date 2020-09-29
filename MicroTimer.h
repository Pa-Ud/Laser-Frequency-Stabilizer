//Author Dr Matthew Harvey, United Kingdoms, 2018
#ifndef MICROTIMER_H
#define MICROTIMER_H

/*
 * This class sets up a tidy timer which allows the caller to check how much time has elapsed since it was last polled and report whether a minimum interval has been passed.
 * This allows events to be setup to occur at minimum set intervals within a block of code.
  */

class MicroTimer
{ 
    volatile unsigned long start;
    volatile unsigned long current;
    volatile unsigned long timeOut;
    //volatile bool autoReset;
  public:
    MicroTimer();
    MicroTimer(unsigned long t);
    void init(unsigned long t); //initialisation function to be used if the timer is created with the null constructor.
    unsigned long elapsed();// return the elapsed time.
    bool timedOut();// function to check whether the timer has timed out. If it has, reports true and resets the timer
    bool timedOut(bool RESET);
    void updateTimeOut(unsigned long t); //update the timeout variable without restarting the current time
    unsigned long getTimeOut();
    void reset(); // reset the timer.      
};

//MicroTimer::MicroTimer(){ //default null constructor that initialises to default 100us interval
//  timeOut=100;
//  start=0;
//  current=start;
//  autoReset=true;
//}



#endif 
