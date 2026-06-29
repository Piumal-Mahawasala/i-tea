
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (-1)
#define PN532_RESET (-1)  // Not connected by default on the NFC Shield
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);



void setupNfc(void)
{
Serial.println("Connecting to NFC");
nfc.begin();

uint32_t versiondata = nfc.getFirmwareVersion();
if (!versiondata)
{
  Serial.print("Didn't find PN53x board");
  return;
}
Serial.print("Found chip PN5");
Serial.println((versiondata >> 24) & 0xFF, HEX);
Serial.print("Firmware ver. ");
Serial.print((versiondata >> 16) & 0xFF, DEC);
Serial.print('.');
Serial.println((versiondata >> 8) & 0xFF, DEC);
}

u16 nfc_timer = 0;

void handleNfc(void)
{
if (++nfc_timer >= 100)
  return; // every second
nfc_timer = 0;

uint8_t success;
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
uint8_t dataLength;

success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

if (!success)
  return;

Serial.println("Found an ISO14443A card");
Serial.print(" UID Length: ");
Serial.print(uidLength, DEC);
Serial.println(" bytes");
Serial.print(" UID Value: ");
nfc.PrintHex(uid, uidLength);
Serial.println();

if (uidLength != 7)
{
  Serial.println("This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
  return;
}

uint8_t data[32];
memset(data, 0, 4);

success = nfc.ntag2xx_ReadPage(3, data);
if (!success)
{
  Serial.println("Unable to read the Capability Container (page 3)");
  return;
}
// If the tag has already been formatted as NDEF, byte 0 should be:
// Byte 0 = Magic Number (0xE1)
// Byte 1 = NDEF Version (Should be 0x10)
// Byte 2 = Data Area Size (value * 8 bytes)
// Byte 3 = Read/Write Access (0x00 for full read and write)

if (data[0] != 0xE1 || data[1] != 0x10)
{
  Serial.println("This doesn't seem to be an NDEF formatted tag.");
  Serial.println("Page 3 should start with 0xE1 0x10.");
  return;
}

dataLength = data[2] * 8;
Serial.print("Tag is NDEF formatted. Data area size = ");
Serial.print(dataLength);
Serial.println(" bytes");

// Erase the old data area
Serial.print("Erasing previous data area ");
memset(data, 0, 4);
for (uint8_t i = 0; i < 4; i++)
{
  success = nfc.ntag2xx_WritePage(i + 4, data);
  Serial.print(".");
  if (!success)
  {
    Serial.println(" ERROR!");
    return;
  }
}
Serial.println(" DONE!");

// 6.) Try to add a new NDEF URI record
Serial.print("Writing URI as NDEF Record ... ");
char *url = (char *)"test@something.example";
success = nfc.ntag2xx_WriteNDEFURI(NDEF_URIPREFIX_MAILTO, url, dataLength);
if (success)
{
  Serial.println("DONE!");
}
else
{
  Serial.println("ERROR! (URI length?)");
}
delay(1000);
}

