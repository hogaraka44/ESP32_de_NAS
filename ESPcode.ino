//blynk関係
#define BLYNK_TEMPLATE_ID "Your BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Your BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "Your BLYNK_AUTH_TOKEN"
#define BLYNK_PRINT Serial
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <WiFi.h>  //postを送るだけのやつ
#include "HTTPSRedirect.h"

#include "FS.h"   //ファイルシステム　SDカードを使うやつ
#include "SD.h"   //SDカード
#include "SPI.h"  //SPI通信

#include <HTTPClient.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "your ssid";
char pass[] = "your passward";
String lastloadFile = "";

//ESP32で使う時には、ライブラリのHTTPSRedirec.cppの"stop();//may not be required" を活かす
const char *ssid1 = "your ssid";  //redirect用
const char *password1 = "your password";
const String url = "https://script.google.com/macros/s/<id>/exec";
const char *host = "script.google.com";
const int httpsPort = 443;
String payload = "";
HTTPSRedirect *client = nullptr;


String header;  //HTTPリクエストのヘッダーを格納する変数

char str[256];
char *ptr;
BLYNK_WRITE(V7)  //LightPin:Relay_operates_on_LOW_Input_Voltage_coneccted_NC
{
  String string = param.asString();
  ptr = strtok(strcpy(str, string.c_str()), ",");
  String fileName = ptr;
  Serial.println(ptr);
  String func = strtok(NULL, ",");
  if (func != NULL) {
    Serial.println(func);
    String save = "save";
    if (func == save) {
      Serial.println("saveを始めます");
      SaveData(fileName);
      Blynk.virtualWrite(V7, fileName + "にsaveできました");
    } else {
      SendData(fileName);
      Blynk.virtualWrite(V7, fileName + "を開きます");  // send first
    }
  }
}
BLYNK_CONNECTED() {
  Serial.println("Connected!");
}
//SDカードからファイルを読み込む関数
String readFile(fs::FS &fs, const char *path) {
  String str;
  File file = fs.open(path);
  if (!file) {
    Serial.println("Filed to open file for reading");
    return "";
  }
  while (file.available()) {
    str += char(file.read());
  }

  file.close();
  return str;
}

void writeFile(fs::FS &fs, const char *path, const char *message) {  //作った関数

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void setup() {
  //blynkの設定
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  //https1redirectの設定
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);

  WiFi.begin(ssid1, password1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //SDカード設定
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
}

void loop() {
  Blynk.run();
}
void Httpsr_connect() {
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      break;
    } else {
      Serial.println("Connection failed. Retrying...");
      delay(5);
    }
  }

  Serial.print("Successfully Connected to ");
  Serial.println(host);
}

void SendData(String str) {  //GASに送り付ける関数

  Httpsr_connect();

  //ここでGASに送り付ける
  payload = readFile(SD, str.c_str());  //ここでGASに送り付ける strはString型
  if (client->POST(url, host, payload)) {
    Serial.println("Success! send data");
  }
  delete client;
}

void SaveData(String str) {  //GASに送り付けてSDに保存する関数

  Httpsr_connect();

  if (client->GET(url, host, "")) {
    Serial.println("Success! send data");
  }
  Serial.println("save場所は" + str);

  String responseBody = client->getResponseBody();
  writeFile(SD, "/tmp.txt", responseBody.c_str());

  String filestr0 = readFile(SD, "/tmp.txt");  //ここでSDから読み込む
  std::string filestr1 = filestr0.c_str();

  char cstr1[filestr1.size() + 1];
  strcpy(cstr1, filestr1.c_str());

  char *ptr;
  char *ptr2;
  ptr = strtok(cstr1, "@");
  if (ptr != NULL) {
    Serial.print("ここは");
    Serial.println(ptr);
  }
  ptr2 = strtok(NULL, "@");
  if (ptr2 != NULL) {
    Serial.print("ここ2は");
    Serial.println(ptr2);  //ここを保存する

    writeFile(SD, str.c_str(), ptr2);
    Serial.print("SDの中身は");
    Serial.println(readFile(SD, str.c_str()));
  }
}
