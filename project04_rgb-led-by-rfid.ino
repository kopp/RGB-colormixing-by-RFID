/*
 * Use different RFID tags to increase the red/green/blue brightness of a RGB led or to reset it back to "off".
 */

#include <SPI.h>
#include <MFRC522.h>


// RFID tags to use to control the colors.
MFRC522::Uid const RFID_TAG_RED{7, {0x4, 0xCA, 0xA9, 0xBA, 0xC1, 0x58, 0x80}, 0};
MFRC522::Uid const RFID_TAG_GREEN{7, {0x4, 0x8A, 0xD, 0x72, 0xC0, 0x5D, 0x84}, 0};
MFRC522::Uid const RFID_TAG_BLUE{7, {0x4, 0x27, 0x9B, 0xBA, 0xC1, 0x58, 0x80}, 0};
MFRC522::Uid const RFID_TAG_BACK{7, {0x4, 0xE3, 0x73, 0x5A, 0xC0, 0x5D, 0x80}, 0};

MFRC522::Uid const RFID_TAG_WHITE_CARD{4, {0xE4, 0xF6, 0x9C, 0x2A}, 0};



// WIRING RFID-RC522 -> Arduino Uno/Nano
// SDA -> 10
constexpr int SDA_PIN = 10;
// SCK -> 13
// MOSI -> 11
// MISO -> 12
// IRQ -> Not Connected
// GND -> GND
// 3V3 -> 3V3  // IMPORTANT: Do not use 5V!
// RST -> 9
constexpr int RST_PIN = 9;

// WIRING RGB LED -> Arduino
// GND -> GND
// red -- 220 Ohm -> PIN_RED
constexpr int PIN_RED = 3;
// green -- 220 Ohm -> PIN_GREEN
constexpr int PIN_GREEN = 5;
// blue -- 220 Ohm -> PIN_BLUE
constexpr int PIN_BLUE = 6;


void set_rgb_colors(int red, int green, int blue)
{
  analogWrite(PIN_RED, red);
  analogWrite(PIN_GREEN, green);
  analogWrite(PIN_BLUE, blue);
}


MFRC522 mfrc522(SDA_PIN, RST_PIN);


/** Compare given UID with uid currently known to the mfrc class. */
bool is_uid(MFRC522 const & mfrc, MFRC522::Uid const & uid)
{
  if (mfrc.uid.size != uid.size)
  {
    return false;
  }

  for (int i = 0; i < uid.size; ++i) {
    if (mfrc522.uid.uidByte[i] != uid.uidByte[i])
    {
      return false;
    }
  }

  // ignore 'sak' byte

  return true;
}


void print_uid(MFRC522::Uid const & uid)
{
  Serial.print("ID ");
  Serial.print(" (size ");
  Serial.print(uid.size);
  Serial.print("): ");

  for (int i = 0; i < uid.size; i++)
  {
    Serial.print(uid.uidByte[i], HEX);
    Serial.print(" ");
  }
}



int red = 0;
int green = 0;
int blue = 0;


void increment(int& value)
{
  if (value == 0)
  {
    value = 1;
  }
  else {
    value *= 2;
  }
  if (value > 255) {
    value = 255;
  }
}



void setup() // Beginn des Setups:
{

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);


  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}



void loop()
{
  if (mfrc522.PICC_IsNewCardPresent())
  {
    Serial.print("Karte DA; ");
  }
  else {
    Serial.println("keine Karte; ");
    return;
  }


  if (mfrc522.PICC_ReadCardSerial())
  {
    Serial.print("ausgelesen; ");
  }
  else
  {
    Serial.println("FEHLER beim Lesen");
    return;
  }

  bool something_changed = false;

  print_uid(mfrc522.uid);
  Serial.println();

  if (is_uid(mfrc522, RFID_TAG_RED))
  {
    Serial.println("RED");
    increment(red);
    something_changed = true;
  }
  if (is_uid(mfrc522, RFID_TAG_GREEN))
  {
    Serial.println("GREEN");
    increment(green);
    something_changed = true;
  }
  if (is_uid(mfrc522, RFID_TAG_BLUE))
  {
    Serial.println("BLUE");
    increment(blue);
    something_changed = true;
  }

  if (is_uid(mfrc522, RFID_TAG_BACK))
  {
    Serial.println("RESET");
    red = 0;
    green = 0;
    blue = 0;
    something_changed = true;
  }

  Serial.print("Colors: red: ");
  Serial.print(red);
  Serial.print(", green: ");
  Serial.print(green);
  Serial.print(", blue: ");
  Serial.print(blue);
  Serial.println();


  if (something_changed) {
    set_rgb_colors(red, green, blue);
    delay(250);
  }

}
