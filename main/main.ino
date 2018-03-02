// ----- настройки -----

#define DI_PIN 7                        // пин подключения ленты
#define NUM_LEDS 13                     // количество светодиодов в ленте

#define ROUND_TIME 300                  // время одного полного оборота оси на полной скорости

#define MOTOR_PIN 3                     // пин подключения мотора

#define start_flashes 1                 // проверка цветов при запуске (1 - включить, 0 - выключить)

// ---- \настройки/ ----
#include <FastLED.h>
#include <SoftwareSerial.h>


char leds_mem[NUM_LEDS];
long availableBytes;

bool leds_on = false;
bool showing_half = false;

String strData = "";

CRGB leds[NUM_LEDS];  // создаём ленту

void setup() {
    FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  // инициализация светодиодов
    FastLED.showColor(CRGB::Black); // если нажат reset, то лента

    // вспышки красным синим и зелёным при запуске (можно отключить)
    if (start_flashes) {
        LEDS.showColor(CRGB::Red);
        delay(500);
        LEDS.showColor(CRGB::Green);
        delay(500);
        LEDS.showColor(CRGB::Blue);
        delay(500);
        LEDS.showColor(CRGB::Black);


        // Тест цвета закончен
        delay(100);
        LEDS.showColor(CRGB::Red);
        delay(100);
        LEDS.showColor(CRGB::Black);
        delay(100);
        LEDS.showColor(CRGB::Red);
        delay(100);
        LEDS.showColor(CRGB::Black);


        delay(200);


        for (char i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Red; // показываем красный
            leds[NUM_LEDS - 1 - i] = CRGB::Green; // показываем зелёный
            FastLED.show(); // записываем на ленточку
            delay(200);
            leds[i] = CRGB::Black; // показываем чёрный
            leds[NUM_LEDS - 1 - i] = CRGB::Black;
            FastLED.show(); // записываем на ленточку
            delay(100);
        }

    }

    pinMode(MOTOR_PIN, OUTPUT);

    LEDS.showColor(CRGB::Green);
    delay(100);


    Serial.begin(9600);
    while (!Serial) { ; } // wait for serial port to connect. Needed for native USB port only


    LEDS.showColor(CRGB::Black);
    delay(50);
    LEDS.showColor(CRGB::Green);
    delay(100);
    LEDS.showColor(CRGB::Black);
    delay(50);
    LEDS.showColor(CRGB::Green);
    delay(100);
    LEDS.showColor(CRGB::Black);

}

void loop() {

    availableBytes = Serial.available(); // обновляем число байт в буфере serial

    if (availableBytes == 1) {
        delay(10);
        if (Serial.available() == 1) {
            char cmd = Serial.read();

            if (cmd == '^') {
                Start();
            } else if (cmd == 'v') {
                LEDS.showColor(CRGB::Black); // убираем полностью свет
                analogWrite(MOTOR_PIN, 0); // отключаем мотор
                leds_on = false;
            }
        }
    }

    delay(50);
    availableBytes = Serial.available(); // обновляем число байт в буфере serial

    if (availableBytes == NUM_LEDS) {

        for (char i = 0; i < NUM_LEDS; i++) {
            char sym = Serial.read();

            /* /// СВОЯ РЕАЛИЗАЦИЯ strtol \\\ */
            sym = sym > 47 && sym < 58 ? // цифра
                  sym - 48 :
                  sym > 96 && sym < 103 ? // a-f
                  sym - 87 :
                  0;
            /* \\\ СВОЯ РЕАЛИЗАЦИЯ strtol /// */

            leds_mem[i] = sym << 4 | sym;

        }

        showing_half = false;

    } else if (availableBytes > NUM_LEDS) {

        for (int i = 0; i < availableBytes; i++)
            Serial.read();

    }


    // если вообще работаем
    if (leds_on) {

        // мы находимся на "светлой стороне"?
        bool visible_half = millis() % ROUND_TIME > ROUND_TIME / 2;

        // если на светлой и предыдущая была показана тёмная
        if (visible_half && !showing_half) {
            ShowColorsFromMem();
            showing_half = true;
        } else if (!visible_half && showing_half) {   // если текущая сторона тёмная и предыдущая светлая была
            FastLED.showColor(CRGB(0, 0, 0));
            showing_half = false;
        }

    }

}

void Start() {

    for (char i = 0; i < NUM_LEDS; i++) {
        // 224 = 14 * 16  -  фиолетовый в 16ричной системе переводим в 256
        leds_mem[NUM_LEDS - 1 - i] = 224 * i / NUM_LEDS;
    }

    // говорим, что предудыщей была "тёмная" сторона, чтобы изменить цвет, если ось находится на "светлой" стороне
    showing_half = false;
    leds_on = true;

    // "да будет движение" - сказал Ньютон и включил моторчик
    analogWrite(MOTOR_PIN, 255);
}

void ShowColorsFromMem() {

    for (char i = 0; i < NUM_LEDS; i++)
        leds[i].setHue(leds_mem[i]);       // назначаем цвет

    FastLED.show();                 // записываем в ленточку

}
