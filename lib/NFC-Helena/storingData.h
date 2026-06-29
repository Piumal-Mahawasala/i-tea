#include <iostream>
#include <string>
#include <sstream>

void extractTemperatureAndTime(const std::string &input, int &reqtemp, int &reqmin)
{ // extract the value of Temperature and Time out of the string and store it into variables
    std::stringstream ss(input);
    std::string line;
    while (std::getline(ss, line))
    {
        std::stringstream lineStream(line);
        std::string key;
        int value;

        if (lineStream >> key >> value)
        {
            if (key == "Temperature:")
                reqtemp = value;
            else if (key == "Time:")
                reqmin = value;
        }
    }
}

int main()
{

    int reqtemp = 0;
    int reqmin = 0;

    extractTemperatureAndTime(payloadAsString, reqtemp, reqmin);

    std::cout << "Temperature: " << reqtemp << "C" << std::endl;
    std::cout << "Time: " << reqmin << " min" << std::endl;

    return 0;
    delay(10000);
}

void read_show_nfc()
{

    nfc.begin();

    Serial.println("\nPut tea box left\n"); // Command so that you an others will know what to do

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("SSD1306 allocation failed");
        while (true)
            ;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 28);
    display.println(print);
    display.display();

    int a = 0;

    while (a == 0)
    {
        if (nfc.tagPresent())
        {
            NfcTag tag = nfc.read();
            Serial.println(tag.getTagType());
            Serial.print("UID: ");
            Serial.println(tag.getUidString()); // Retrieves the Unique Identification from your tag

            if (tag.hasNdefMessage()) // If your tag has a message
            {

                NdefMessage message = tag.getNdefMessage();
                Serial.print("\nThis Message in this Tag is ");
                Serial.print(message.getRecordCount());
                Serial.print(" NFC Tag Record");
                if (message.getRecordCount() != 1)
                {
                    Serial.print("Error: NFC tag does not contain NDEF message");
                }
                if (print != "")
                {
                    a = 1;
                }
                else
                {
                    Serial.println("Error: NFC tag does not contain NDEF message");
                }
            }
            // If you have more than 1 Message then it wil cycle through them
            int recordCount = message.getRecordCount();
            for (int i = 0; i < recordCount; i++)
            {
                Serial.print("\nNDEF Record ");
                Serial.println(i + 1);
                NdefRecord record = message.getRecord(i);

                int payloadLength = record.getPayloadLength();
                byte payload[payloadLength];
                record.getPayload(payload);

                String payloadAsString = ""; // Processes the message as a string vs as a HEX value
                for (int c = 0; c < payloadLength; c++)
                {
                    payloadAsString += (char)payload[c];
                }
                Serial.print("  Information (as String): ");
                Serial.println(payloadAsString);
                print = payloadAsString;

                String uid = record.getId();
                if (uid != "")
                {
                    Serial.print("  ID: ");
                    Serial.println(uid); // Prints the Unique Identification of the NFC Tag
                }
                if (print != "")
                {
                    a = 1;
                }
            }
        }
    }
}

delay(10000);
}

void start_write_nfc()
{
    Serial.println("NDEF Writer");
    nfc.begin();
}

void write_nfc()
{
    Serial.println("\nPlace a formatted Mifare Classic or Ultralight NFC tag on the reader.");
    if (nfc.tagPresent())
    {
        NdefMessage message = NdefMessage();
        message.addUriRecord("Tea: Earl Gray\nTemperature: 90C\nTime: 2 min");

        bool success = nfc.write(message);
        if (success)
        {
            Serial.println("Success. Try reading this tag with your phone.");
        }
        else
        {
            Serial.println("Write failed.");
        }
    }
    delay(5000);
}
