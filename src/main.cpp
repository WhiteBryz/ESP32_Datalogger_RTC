#include <RTC.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SD_Handle.h>
#include <LiquidCrystal_I2C.h>

#define CS_PIN 5
#define FILE_NAME "/log.json"

DS1307_RTC RTC;

File root;

SDHandle sdHandle;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(void)
{
  Serial.begin(115200); /* Inicializar baudrate a 115200 */
  RTC.RTC_init();       /* Inicializar configuración del reloj */

  Serial.println(F("El DS-1307 ha sido configurado exitosamente."));

  // --- INICIALIZACION DE MÓDULO SD ---
  Serial.print(F("Initializing SD card... "));

  if (!SD.begin(CS_PIN))
  {
    Serial.println(F("Card initialization failed!"));
    while (true)
      ;
  }

  Serial.println(F("Initialization done."));

  Serial.println(F("Files in the card:"));
  root = SD.open("/");
  // PrintDirectory(root, 0);
  Serial.println("");

  if (!SD.exists(FILE_NAME))
  {
    Serial.println(F("Error opening log.json"));
    File jsonDocument = SD.open(FILE_NAME, FILE_WRITE);
    jsonDocument.close();
  }

  // Checar nuevamente
  if (!SD.exists(FILE_NAME))
  {
    Serial.println(F("Problemas para crear el archivo"));
    return;
  }

  lcd.init();
  lcd.backlight();
}

void loop(void)
{
  // File jsonDocument = SD.open(FILE_NAME, FILE_WRITE);
  char outputJson[256];

  RTC.get_time();

  JsonDocument doc;

  doc["fecha"] = RTC.format_date();
  doc["hora"] = RTC.format_time();
  String fecha_s = doc["fecha"];
  String hora_s = doc["hora"];

  lcd.setCursor(0, 0);
  lcd.print("Fecha:");
  lcd.print(fecha_s);
  lcd.setCursor(0, 1);
  lcd.print("Hora:");
  lcd.print(hora_s);

  if (serializeJson(doc, outputJson) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  serializeJson(doc, outputJson);

  sdHandle.appendFile(SD, FILE_NAME, outputJson);

  doc.clear();

  // RTC.show_time(); /* Dar formato y mostrar por consola serial */

  delay(1000);
}
