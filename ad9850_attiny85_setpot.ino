#include <AD9850.h>

AD9850 ad(0, 1, 2); // w_clk, fq_ud, d7
int Ain;
int freq;
void setup() {}

// sweep form 1MHz to 10MHz
void loop() {

  Ain = analogRead(A3);
  // The analog channels read 1024 levels. I only want 256 levels to reduce range and jitter.
  // if I bitshift it twice to the right, my numbers should go from 0 to 255.
  Ain >>= 2;

  freq=50*(Ain+1); //min freq = 50, max freq = 12800 Hz
  ad.setfreq(freq);
  delay(10);
}
