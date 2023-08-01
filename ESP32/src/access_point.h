#ifndef ACCESS_POINT_H_
#define ACCESS_POINT_H_

#include <Arduino.h>
#include <WiFi.h>
#include "url_encode_decode.h"
#include "string_vector.h"

const char *esp_ssid = "ESP32-Access-Point";
const char *esp_password = "123456789";
WiFiServer server(80); // Set web server port number to 80
String builtInLedState = "off";
const int &builtInLed = BUILTIN_LED;

bool openServer()
{
    unsigned long oldBoudRate = Serial.baudRate();
    if (oldBoudRate == 0ul)
    {
        Serial.begin(9600);
    }
    pinMode(builtInLed, OUTPUT);
    digitalWrite(builtInLed, LOW);
    delay(100);
    Serial.print("Setting access point ");
    Serial.println(esp_ssid);

    WiFi.softAPdisconnect(true);
    IPAddress apIP(1, 1, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    const char *hostname = "ESP32 Alarm Clock";
    WiFi.softAPsetHostname(hostname);
    WiFi.softAPConfig(apIP, apIP, subnet);

    uint8_t retries = 0;
    const uint8_t retries_max = 40;
    while ((!WiFi.softAP(esp_ssid, esp_password, 1, 0, 1)) && retries < retries_max)
    {
        retries++;
        Serial.print(".");
        delay(500);
    }

    Serial.println();

    if (retries == retries_max)
    {
        Serial.print("Unable to open access point ");
        Serial.println(esp_ssid);
        // Serial.begin(oldBoudRate);
        return false;
    }

    Serial.print("Successfully opened access point ");
    Serial.print(esp_ssid);
    Serial.print("with password ");
    Serial.println(esp_password);
    Serial.print("the ESP's AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
    //   Serial.begin(oldBoudRate);
    return true;
}

// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
// and a content-type so the client knows what's coming, then a blank line:
void sendOK(WiFiClient client)
{
    if (!client)
    {
        return;
    }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
}

// Display the HTML web page
void sendPage(WiFiClient client)
{
    if (!client)
    {
        return;
    }
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\"><style>");
    client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".btn {margin: 10px;font-size: 30px;font-family: sans-serif;color: white;border: none;padding: 16px 40px;min-width: 100px;cursor: pointer;}");
    client.println(".btn-on {background-color: #4CAF50;} .btn-off {background-color: #555555;} .btn-connect {background-color: #2ecc71;} .btn-connect:hover {background-color: #27ae60;}");
    client.println(".btn-round-2 {border-radius: 20px;} .field-ssid {font-size: 30px;margin: 10px;} .field-password {font-size: 30px;margin: 10px;}");
    client.println("</style></head><body><h1>ESP32 Web Server</h1>");
    client.println("<p>Builtin LED - State " + builtInLedState + "</p>");
    if (builtInLedState == "off")
    {
        client.println("<p><a href=\"/Builtin/on\"><button class=\"btn btn-on\">ON</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/Builtin/off\"><button class=\"btn btn-off\">OFF</button></a></p>");
    }

    client.println("<form method=\"POST\"> \
        <div class=\"field-ssid\">\
            <label for=\"ssid\">SSID (wifi name) - REQUIRED</label>\
            <input type=\"text\" placeholder=\"my WIFI\" name=\"ssid\" id=\"ssid\" required=\"required\" aria-required=\"true\">\
        </div>\
        <div class=\"field-password\">\
            <label for=\"password\">Password (only if there is a password)</label>\
            <input type=\"text\" placeholder=\"1234\" name=\"password\" id=\"password\">\
        </div>\
        <div>\
        <button class=\"btn btn-connect btn-round-2\">Connect</button>\
        </div>\
    </form>");

    client.println("</body></html>");
    client.println();
}

bool hasBody(const String &header)
{
    return header.indexOf("POST /") == 0;
}

String getBody(WiFiClient client)
{
    if (!client)
    {
        return "";
    }
    String body;
    while (client.connected() && client.available())
    {
        char c = client.read();
        Serial.write(c);
        body += c;
        // if (c == '\n') {break;}
    }

    return body;
}

StringVector handleRequest(const String &header, const String &body)
{
    Serial.println("handleRequest" + header.substring(0, 11) + "...");
    StringVector res;
    if (!body.isEmpty())
    {
        int before_ssid = body.indexOf('=');
        int after_ssid = body.indexOf('&');
        String ssid = body.substring(before_ssid + 1, after_ssid);
        String body_password = body.substring(after_ssid + 1);
        int before_password = body_password.indexOf('=');
        String password = body_password.substring(before_password + 1);
        Serial.print("ssid is:");
        Serial.println(ssid);
        Serial.print("password is:");
        Serial.println(password);
        Serial.print("ssid decoded is:");
        Serial.println(urldecode(ssid));
        Serial.print("password  decoded is:");
        Serial.println(urldecode(password));

        res.insert("wifi");
        res.insert(urldecode(ssid));
        res.insert(urldecode(password));
    }
    else
    {
        if (header.indexOf("GET /Builtin/on") == 0)
        {
            Serial.println("Builtin LED on");
            builtInLedState = "on";
            digitalWrite(builtInLed, HIGH);
        }
        else if (header.indexOf("GET /Builtin/off") == 0)
        {
            Serial.println("Builtin LED off");
            builtInLedState = "off";
            digitalWrite(builtInLed, LOW);
        }
        else if (header.indexOf("GET /scan") == 0)
        {
            Serial.println("scan network");
            res.insert("scan");
        }
        else if (header.indexOf("GET /close") == 0)
        {
            Serial.println("closing closing server");
            res.insert("close");
        }
        else if (header.indexOf("GET /") == 0)
        {
            Serial.println("get home page");
            res.insert("home");
        }
    }
    return res;
}

String getHeader(WiFiClient client)
{
    if (!client)
    {
        return "";
    }
    Serial.println("reading the header");
    String header;
    String body;
    bool lineIsEmpty = true; // check if line is empty to know if the whole header was read
    while (client.connected())
    { // loop while the client's connected
        if (!client.available())
        {
            continue;
        }
        // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (lineIsEmpty)
            {
                break;
            }

            lineIsEmpty = true; // if you got a newline, then clear currentLine
        }
        else if (c != '\r')
        {                        // if you got anything else but a carriage return character,
            lineIsEmpty = false; // add it to the end of the currentLine
        }
    }

    Serial.println("got the header");
    return header;
}

StringVector getSSIDandPASSWORD()
{
    WiFiClient client = server.available(); // Listen for incoming clients
    if (!client)
    {
        return StringVector();
    }
    // If a new client connects,
    unsigned long oldBoudRate = Serial.baudRate();
    if (oldBoudRate == 0ul)
    {
        Serial.begin(9600);
    }
    Serial.println("New Client.");
    String header = getHeader(client);
    String body;
    if (hasBody(header))
    {
        Serial.println("recieved [POST /]");
        body = getBody(client);
        Serial.print("body is:");
        Serial.println(body);
    }

    sendOK(client);

    StringVector res = handleRequest(header, body);

    sendPage(client);

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    // Serial.begin(oldBoudRate);
    return res;
}

bool closeServer()
{
    unsigned long oldBoudRate = Serial.baudRate();
    if (oldBoudRate == 0ul)
    {
        Serial.begin(9600);
    }
    Serial.print("closing access point ");
    Serial.println(esp_ssid);

    bool res = WiFi.softAPdisconnect(true);
    if (!res)
    {
        Serial.print("failed to close access point ");
    }
    else
    {
        server.close();
        Serial.print("Successfully closed access point ");
    }
    Serial.print(esp_ssid);

    //   Serial.begin(oldBoudRate);
    return res;
}

#endif // ACCESS_POINT_H_
