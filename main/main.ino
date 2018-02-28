// ----- настройки -----

#define DI_PIN 7                        // пин подключения ленты
#define NUM_LEDS 14                     // количество светодиодов в ленте

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

    // вспышки красным синим и зелёным при запуске (можно отключить)
    if (start_flashes) {
        LEDS.showColor(CRGB(255, 0, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 255, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 255));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 0));

        for (char i = 0; i < NUM_LEDS; i++) {
            leds[i].setHue(0); // показываем красный
            FastLED.show(); // записываем на ленточку
            delay(200);
            LEDS.showColor(CRGB(0, 0, 0)); // вырубаем все диоды
        }
    }

    pinMode(MOTOR_PIN, OUTPUT);

    Serial.begin(9600);

    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.print("Hi!!! (-_-)\n");
    Help();
}

void loop() {

    availableBytes = Serial.available(); // обновляем число байт в буфере serial

    if (availableBytes == 1) {
        delay(10);
        if (Serial.available() == 1) {
            char cmd = Serial.read();

            if (cmd == '^') {
                Serial.print("Warming up... (^_^)\n");
                Start();
                Serial.print("Warmed up! \\*-*/\n");
            } else if (cmd == 'v') {

                Serial.print("Coming down... (.-.)\n");
                LEDS.showColor(CRGB::Black); // убираем полностью свет
                analogWrite(MOTOR_PIN, 0); // отключаем мотор
                leds_on = false;
                Serial.print("Off \\('_')\n");
            } else if (cmd == 'h') {
                Help();
            }
        }
    }

    delay(50);
    availableBytes = Serial.available(); // обновляем число байт в буфере serial

    if (availableBytes == NUM_LEDS) {

        for (char i = 0; i < NUM_LEDS; i++) {
            char sym = Serial.read();

//            magic
            sym = sym > 47 && sym < 58 ? // numeric char
                  sym - 48 :
                  sym > 96 && sym < 103 ? // a-f char
                  sym - 87 :
                  0;


            leds_mem[i] = sym << 4 & sym;
        }

        showing_half = false;

        Serial.print("Command accepted! \\'V'/\n");

    } else if (availableBytes > NUM_LEDS) {

        for (int i = 0; i < availableBytes; i++)
            Serial.read();



//        clearing serial buffer
//        for (long i = 0; i < availableBytes; i++)
//            Serial.read();
    } else if (availableBytes > 1 && availableBytes < NUM_LEDS) {
        Serial.print("Buf size: ");
        Serial.print(availableBytes);
        Serial.print('/');
        Serial.print(NUM_LEDS);
        Serial.print('\n');

    }


    if (leds_on) {

        bool visible_half = millis() % ROUND_TIME > ROUND_TIME / 2;

        if (visible_half && !showing_half) {
            ShowColorsFromMem();
            showing_half = true;
        } else if (!visible_half && showing_half) {
            FastLED.showColor(CRGB(0, 0, 0));
            showing_half = false;
        }


    }

}

void Start() {
// показываем радугу на первых 14 лампочках
    leds[13] = 0; // каждый
    leds[12] = 0; // каждый
    leds[11] = 28; // охотник
    leds[10] = 28; // охотник
    leds[9] = 43; // желает
    leds[8] = 43; // желает
    leds[7] = 85; // знать
    leds[6] = 85; // знать
    leds[5] = 128; // где
    leds[4] = 128; // где
    leds[3] = 170; // сидит
    leds[2] = 170; // сидит
    leds[1] = 213; // фазан
    leds[0] = 213; // фазан

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

void Help() {
    Serial.print("Here are the commands:\n");
    Serial.print(" ^ - turns me up\n");
    Serial.print(" v - turns me down\n");
    Serial.print(" h - shows commands\n");
    Serial.print("any other symbol should be in range [0-9a-f] or it'll be interpreted as 0\n");
    Serial.print("\n");
    Serial.print("Have a nice day\n");
}