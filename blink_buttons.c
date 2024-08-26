#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>

#define CHAVE1_BUTTON 5
#define CHAVE2_BUTTON 6
#define LED1 25
#define LED2 8

void setup() {
    // Inicializa o pigpio
    if (gpioInitialise() < 0) {
        printf("Falha ao inicializar o pigpio.\n");
        return;
    }

    // Configuração dos botões como entradas com resistores pull-down
    gpioSetMode(CHAVE1_BUTTON, PI_INPUT);
    gpioSetMode(CHAVE2_BUTTON, PI_INPUT);
    gpioSetPullUpDown(CHAVE1_BUTTON, PI_PUD_DOWN);
    gpioSetPullUpDown(CHAVE2_BUTTON, PI_PUD_DOWN);

    // Configuração dos LEDs como saídas
    gpioSetMode(LED1, PI_OUTPUT);
    gpioSetMode(LED2, PI_OUTPUT);

    // Inicializa os LEDs como desligados
    gpioWrite(LED1, PI_LOW);
    gpioWrite(LED2, PI_LOW);

    time_sleep(1); // Espera de 1 segundo
}

void main_loop() {
    bool on_off = false;

    while (1) {
        time_sleep(0.1);

        // Verifica o botão CHAVE1 pressionado e CHAVE2 não pressionado
        while (gpioRead(CHAVE1_BUTTON) && !gpioRead(CHAVE2_BUTTON)) {
            on_off = !on_off;
            time_sleep(1);
            break;
        }

        // Verifica o botão CHAVE1 não pressionado e CHAVE2 pressionado
        while (!gpioRead(CHAVE1_BUTTON) && gpioRead(CHAVE2_BUTTON)) {
            gpioWrite(LED1, PI_HIGH);
            gpioWrite(LED2, PI_LOW);
            time_sleep(0.125);
            gpioWrite(LED1, PI_LOW);
            gpioWrite(LED2, PI_HIGH);
            time_sleep(0.125);
        }

        // Verifica ambos os botões pressionados
        while (gpioRead(CHAVE1_BUTTON) && gpioRead(CHAVE2_BUTTON)) {
            gpioWrite(LED1, PI_HIGH);
            gpioWrite(LED2, PI_HIGH);
            time_sleep(0.0625);
            gpioWrite(LED1, PI_LOW);
            gpioWrite(LED2, PI_LOW);
            time_sleep(0.0625);
        }

        // Controla os LEDs conforme a variável on_off
        if (on_off) {
            gpioWrite(LED1, PI_HIGH);
            gpioWrite(LED2, PI_HIGH);
        } else {
            gpioWrite(LED1, PI_LOW);
            gpioWrite(LED2, PI_LOW);
        }
    }
}

int main() {
    setup();
    main_loop();

    // Limpeza final
    gpioTerminate();
    return 0;
}

