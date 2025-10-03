#include <arduinoFFT.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <NewPing.h>

 #define SAMPLES 128            
 #define SAMPLING_FREQUENCY 1400   //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 #define SCREEN_WIDTH 128          // OLED display width
 #define SCREEN_HEIGHT 32          // OLED display height
 #define OLED_RESET -1             // OLED reset pin
 #define MIC_PIN 0                // Microphone 
 #define TRIGGER_PIN 11           // IR
 #define ECHO_PIN 12              // IR
 #define MAX_DISTANCE 12

 unsigned int samplingPeriod;
 unsigned long microSeconds;
 float vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
 float vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
 NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
 ArduinoFFT<float> FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY); 
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayOled(float frequency) {
  display.clearDisplay();        // Clear the buffer
  display.setTextSize(2);        // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color
  display.setCursor(0, 0);       // Set cursor position
  display.print("Freq:");       // Print a label
  display.setCursor(0, 18);      // Move cursor for value
  display.print(frequency,2);   // Print the float value with 2 decimal places     
  display.print("Hz");
  display.display();         // Render the display
}

void clearOled(){
  display.clearDisplay();        // Clear the buffer
  display.display();         // Render the display
}

void closerOled(){//unused 
  display.clearDisplay();        // Clear the buffer
  display.setTextSize(2);        // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color
  display.setCursor(0, 0);       // Set cursor position
  display.print("Lemme get");       // Print a label
  display.setCursor(0, 18);       // Set cursor position
  display.print("closer..");       // Print a label
  display.display();         // Render the display
}

void helloOled(){
  display.clearDisplay();        // Clear the buffer
  display.setTextSize(1);        // Set text size
  display.setTextColor(SSD1306_WHITE);  // Set text color
  display.setCursor(20, 3);       // Set cursor position
  display.print("Acordor de tobe");       // Print a label
  display.setCursor(12, 12);       // Set cursor position
  display.print("Stoica Paul-Isaac");       // Print a label
  display.setCursor(22, 21);       // Set cursor position
  display.print("versiunea 0.7");       // Print a label
  display.display();         // Render the display
}

void automaticSaver(float value, int count){
      if (count > 10 ){
      clearOled();
    }else{
      displayOled(value);
    }
}
void setup() 
{
    Serial.begin(115200); //Baud rate for the Serial Monitor
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    while (true);  // Halt execution if OLED initialization fails
  } else{
    helloOled();
  }
}
int secondsWaiting=0; 
float lastMeasure=0;
int firstOpened=0;
int i=0;
void loop() 
{
    unsigned int distance = sonar.ping_cm(); 
    /*Sample SAMPLES times*/
    for(int i=0; i<SAMPLES; i++){
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
        vReal[i] = analogRead(MIC_PIN); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always
        
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
    //Perform FFT on samples
    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.complexToMagnitude(vReal, vImag, SAMPLES);
    //Find peak frequency and print peak
    float peak = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
    while(i<10){
      helloOled();
      firstOpened=1;
      i++;
      delay(500);
    }
    if (firstOpened == 1){ //If the title screen ran
     if (distance < 5){
      lastMeasure=peak;
      secondsWaiting=0;
     }
     secondsWaiting+=1;
     automaticSaver(lastMeasure,secondsWaiting);
    }
  delay (1000);
}