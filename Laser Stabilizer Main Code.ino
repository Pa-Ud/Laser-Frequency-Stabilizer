//Laser Stabilizing Unit
//Author Mr Parinya Udommai, updated 29/09/2020
//This code is to be uploaded in the ESP32 via Arduino ide.

//1. (Optional device) Controlling frequency of signal generator (replacing ATtiny85)
  #include <AD9850.h>	//***See separate C++ code AD9850.h
  int W_CLK_PIN = 12;
  int FQ_UD_PIN = 13;
  int DATA_PIN = 32;
  int RESET_PIN = 1; 	//assign any free pin to reset, because the reset on SigGen is GND
  double B_freq = 600;	//choose frequency of output sine wave
  int phase = 0;

//2.Input signals to
  int Bpin = 36;    //Bpin reads B-field voltage (solenoid)
  int VPDac = 34;   //VPDac reads AC absorption voltage
  int VPDdc = 35;   //VPDdc reads DC absorption voltage
  int Temppin = 39; //Tempin reads temperature voltage

//3.DAC controlling
  #include "AD5662_BitBang.h"	//***See separate C++ code AD5662_BitBang.h 
  int din = 23;   //is MOSI = master out slave in
  int sclk = 18;  //is clock
  int sync = 5;   //is cs (chip select), it can go to any digital pin
                  //we don't need MISO here
  AD5662 dac(sync, sclk, din);

//4. Micro-timer
  #include "MicroTimer.h"
  //long eventTime = 250000;
  //MicroTimer eventTimer(eventTime);
  
//5. LCD display
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27,16,2); //set the LCD address to 0x27 for a 16 chars and 2 line display
  int SDApin = 33;
  int SCLpin = 14;
  char output[6];
  
//6. Encoder
  #include "Encoder.h"		//***See separate C++ code Encoder.h 
  // define pins
  int encPinA = 27;
  int encPinB = 26; 
  int encPinC = 25;
  Encoder enc(encPinA, encPinB, encPinC);
  volatile int i = 0;
  volatile double n = 0;
  volatile int k = -1;
  volatile int l = 1;
  volatile int i0 = 0;
  volatile double n0 = 0;
  volatile int k0 = 0;
  volatile int a;
  volatile int d = 0;
  volatile int K;
  //volatile int KK[] = {0,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000,2000,3000,4000,5000};
  volatile int KK[] = {0,2,4,6,8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,125,150,175,200,250,300,350,400,500};
  volatile double Z;

//7. Start and stop chars
  const char STX = 0x02; 	//start transmission ascii code
  const char LF = 0x0A; 	//line feed ascii code
  int msgIndex;
  static const int LEN = 9;
  char message[LEN]; 	//max length of a message. NOT terminated by \0   
  

//void setup void setup void setup void setup void setup void setup 
#include "driver/adc.h"
void setup() {
  Serial.begin(250000);
  //Serial.println("Connected.");
  analogSetAttenuation(ADC_6db); // This means full range is 0.15V to ~2V
  //Set the AD9850 sine wave generator
    DDS.begin(W_CLK_PIN, FQ_UD_PIN, DATA_PIN, RESET_PIN);
    DDS.setfreq(B_freq,phase);
   
  //Set input signal pins
    pinMode(Bpin,INPUT);
    pinMode(VPDac,INPUT);
    pinMode(VPDdc,INPUT);
    pinMode(Temppin,INPUT);
  
  //AD5662 dac
    dac.init();
    dac.setVoltage(32767);	//Initially set locking voltage to 0V
 
  //Micro timer
    //eventTimer.reset();
  
  //LCD display
    lcd.begin(SDApin, SCLpin);// initialize the lcd with SDA and SCL pins
    // Print a message to the LCD.
    lcd.noBacklight();
    lcd.setCursor(0,1);
    lcd.print("K=+000");
    lcd.setCursor(8,1);
    lcd.print("Z=+0.00V");
    lcd.setCursor(0,0);
    lcd.print("T=");
    lcd.setCursor(5,0);
    lcd.print("C");
    lcd.setCursor(7,0);
    lcd.print("FB=");
    lcd.setCursor(15,0);
    lcd.print("V");
  
  //Encoder
    enc.init();
    attachInterrupt(digitalPinToInterrupt(encPinA), EncPoll, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encPinB), EncPoll, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encPinC), EncPoll, CHANGE);
  }

uint8_t voidCount = 0;
long t0;
int N = 0;
long timestart, loopInterval;
float Product = 0;
int jmax = 5000;
float Vmean = 0;
float Bi,Bf;
long D = 32767; //D to DAC
long D0 = 32767;
int Temp;
int Temp0 = 0;
double FBvoltage = 0;
int phaseOffset = 0;


//void loop()void loop()void loop()void loop()void loop()void loop()void loop()void loop()
void loop() {
  ////Display values on LCD based on the signal receive from encoder
  int WriteLCD_on = voidCount%5;
  if (WriteLCD_on == 0){ //To avoid program crashing, LCD is updated once every 5 loops
      if (l == 1 && (i != i0 || k != k0))
          {
            lcd.setCursor(2,1);
            if (k==1){lcd.print("+");}
            else {lcd.print("-");}
            sprintf(output, "%03d", K);
            lcd.print(output);
          }
      if (l == -1 && n != n0)
          { 
            lcd.setCursor(10,1);
            if (n<0){lcd.print("-");}
            else {lcd.print("+");}
            lcd.print(Z,2);
          }
      i0 = i;
      k0 = k;
      n0 = n;
      ////Display Temperature
      Temp = int((923*(((analogRead(Temppin)+198)/2281.6)+6.98)/22)-273);
      if (Temp != Temp0){
        lcd.setCursor(2,0);
        sprintf(output,"%03d",Temp);
        lcd.print(output);
        Temp0 = Temp;
        }
      //Display the laser locking voltage
      lcd.setCursor(10,0);
      if (FBvoltage<0){lcd.print("-");}
      else {lcd.print("+");}
        lcd.print(abs(FBvoltage),2);
    }
  voidCount++;
  
  ////////////////////////////////////////////////////////////////////////////////
  timestart = micros();
  //Evaluate the mean AC photodiode voltage
      for (int j=0; j < jmax; j++)
      { Vmean = Vmean + analogRead(VPDac);
      }
      Vmean = Vmean/jmax; //mean value of V of PD
      Vmean = Vmean + phaseOffset;
  ///////////////////////////////////////////////////////////////////////////////
  //MAIN ALGORITHM
    Bi = analogRead(Bpin);
    t0 = micros();
    while (micros() - t0 < 40*1000000/B_freq)  //Do while loop for 40 cycle of Bfreq
    {if (analogRead(VPDac)>Vmean)
        { Bf = analogRead(Bpin);
            Product = Product - (Bf - Bi);
        }
     else
        { Bf = analogRead(Bpin);
			Product = Product + (Bf - Bi);
        }
     Bi = Bf;
     N++;
    }

    D = D + (-k)*K*(Product/N); //16-bit feedback value
      if (D > 65535){D = 65535;}
      if (D < 0){D = 0;}
  dac.setVoltage(D);  //Set the locking voltage to the DAC

  if (d == 1){//if the locking voltage is set manually via an encoder
    D = D0+(n/9.25)*D0;
    dac.setVoltage(D);
    d = 0;
    }
  
  checkSerial();

  loopInterval = micros() - timestart;

  FBvoltage = 2*9.25*(D-32767.5)/65535;
  Serial.print(loopInterval);
  Serial.print(' ');
  Serial.println(k*K);
  Vmean = 0;
  Product = 0;
  N = 0;
}

//voids_voids_voids_voids_voids_voids_voids_voids_voids_voids_voids_voids_voids_voids_
void EncPoll(){// 
    switch(enc.poll())
    {
        case NO_CHANGE:
            break;
        case CW_RATE1: 
            //Serial.println("Clockwise rate 1");
            i = i + 0.5*(l+1);
            n = n + 0.025*(1-l);
            break;
        case CW_RATE2:
            //Serial.println("Clockwise rate 2");
            break;
        case CW_RATE3: 
            //Serial.println("Clockwise rate 3");
            break;
        case ACW_RATE1: 
            //Serial.println("Anti Clockwise rate 1");
            i = i - 0.5*(l+1);
            n = n - 0.025*(1-l);
            break;
        case ACW_RATE2: 
            //Serial.println("Anti Clockwise rate 2");
            break;
        case ACW_RATE3: 
            //Serial.println("Anti Clockwise rate 3");
            break;
        case BUTTON_UP: 
            //Serial.println("Button Released");
            break;
        case BUTTON_DOWN: 
            Serial.println("Button Pressed");
            k = -k*l;
            break;
        case BUTTON_DOUBLECLICK: 
            Serial.println("Button Double Clicked");
            l = -l;
            d = 0.5*(l+1);
            break;
        case BUTTON_HOLD:
            //Serial.println("Button Held Down");
            break;
    }
    if (l == 1 && (i != i0 || k != k0))
      {   if (i<0){i = 0;}
          if (i>32){i = 32;}
            K = KK[i];  
      }
    if (l == -1 && n != n0)
      {   if (n<-9.25){n = -9.25;}
          if (n>9.25){n = 9.25;}
          Z = abs(n);
      }
}
void checkSerial()
{
    while(Serial.available())
    {
        char in = Serial.read();
        if(in != '\n')
        {
            message[msgIndex] = in;
            msgIndex++;
        }
        else
        {
            char type = message[0];
            long value;
            if(message[1] == '-')
            {
                value = - messageToInt(2);
            }
            else
            {
                value = messageToInt(1);
            }
            msgIndex=0; // make the message array clear for a new message

            switch(type)
            {
                case 'P':
                    phaseOffset = value;
                    //Serial.print("K: "); Serial.println(K);
                    break;
                default:
                    break;
            }
        }
    }
}
long messageToInt(int startIndex)
{
    // Returns the number stored in a char array, starting at startIndex
    long number = 0;
    for(int b = startIndex; b < msgIndex; b++) // msgIndex - 1 because we don't include the checksum!
    {
        number *= 10;
        number += (message[b] -'0');
    }
    return number;
}
