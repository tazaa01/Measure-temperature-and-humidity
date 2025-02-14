#include <dummy.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// กำหนดพินที่ใช้กับ DHT11
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// กำหนดข้อมูล Wi-Fi
const char* ssid = "ita";
const char* password = "sikibidi1";

// สร้างเว็บเซิร์ฟเวอร์ที่พอร์ต 80
ESP8266WebServer server(80);

void setup() {
  // เริ่มต้น Serial Monitor
  Serial.begin(115200);
  
  // เชื่อมต่อกับ Wi-Fi
  WiFi.begin(ssid, password);
  
  // รอจนกว่า ESP8266 จะเชื่อมต่อกับ Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // เมื่อเชื่อมต่อเสร็จแล้ว ให้แสดง IP
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // แสดง IP ของ ESP8266

  // เริ่มต้นเซ็นเซอร์ DHT
  dht.begin();

  // ตั้งค่าเว็บเซิร์ฟเวอร์ให้ส่งข้อมูลเกี่ยวกับความชื้น
  server.on("/", HTTP_GET, []() {
    String html = "<html><head>";
    html += "<style>";
    html += "body { background-color: #f0f8ff; font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }";
    html += "h1 { color: #4682b4; }";
    html += "p { font-size: 24px; color: #333; }";
    html += "footer { margin-top: 30px; font-size: 14px; color: #888; }";
    html += "</style>";
    html += "<script>";
    html += "function updateHumidity() {";
    html += "  fetch('/humidity').then(response => response.text()).then(data => {";
    html += "    document.getElementById('humidity').innerText = 'Humidity: ' + data + ' %';";
    html += "  });";
    html += "}";
    html += "setInterval(updateHumidity, 5000);"; // อัปเดตทุกๆ 5 วินาที
    html += "window.onload = updateHumidity;";
    html += "</script>";
    html += "</head><body>";
    html += "<h1>Humidity Monitoring</h1>";
    html += "<p id='humidity'>Humidity: Loading...</p>"; // ข้อความเริ่มต้น
    html += "<footer>ESP8266 - DHT11 Sensor</footer>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // ตั้งค่าให้เซิร์ฟเวอร์ส่งค่าความชื้น
  server.on("/humidity", HTTP_GET, []() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
      server.send(500, "text/plain", "Failed to read from DHT sensor!");
    } else {
      server.send(200, "text/plain", String(humidity));
    }
  });

  // เริ่มต้นเว็บเซิร์ฟเวอร์
  server.begin();
}

void loop() {
  server.handleClient();  // ให้เว็บเซิร์ฟเวอร์ทำงาน
}