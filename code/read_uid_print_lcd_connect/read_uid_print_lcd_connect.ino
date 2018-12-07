// tubes spektrum 2018
// coba baca uid dan tampilkan ke lcd
// ramhdi, 22/08/2018, 18.30 UTC+7

#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#define SS_PIN D8
#define RST_PIN D3

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

const char* ssid     = "ramhdiAP";
const char* password = "pancasila";
const char* host = "ramhdi026.000webhostapp.com";

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  Wire.begin(D2, D1); //init i2c bus

  lcd.begin();
  lcd.backlight();

  //mulai koneksi wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.clear();
    lcd.print("Connecting...");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("Connected");
  delay(500);

  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  lcd.clear();
  lcd.print("Ready to scan");
}

void loop() {

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  else
  {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    String uid = uidHex2Str(rfid.uid.uidByte, rfid.uid.size);
    lcd.setCursor(0, 0); lcd.print("Card detected");
    lcd.setCursor(0, 1); lcd.print(uid);
    delay(3000);

    //memulai koneksi ke halaman web
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, 80)) {
      Serial.println("Connection failed");
      lcd.clear();
      lcd.print("Connection");
      lcd.setCursor(0,1); lcd.print("failed");
      delay(1500);
    } else {

      // We now create a URI for the request
      String url = "/kuysekre/search.php?uid=" + uid;

      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 30000) {
          Serial.println(">>> Client Timeout !");
          lcd.clear();
          lcd.print("Client timeout");
          client.stop();
          return;
        }
      }

      // Read all the lines of the reply from server and print them to Serial
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        if (line.length() > 2 && line.length() <= 16 && (line[1] == '1' || line[1] == 'U'))
        {
          String msg = line;
          lcd.clear();
          lcd.print(line);
        }
      }
      delay(1500);

      Serial.println();
      Serial.println("Closing connection");
      client.stop();
    }
  }

  lcd.clear();
  lcd.print("Ready to scan");

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

String uidHex2Str(byte *buffer, byte bufferSize) {
  String r = "";
  for (byte i = 0; i < bufferSize; i++) {
    r += (buffer[i] < 0x10 ? "0" : "");
    r += String(buffer[i], HEX);
  }

  return r;
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
