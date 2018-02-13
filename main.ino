// ----- настройки -----

#define DI_PIN                          // пин подключения ленты
#define NUM_LEDS 10                     // количество светодиодов в ленте

#define MOTOR_PIN                       // пин подключения потора

#define start_flashes 1                 // проверка цветов при запуске (1 - включить, 0 - выключить)

// ---- \настройки/ ----
#include <SoftwareSerial.h>
#include <FastLED-3.1.8/FastLED.h>

String strData = "";

CRGB leds[NUM_LEDS];  // создаём ленту

void setup() {
    FastLED.addLeds<WS2812, DI_PIN, GRB > (leds, NUM_LEDS);  // инициализация светодиодов

    // вспышки красным синим и зелёным при запуске (можно отключить)
    if (start_flashes) {
        LEDS.showColor(CRGB(255, 0, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 255, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 255));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 0));
    }

    Serial.begin(9600);

    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.print("Hi!!!\n");
}

void loop() {
    for (;;) {         // главный подцикл, ожидает данные в Serial и отрабатывает кнопку
        availableBytes = Serial.available();         // считаем, сколько байт в буфере
        if (availableBytes > 0) {      // если есть что то на вход
            delay(200);                                // ждём, пока придут остальные символы
            availableBytes = Serial.available();       // обновляем число байт в буфере
            strData = "";                              // очистить строку
            for (int i = 0; i < availableBytes; i++) {
                char newByte = Serial.read();
                strData += newByte;                      // забиваем строку принятыми данными
            }
            break;                                     // выходим из цикла
        }
    }

    switch (strData) {
        case "stop":
//            отключаем ленту
            LEDS.showColor(CRGB(0, 0, 0));
//            включаем мотор
            analogWrite(MOTOR_PIN, 0);
            break;

        case "start":
//            показываем радугу на первых 7 лампочках
            leds[0].setHue(CHSV::HUE_PURPLE);
            leds[1].setHue(CHSV::HUE_BLUE);
            leds[2].setHue(CHSV::HUE_AQUA);
            leds[3].setHue(CHSV::HUE_GREEN);
            leds[4].setHue(CHSV::HUE_YELLOW);
            leds[5].setHue(CHSV::HUE_ORANGE);
            leds[6].setHue(CHSV::HUE_RED);

//            "да будет движение" - сказал Ньютон и включил моторчик
            analogWrite(MOTOR_PIN, 1023);
            break;
        default:
//            если количество символов не соответствует количеству лампочек
            if (strData.length() != NUM_LEDS)
                break;

            for (int i = 0; i < NUM_LEDS; i++) {

//                f0a => 255, 0, 170 # 3 лампочки, одна гореть будет фиолетовеньким, другая красиньким, третья каким-то...
//                используем почти безопасные цвета, но каждый пиксель кодируем одним символом
                unsigned long number = strtoul(strData[i], NULL, 16);
                long color = number << 4 & number;

//                начинаем
                leds[i].setHue(color);

            }

//            записываем в ленточку
            FastLED.show();

            break;
    }


}