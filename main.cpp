//Mini Stacker is an attempt at cloning the Arcade game Stacker by LAI Games.
//Using LED Strips, a button, and optionally a Buzzer.
//Oh, and of course the Arduino Uno
//Animations and sounds aren't accurate to the machine/machines.
//Also my first time coding for Arduino.
#include <FastLED.h>
#include <Arduino.h>

#define NUM_STRIPS 7
#define NUM_LEDS_PER_STRIP 12
#define BUTTON_PIN 4
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];


bool clicked = false;

bool goingLeft = true; // Going left if true, right if false.
const int bottom = 11; // Subtract one from the top if using the whole strip.
const int buzzerPin = 6; //Is there a buzzer, set to -1 to disable.
const int top = 12; // Basically the height of the strips
const int maxHeight = bottom - top;

CRGB blockColor = CRGB::Blue;

unsigned long prevMillis = 0; // The previous time in milliseconds
unsigned long curMillis = 0; //Current time in milliseconds, to prevent multiple calls to millis()
unsigned long buttonwait =0;
const long animationDuration = 2000; // Animation for loss of block
const long defaultInterval = 200; // The starting time between updates
const int intervalModifier = 0; // How much the interval should change for every success.
//Your maximum interval modifier should equal defaultInterval / top
long maxSpeed = 10; // Maximum allowed speed for the blocks.  Never set to negative
long interval = defaultInterval; // Time between updates.  This shouldn't ever be made a const, as it will change.

int cursize = 3; //Size of the stacking block
int curpos = 2; // Position of the block
int currow = bottom;//What row of LEDS the block is in

// For mirroring strips, all the "special" stuff happens just in setup.  We
// just addLeds multiple times, once for each strip
void setup() {

  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, 7>(leds[6], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 8>(leds[5], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 9>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 10>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 12>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 13>(leds[3], NUM_LEDS_PER_STRIP);

  if (maxSpeed<0) {
    maxSpeed = 0;
    Serial.println("Error: Max speed was set to negative.");
    }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  int value = digitalRead((BUTTON_PIN));
}
bool checkAir ();
void drawBlock();
void onUpdate();
void resetGame();
void failState(int echo);
void animatedrop(int pos, int width);
void animatedrop(int pos, int width, bool isFail);
void buttonPressed();

int lastState = HIGH;
int value = HIGH;

  void onUpdate()
  {
    if (goingLeft) {              // If we're moving left
    if (curpos > 0) {           // Check if we can go left
        curpos--;               // If we can, let's go left
      } else {                  //Otherwise...
        curpos++;               //We'll go right
        goingLeft =false;       //And make sure we're really going right.
      }
    } else {                    //If we're going right
      if (curpos+cursize < 7) { //Check if we can go right
        curpos++;               //If we can, let's go right.
      } else {                  //Otherwise...
        curpos--;               //Go left
        goingLeft=true;         //And really go left
      }
    }
    drawBlock(); // Finally we may draw
    prevMillis = curMillis;
  }

  void drawBlock() 
  {
    leds[curpos][currow] = blockColor; // We always draw the leftmost block
    if (cursize>1) //Then check if there's more
    {
      if (cursize==3) //If there's three we can draw the third
      {
        leds[curpos+2][currow] = blockColor; //Drawing the third
      }
      leds[curpos+1][currow] = blockColor; //If we're more than one, it should be at least two.  So draw the second.
    }

    if (goingLeft) //If the movement is going left
    {
      leds[curpos+cursize][currow] = CRGB::Black; // Clear the previous rightmost
    }
    else 
    {
      leds[curpos-1][currow] = CRGB::Black; //Otherwise we're going right, and only have to remove one to the left
    }

    FastLED.show(); // Then we can draw

    //Serial.print("X: "); Serial.print(curpos); Serial.print(" Y: "); Serial.print(currow);Serial.print("\n");
  }

  bool checkAir () 
  {
    Serial.println("Checking Air");
    if (leds[curpos][currow+1] == CRGB(0,0,0)) { // If the leftmost block has fallen
      if (cursize>1) { // If there's more than one block
        if (cursize==3) // if 3 wide
        {
          if (leds[curpos+2][currow+1] == CRGB(0,0,0)) { // If the bar isnt seated
            //Failstate, the triple block has fallen
            failState(1);
            return true;
          }
          else if (leds[curpos+1][currow+1] == CRGB(0,0,0)) { //If this is call
            cursize -= 2; //2 blocks have dropped 
            animatedrop(curpos,2);
          }
          else {
            cursize--; // 1 block has dropped 
            animatedrop(curpos,1);
          }
        }
        else if (leds[curpos+1][currow+1] == CRGB(0,0,0)) { // If its greater than one but less than three it's two wide
          //Failstate, the double block has fallen
           failState(2);
           return true;
        }
        else {
          cursize--; // one block has dropped
          animatedrop(curpos,1);
        }
      }
      else { // If there's only one block
        //Failstate, the single block has fallen
         failState(3);
         return true;
      }
    }
    else // If the leftmost block hasn't fallen 
    {
      //Serial.println("Left didnt fall");
      if (cursize>1) { // If there's more than one block
        if (cursize==3) // if 3 wide
        {
          if (leds[curpos+1][currow+1] == CRGB(0,0,0)) { // If the second isn't seated
            cursize -= 2; //2 blocks have dropped 
            animatedrop(curpos+1,2);
          }
          else if (leds[curpos+2][currow+1] == CRGB(0,0,0)) { //if the third isn't seated
            cursize -= 1; //1 blocks have dropped 
            animatedrop(curpos+2,1);
          }
          else  // Nothing fallen scenario
          {
            if (buzzerPin != -1) 
            {
              tone(buzzerPin, 1000, 100);
            }
          }
        }
        else if (leds[curpos+1][currow+1] == CRGB(0,0,0)) {// if there's two blocks, check only the rightmost
          cursize -= 1; //1 blocks have dropped 
          animatedrop(curpos+1,1);
        }
        else //Nothing fallen, with only two blocks 
        {
          if (buzzerPin != -1)  //If we can buzz
          {
            tone(buzzerPin, 1000, 100);//Buzz
          }
        }
      }
      else { // If there's only one block
         //failState(4);
         //Congrats!  Nothing happens!
            if (buzzerPin != -1) 
            {
              tone(buzzerPin, 1000, 100);
            }
      }
    }
    return false;
  }

  void resetGame() 
  {
    //Begin resetting the scenario
    curpos=2;
    currow = bottom;
    cursize=3;
    interval = defaultInterval;

    // This outer loop will go over each strip, one at a time
    for(int x = 0; x < NUM_STRIPS; x++) {
    // This inner loop will go over each led in the current strip, one at a time
      for(int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
        leds[x][i] = CRGB::Black;
      }
    }
    FastLED.show();

    curMillis = millis();
    prevMillis = curMillis; // The previous time in milliseconds
  }
  void winState() 
  {
    Serial.println("Winstate!");//Output success
    if (buzzerPin != -1) //play a little song if we can
    {
      tone(buzzerPin, 1000, 100);
      delay(100);
      tone(buzzerPin, 1200, 100);
      delay(100);
      tone(buzzerPin, 1000, 100);
    }

    resetGame(); // Reset the game to play again
  }

  void failState(int echo) //Function for the ending of the game
  {
    /*Serial.print("\nFailstate! ");
    Serial.print(echo);
    Serial.print("\n");*/
    //Animate the failure
    animatedrop(curpos,cursize,true);
    resetGame(); //Reset the game to play again
  }

    void animatedrop(int pos, int width) //This is the function that animates the loss of blocks when the player overshoots
    {
      //Serial.println("Animating");
      unsigned long tillAnimation = curMillis + animationDuration;
      unsigned long blinkTimer = curMillis + (animationDuration/8);
      bool blink = false;
      while(tillAnimation > curMillis) 
      {
        curMillis = millis();
        prevMillis = curMillis;

        if (blinkTimer < curMillis) 
        {
          if (blink) //if off
          {
            for (int i = 0; i<width; i++) 
            {
              leds[pos+i][currow] = blockColor;
              
            }
            FastLED.show();
            blink = false;
            blinkTimer = curMillis + (animationDuration/8);
            //Serial.println("Blinking on");

            if (buzzerPin != -1) 
            {
              tone(buzzerPin, 262, (animationDuration/8));
            }
          }
          else 
          {
            for (int i = 0; i<width; i++) 
            {
              leds[pos+i][currow] = CRGB::Black;
              
            }
            FastLED.show();
            //Serial.println("Blinking off");
            if (buzzerPin != -1) 
            {
              tone(buzzerPin, 288, (animationDuration/8));
            }
            blink = true;
            blinkTimer = curMillis + (animationDuration/8);
          }
          FastLED.show();
        }
      }
    }

    void animatedrop(int pos, int width, bool isFail) // This one is only called if it's a fail
    { // This is done to make a different noise, can definitely be written more compactly, but I'm coding this lazily
      //Serial.println("Animating");
      unsigned long tillAnimation = curMillis + animationDuration;
      unsigned long blinkTimer = curMillis + (animationDuration/8);
      unsigned int buzz = 600;
      unsigned int buzzI = 10;
      bool blink = false;
      while(tillAnimation > curMillis) 
      {
        curMillis = millis();
        prevMillis = curMillis;

        if (blinkTimer < curMillis) 
        {
          if (blink) //if off
          {
            for (int i = 0; i<width; i++) 
            {
              leds[pos+i][currow] = blockColor;
              
            }
            FastLED.show();
            blink = false;
            blinkTimer = curMillis + (animationDuration/8);
            //Serial.println("Blinking on");
          }
          else 
          {
            for (int i = 0; i<width; i++) 
            {
              leds[pos+i][currow] = CRGB::Black;
              
            }
            FastLED.show();
            //Serial.println("Blinking off");
            blink = true;
            blinkTimer = curMillis + (animationDuration/8);
          }
          if (buzzerPin != -1) 
          {
            tone(buzzerPin, buzz, (animationDuration/8));
          }
          buzz -= buzzI;
          FastLED.show();
        }
      }
    }

  void buttonPressed() 
  {
    if (value== HIGH) 
    {
      //Serial.println("Button Released");
      clicked = true;
      buttonwait = curMillis;
    }
    else if (value == LOW)
    {
      //Serial.println("Button Pressed");
      if (!clicked) 
      {
      if (currow != bottom) 
      {
        if (checkAir()) 
        {
          return 0;
        }
      }
      else // Using an else statement here to play the sound that check air would play normally on success 
      {
        if (buzzerPin != -1) 
          {
            tone(buzzerPin, 1000, 100);
          }
      }
      currow--; // Going upwards on the rows
      if (currow <= maxHeight ) 
      {
        winState();
        return 0;
      }
      if (currow < (bottom - (top / 4) ) && cursize == 3) 
      {
        cursize--;
      }
      else if (currow < (bottom - (top / 2) ) && cursize == 2) 
      {
        cursize--;
      }
      onUpdate();
      interval -= intervalModifier;
      if (interval < maxSpeed) // prevention from going over the max speed
      {
        interval = maxSpeed;
      }
      clicked = true;
      buttonwait = curMillis;
      }
      
    }
    
  }



void loop() {
  curMillis = millis();
  value = digitalRead((BUTTON_PIN));
  if (value != lastState) {
      lastState = value;
      buttonPressed();
    }
    
    if (curMillis - prevMillis >= interval) //If the interval between updates has been reached
    {
      onUpdate(); // update
    }

    if (clicked) 
    {
      if (curMillis - buttonwait >= 50) 
      {
        clicked = false;
      }
    }
  
    //delay(500); DELAY IS NASTY.  DO NOT USE DELAY
}
