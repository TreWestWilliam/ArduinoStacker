# ArduinoStacker
A clone of the Arcade Game Stacker, made using the FastLED library and PlatformIO / C++.  Targeting the Arduino UNO
Originally made in the Wowki Arduino Emulator.
Try it out for free, and online here https://wokwi.com/projects/313674725926634049

This project uses NEOPIXEL/WS2812B Individually addressable LED Strip lights for each strip.
You may also attach a buzzer for sounds.
Default configuration is 7 strips with 12 LEDS
You can find these all over Amazon for pretty cheap.
This is the one I bought myself: https://www.amazon.com/gp/product/B07RBVKLPX/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&th=1
(Not sponsored or using an affiliate link)

I haven't tried this project IRL yet, but everything should work, as my UNO has run many more LEDS than the 84 that the project requires.
Might make another version for a real screen.

Pretty much everything is configurable, from the invterval between updates (aka the speed of the cursor) to color and stuff.
Some other stuff like buzzer pitches and stuff could be added to the configurable variables.

Things to add would be a way to do the minor prize, and major prize.  Also potentially could want to add a way to rig the game to simulate rigged Arcades.
Could be fun to hook up an additional button to add plays, then use that with a coinslot to simulate the arcade experience.
