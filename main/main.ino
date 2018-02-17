// ----- настройки -----

#define DI_PIN 7                        // пин подключения ленты
#define NUM_LEDS 10                     // количество светодиодов в ленте

#define MOTOR_PIN 3                     // пин подключения потора

#define start_flashes 1                 // проверка цветов при запуске (1 - включить, 0 - выключить)

// ---- \настройки/ ----
#include <FastLED.h>
#include <SoftwareSerial.h>

String strData = "";

CRGB leds[NUM_LEDS];  // создаём ленту

void setup() {
    FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  // инициализация светодиодов

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
    long availableBytes;

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

    if (strData.equals("stop")) {
        LEDS.showColor(CRGB::Black); // убираем полностью свет
        analogWrite(MOTOR_PIN, 0); // отключаем мотор
    } else if (strData.equals("start")) {

//            показываем радугу на первых 7 лампочках

        leds[6].setHue(0); // каждый
        leds[5].setHue(28); // охотник
        leds[4].setHue(43); // желает
        leds[3].setHue(85); // знать
        leds[2].setHue(128); // где
        leds[1].setHue(170); // сидит
        leds[0].setHue(213); // фазан

        FastLED.show(); // записываем на ленточку

//            "да будет движение" - сказал Ньютон и включил моторчик
        analogWrite(MOTOR_PIN, 1023);
    } else {
//            если количество символов не соответствует количеству лампочек
        if (strData.length() != NUM_LEDS)
            return;

        for (int i = 0; i < NUM_LEDS; i++) {

//                f0a => 255, 0, 170 # 3 лампочки, одна гореть будет фиолетовеньким, другая красиньким, третья каким-то...
//                используем почти безопасные цвета, но каждый пиксель кодируем одним символом

            unsigned long number = strtoul(strData[i], NULL, 16);
            long color = number << 4 & number;

            leds[i].setHue(color);       // назначаем цвет

        }


        FastLED.show();                 // записываем в ленточку

    }
}
