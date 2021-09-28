//base123
#include "defines.h"
#include "midi_serial.h"
const int Switch=PB12;
static const unsigned ledPin = PC13;      // LED pin on Arduino Uno

const int Output_row[5]={PA4,PA5,PA6,PA7,PA8};
const int Input_row[4]={PA0,PA1,PA2,PA3};

#include <USBComposite.h> 
USBMIDI midi_usb;

USBHID HID;
HIDKeyboard BootKeyboard(HID,0);
bool Buttons[5][4];
byte Key[5][4]={ {194,199,204,214}, //
                {195,200,205,215}, //
                {196,201,210,216},
                {197,202,211,217},
                {198,203,213,218}};
byte MIDI_CC[5][4]={  {1,6,11,16}, //
                      {2,7,12,17}, //
                      {3,8,13,18},
                      {4,9,14,19},
                      {5,10,15,20}};
  
void setup() 
{
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, HIGH);
pinMode(Switch,INPUT_PULLDOWN);
  // put your setup code here, to run once:
for (int i=0;i<=3;i++)
  {
  pinMode(Input_row[i], INPUT_PULLDOWN);  
  }
for (int i=0;i<=4;i++)
  {
  pinMode(Output_row[i], OUTPUT);  
  }
  
if(digitalRead(Switch)==false) // hid клавиатура
  {      
  HID.begin(HID_BOOT_KEYBOARD);
  BootKeyboard.begin(); // needed just in case you need LED support      
  }
else // midi клавиатура
  {   
  //USBComposite.setVendorId(2);
  USBComposite.setManufacturerString("Razor_Soundpad_Midi");
  USBComposite.setProductString("Razor_Soundpad_Midi");
  USBComposite.setSerialString("Razor_Soundpad_Midi");
  USBComposite.setProductId(0x0031);      
  midi_usb.begin();      
  while (!USBComposite);  
  midi_serial.begin(MIDI_CHANNEL_OMNI); // Launch MIDI and listen to all channels
  }
}

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long WaitDelay = 500;
unsigned long DelayTime = 0;
bool lastbuttonState[5][4];  
byte button_click[5][4];
int Delay=1;


void loop() 
{
for(byte j=0;j<=4;j++)
  {
  digitalWrite(Output_row[j],true);
  for (byte i=0;i<=3;i++)
    {      
    Buttons[j][i] = digitalRead(Input_row[i]);
    if (Buttons[j][i] == HIGH) 
      {
      lastDebounceTime = millis(); // нажата кнопка - дебоунсим
      lastbuttonState[j][i]=HIGH;
      }
    if ((millis() - lastDebounceTime) > debounceDelay) // стало больше время дебоунса прошло
      {
      if (Buttons[j][i] == LOW && lastbuttonState[j][i]==HIGH)
        {        
        //----------------------------------------------------------------------------------------------
        if (digitalRead(Switch)==true) 
          {
          midi_usb.sendControlChange(1,MIDI_CC[j][i],127);  
          midi_serial.sendControlChange(MIDI_CC[j][i], 127, 1);
          }
        button_click[j][i]=button_click[j][i]+1;  
        lastbuttonState[j][i]=LOW;
        DelayTime=millis()+WaitDelay; //время через которое поступит команда если ничего другого не нажато
        }
      //----------------------------------------------------------------------------------- посыл только в режиме клавиатуры
      if(millis()>DelayTime && button_click[j][i]!=0 && digitalRead(Switch)==false)
        {
        if(button_click[j][i]==1) 
          {
          BootKeyboard.press(KEY_LEFT_CTRL);
          BootKeyboard.press(KEY_LEFT_SHIFT);
          }
        delay(Delay);
        if(button_click[j][i]==2) 
          {
          BootKeyboard.press(KEY_LEFT_CTRL);
          BootKeyboard.press(KEY_LEFT_ALT);
          }
        if(button_click[j][i]==3) 
          {
          BootKeyboard.press(KEY_LEFT_CTRL);
          BootKeyboard.press(KEY_LEFT_ALT);          
          BootKeyboard.press(KEY_LEFT_SHIFT);
          }      
        delay(Delay);
        BootKeyboard.press(Key[j][i]);
        delay(Delay);
        BootKeyboard.release(Key[j][i]);            
        delay(Delay);
        if(button_click[j][i]==1) 
          {
          BootKeyboard.release(KEY_LEFT_CTRL);
          BootKeyboard.release(KEY_LEFT_SHIFT);
          }
        delay(Delay);
        if(button_click[j][i]==2) 
          {
          BootKeyboard.release(KEY_LEFT_CTRL);
          BootKeyboard.release(KEY_LEFT_ALT);
          } 
        if(button_click[j][i]==3) 
          {
          BootKeyboard.release(KEY_LEFT_CTRL);
          BootKeyboard.release(KEY_LEFT_ALT);          
          BootKeyboard.release(KEY_LEFT_SHIFT);
          }  
        button_click[j][i]=0;
        }
      }    
    }
  digitalWrite(Output_row[j],false);
  }  
/*if(digitalRead(Switch)==false) // hid клавиатура
  {     
  Keyboard.press(KEY_RIGHT_ARROW);
  Keyboard.release(KEY_RIGHT_ARROW);  
  }
  */
}
