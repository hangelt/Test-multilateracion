#include <WiFi.h>
#include <WiFiUdp.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>

// the IP of the machine to which you send msgs - this should be the correct IP in most cases (see note in python code)
#define CONSOLE_IP "192.168.1.127"
#define CONSOLE_PORT 12345
#define INTERVAL   250
#define ID    1

const char* ssid = "Cisco";
const char* password = "CiscoCisco";

WiFiUDP Udp;
IPAddress local_ip(192, 168, 1, 101);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
unsigned int localPort = 9999;
byte counter;
int instantValue;
unsigned long timer;

byte txBuffer[2];

void setup()
{
  Serial.begin(115200);
  // Connect to Wifi network.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500); Serial.print(F("."));
  }
  Udp.begin(localPort);
  Serial.printf("UDP server : %s:%i \n", WiFi.localIP().toString().c_str(), localPort);

  if (ELECHOUSE_cc1101.getCC1101()){         // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  }
  else{
    Serial.println("Connection Error");
  }

  ELECHOUSE_cc1101.Init();              // must be set to initialize the cc1101!
  ELECHOUSE_cc1101.setMHZ(433.955);
  ELECHOUSE_cc1101.setDRate(2.4);      // 2.4 kBaud
  ELECHOUSE_cc1101.setRxBW(325);       // 325 kHz
  //Serial.println("Rx Mode");
  ELECHOUSE_cc1101.SetRx();
  
  timer = millis();
}

void loop()
{
  if((millis() - timer) >= INTERVAL)
  {
    instantValue = ELECHOUSE_cc1101.getRssi();
    //Serial.println(instantValue);
    prepareTxBuffer(instantValue);
    Udp.beginPacket(CONSOLE_IP, CONSOLE_PORT);
    Udp.write(txBuffer, sizeof(txBuffer));
    Udp.endPacket();
    timer = millis();
  }
}

void prepareTxBuffer(int iV)
{
  txBuffer[0] = ID;
  txBuffer[1] = abs(iV);
}
