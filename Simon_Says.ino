/* Simon Says Custom Project
 *  This uses the following items:
 *  - 1 LiquidCrystal LCD display
 *  - 4 Buttons
 *  - 1 RGB LED
 *  - 1 Piezo Buzzer
 *  - 1 Potentiometer
 *  - A bunch of wires and resistors
 *  
 *  This was a long, yet fun project to show off. This
 *  code does also store data in EEPROM and will max out
 *  if it reaches a certain level, have fun!!.
 */

#include <LiquidCrystal.h>                                                        // Header file for lcd display.
LiquidCrystal lcd = LiquidCrystal(12, 6, 5, 4, 3, 2);                             // New object lcd to print to display.
class eeprom {                                                                    // Custom EEPROM class file.
  public:
    eeprom() {}                                                                   // Default Constructor.
    unsigned char read(unsigned int address){                                     // Read data with address as the parameter and return an unsigned char.
      while (EECR & (1 << EEPE));
      EEAR = address;
      EECR |= (1 << EERE);
      return EEDR;
    }
    void write(unsigned int address, unsigned char data){                         // Write to EEPROM using a void function with address and data as parameters.
      while (EECR & (1 << EEPE));
      EEAR = address;
      EEDR = data;
      EECR |= (1 << EEMPE);
      EECR |= (1 << EEPE);
    }
};

void myDelay(unsigned long time_ms) {                                             // Custom delay function using registers.
  unsigned long index = 0;
  while (index != time_ms) {
    SREG &= ~(1 << 7);
    cli();
    TCNT0 = 6;
    TCCR0A &= ~(1 << 0);
    TCCR0A &= ~(1 << 1);
    TCCR0B &= ~(1 << 3);
    TCCR0B &= ~(1 << 2);
    TCCR0B |= (1 << 1);
    TCCR0B |= (1 << 0);
    while ((TIFR0 & (1 << 0)) == 0);
    TCCR0B &= ~(1 << 2);
    TCCR0B &= ~(1 << 1);
    TCCR0B &= ~(1 << 0);
    TIFR0 |= (1 << 0);
    SREG |= (1 << 7);
    index++;
  }
}

void myPinMode(byte pin_number, unsigned char data) {                             // Custom pinMode function.
  if (data == INPUT) {                                                            // If data is input, then set the DDR(_) buffer to 0 (input), else set it to output.
    if (pin_number >= 0 && pin_number <= 7)
      DDRD &= ~(1 << (pin_number));
    else if (pin_number >= 8 && pin_number <= 13)
      DDRB &= ~(1 << (pin_number - 8));
    else if (pin_number >= 14 && pin_number <= 19)
      DDRC &= ~(1 << (pin_number - 14));
  }
  else {
    if (pin_number >= 0 && pin_number <= 7)
      DDRD |= (1 << pin_number);
    else if (pin_number >= 8 && pin_number <= 13)
      DDRB |= (1 << (pin_number - 8));
    else if (pin_number >= 14 && pin_number <= 19)
      DDRC |= (1 << (pin_number - 14));
  }
}

byte myDigitalRead(byte pin_number) {                                             // Custom digitalRead that will return either a 1 or a 0 by bit shifts.
  if (pin_number >= 0 && pin_number <= 7)
    return (PIND & (1 << pin_number)) >> pin_number;
  else if (pin_number >= 8 && pin_number <= 13)
    return (PINB & (1 << pin_number - 8)) >> (pin_number - 8);
  else if (pin_number >= 14 && pin_number <= 19)
    return (PINC & (1 << pin_number - 14)) >> (pin_number - 14);
}

void myDigitalWrite(byte pin_number, unsigned char pin) {                         // Custom digitalWrite where it checks the pin, and whether its HIGH or LOW, then it will set 1 for high, or 0 for low.
  if (pin_number >= 0 && pin_number <= 7) {
    if (pin == HIGH)
      PORTD |= (1 << pin_number);
    else
      PORTD &= ~(1 << pin_number);
  }
  else if (pin_number >= 8 && pin_number <= 13) {
    if (pin == HIGH)
      PORTB |= (1 << (pin_number - 8));
    else
      PORTB &= ~(1 << (pin_number - 8));
  }
  else if (pin_number >= 14 && pin_number <= 19) {
    if (pin == HIGH)
      PORTC |= (1 << (pin_number - 14));
    else
      PORTC &= ~(1 << (pin_number - 14));
  }
}

eeprom EEPROM;                                                                    // Object to store and read from EEPROM.
const short ROUNDS = 10;
const byte max_buttons = 4;                                                       // Declaring Constant values.
const byte max_led_pins = 3;
const byte buzzer = A2;
byte buttons[max_buttons] = {A0, 7, 8, A1};                                       // Setting the arrays.
byte ledPins[max_led_pins] = {11, 10, 9};
byte READ[max_buttons];
short pattern[ROUNDS];
byte pace = 150;                                                                  // Declare pace value, and use wins and games as public variables.
byte wins;
byte games;

void makePattern() {                                                              // Generate the pattern.
  for (byte i = 0; i < ROUNDS; i++) 
    pattern[i] = random(4);
}

void buzzfun(int ton, unsigned long micro, int del) {                             // Automatic buzz function.
  tone(buzzer, ton, del);
  myDelay(del);
  micro = micro / 1000;
  delayMicroseconds(micro);
  tone(buzzer, ton, del);
  myDelay(del);
}

void clearLED() {                                                                 // Clear leds.
  myDigitalWrite(ledPins[0], LOW);
  myDigitalWrite(ledPins[1], LOW);
  myDigitalWrite(ledPins[2], LOW);
}

void showLED(char color) {                                                        // Display the led color.
  switch(color){
    case 'R':
      myDigitalWrite(ledPins[0], HIGH);
      buzzfun(440, 1136, pace);
      break;
    case 'G':
      myDigitalWrite(ledPins[1], HIGH);
      buzzfun(784, 638, pace);
      break;
    case 'B':
      myDigitalWrite(ledPins[2], HIGH);
      buzzfun(587, 851, pace);
      break;
    case 'Y':
      myDigitalWrite(ledPins[0], HIGH);
      myDigitalWrite(ledPins[1], HIGH);
      buzzfun(880, 568, pace);
      break;
  }
  clearLED();
  myDelay(pace);
}

void printPattern(byte num) {                                                     // Print the pattern.
  for (int i = 0; i < num; i++) {
    byte show = pattern[i];
    switch (show) {
      case 0:
        showLED('R');
        break;
      case 1:
        showLED('G');
        break;
      case 2:
        showLED('B');
        break;
      case 3:
        showLED('Y');
        break;
    }
    clearLED();
    myDelay(pace);
  }
}

void fail() {                                                                     // If the user fails to match, they'll be stuck here, with a game counted.
  for (byte i = 0; i < 3; i++) {
    myDigitalWrite(ledPins[0], HIGH);
    myDelay(100);
    myDigitalWrite(ledPins[0], LOW);
    buzzfun(440, 1136, 150);
  }
  games++;
  EEPROM.write(0, games);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mission Failed!!");
  lcd.setCursor(0, 1);
  lcd.print("Press Reset!");
  while (1);
}

void checker(byte checkpat) {                                                     // Checks the pattern and determines if player loses.
  bool gameflag = false;
  int selection;
  for (int i = 0; i < checkpat; i++) {
    gameflag = false;
    while (gameflag == false) {
      for (int j = 0; j < max_buttons; j++)
        READ[j] = myDigitalRead(buttons[j]);
      if (READ[0] == HIGH) {                                                      // If the read is high, then the flag will be set to true, and the select variable will be used to make comparison.
        gameflag = true;
        showLED('R');
        selection = 0;
      }
      if (READ[1] == HIGH) {
        gameflag = true;
        showLED('G');
        selection = 1;
      }
      if (READ[2] == HIGH) {
        gameflag = true;
        showLED('B');
        selection = 2;
      }
      if (READ[3] == HIGH) {
        gameflag = true;
        showLED('Y');
        selection = 3;
      }
    }
    clearLED();
    if (selection != pattern[i])                                                  // If the selection does not match, then it will auto default to fail state.                                        
      fail();
  }
  myDelay(pace);
}

void gamePrint(byte number) {                                                     // Make total games print nicely on lcd.
  if (number > 999)
    lcd.print(999);
  else {
    if (number < 10) 
      lcd.print(" ");
    if (number < 100) 
      lcd.print(" ");
    lcd.print(number);
  }
}

void printNum(byte number) {                                                      // For printing numbers less than 99 on lcd.
  if (number > 99)
    lcd.print(99);
  else if (number < 10) {
    lcd.print(" ");
    lcd.print(number);
  }
  else
    lcd.print(number);
}

void finish() {                                                                   // If the player makes it all the way through, they will end up here.
  /* Idea from zimmermannard */
  byte r = 255, b = 0, g = 0;
  byte transitionDelay = 2;
  games++;
  wins++;
  EEPROM.write(0, games);
  EEPROM.write(1, wins);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("We have a Winner");
  lcd.setCursor(0, 1);
  lcd.print("Congratulations!");
  myPinMode(buzzer, INPUT);                                                       // Prevent faint noise from buzzer.
  while (1) {
    for (; r >= 0, b < 255; b++, r--) {
      analogWrite(ledPins[0], r);
      analogWrite(ledPins[2], b);
      myDelay(transitionDelay);
    }
    for (; b >= 0, g < 255; g++, b--) {
      analogWrite(ledPins[2], b);
      analogWrite(ledPins[1], g);
      myDelay(transitionDelay);
    }
    for (; g >= 0, r < 255; r++, g--) {
      analogWrite(ledPins[0], r);
      analogWrite(ledPins[1], g);
      myDelay(transitionDelay);
    }
  }
}

void setup() {                                                                      // Setup function.
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  randomSeed(analogRead(0));
  games = EEPROM.read(0);
  wins = EEPROM.read(1);
  for (byte i = 0; i < max_buttons; i++)
    myPinMode(buttons[i], INPUT);
  for (byte i = 0; i < max_led_pins; i++)
    myPinMode(ledPins[i], OUTPUT);
  makePattern();
  myPinMode(buzzer, OUTPUT);
}


void loop() {                                                                       // Loop function.
  byte disp = -1;                                                                   // Show the current round, set to -1 because of something weird going on.
  for (int i = 0; i <= ROUNDS; i++) {
    disp++;
    lcd.print("Played Games:");
    gamePrint(games);
    lcd.setCursor(0, 1);
    lcd.print("Wins:");
    printNum(wins);
    lcd.print("|Round:");
    printNum(disp);
    printPattern(i);                                                                // Goes to print pattern.
    checker(i);                                                                     // Then goes to checker, once it goes through successfully, it will then shorten the pace.
    pace -= 2;
  }
  finish();
}
