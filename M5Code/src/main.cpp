#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <M5StickC.h>

using namespace websockets;

const char *websockets_server_host = "192.168.137.1"; //Enter server adress
const uint16_t websockets_server_port = 8000;         // Enter server port

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

WebsocketsClient client;

// Level1 - Connection
bool connected = false;
String data = "";
// Set your Static IP address
IPAddress local_IP(192, 168, 137, 10);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 255, 0);

// Level2 - Data
String dLine1 = "CONNECTED";
String dLine2 = "Waiting for data...";
int dLine1Sz = 2; // Font size line 1
int dLine2Sz = 1; // Font size line 2
int dLine1Ft = 1; // Font line 1
int dLine2Ft = 1; // Font line 2

// Level3 - UI
bool hasNewData = true;

TaskHandle_t uiTask;

void uiTaskLoop(void *parameter)
{
  for (;;)
  {
    if (!connected)
    {
      M5.Lcd.fillRect(40, 50, 20, 20, GREENYELLOW);
      delay(500);
      M5.Lcd.fillRect(40, 50, 20, 20, WHITE);
      M5.Lcd.fillRect(70, 50, 20, 20, GREENYELLOW);
      delay(500);
      M5.Lcd.fillRect(70, 50, 20, 20, WHITE);
      M5.Lcd.fillRect(100, 50, 20, 20, GREENYELLOW);
      delay(500);
      M5.Lcd.fillRect(100, 50, 20, 20, WHITE);
    }
    else
    {
      if (hasNewData)
      {
        hasNewData = false;
        M5.Lcd.fillScreen(WHITE);

        // Line 1
        M5.Lcd.setTextSize(dLine1Sz);  // Font Size
        M5.Lcd.setTextDatum(CC_DATUM); // Alignments
        M5.Lcd.drawString(dLine1, (int)(M5.Lcd.width() / 2), 20, dLine1Ft);

        // Line 2
        M5.Lcd.setTextFont(dLine2Ft);  // Font Choice
        M5.Lcd.setTextSize(dLine2Sz);  // Font Size
        M5.Lcd.setTextDatum(CC_DATUM); // Alignments
        M5.Lcd.drawString(dLine2, (int)(M5.Lcd.width() / 2), 60, dLine2Ft);
      }
    }
    delay(1);
  }
}

// the setup routine runs once when M5StickC starts up
void setup()
{

  // initialize the M5StickC object
  M5.begin();
  pinMode(M5_BUTTON_RST, INPUT);

  // Lcd display
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(25, 20);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextFont(1);
  M5.Lcd.printf("CONNECTING");
  delay(500);

  // Start stuff
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
      uiTaskLoop, /* Function to implement the task */
      "uiTask",   /* Name of the task */
      10000,      /* Stack size in words */
      NULL,       /* Task input parameter */
      0,          /* Priority of the task */
      &uiTask,    /* Task handle. */
      0);         /* Core where the task should run */
  delay(1000);

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(25, 20);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.printf("WifiDOWN");
  }

  WiFi.begin("<SSID>", "<PW>");

  // Wait some time to connect to wifi
  for (int i = 0; i < 25 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(WiFi.status());
  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connector = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connector)
  {
    Serial.println("Connected!");
    client.send("register");
  }
  else
  {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(25, 20);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.printf("WifiUP/SrvDOWN");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    data = message.data();
    Serial.print("Got Message: ");
    Serial.println(data);
    if (data == "sure thing buddy")
    {
      connected = true;
    }
    if (data.indexOf(",") > 0)
    {
      dLine1 = getValue(data, ',', 0);
      dLine1Sz = getValue(data, ',', 1).toInt();
      dLine1Ft = getValue(data, ',', 2).toInt();
      dLine2 = getValue(data, ',', 3);
      dLine2Sz = getValue(data, ',', 4).toInt();
      dLine2Ft = getValue(data, ',', 5).toInt();
      hasNewData = true;
    }
  });
}

// the loop routine runs over and over again forever
void loop()
{
  if (client.available())
  {
    client.poll();
  }
  if (digitalRead(M5_BUTTON_RST) == LOW)
  {
    ESP.restart();
  }
  if (M5.Axp.GetWarningLevel())
  {
    dLine1 = "Warning";
    dLine1Sz = 2;
    dLine1Ft = 1;
    dLine2 = "low battery";
    dLine2Sz = 2;
    dLine2Ft = 1;
    hasNewData = true;
  }
  delay(10);
  //M5.update();
}
