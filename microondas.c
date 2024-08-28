#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>

// saida genericas
#define MAGNETRON 5
#define PRATOGIRANDO 6
#define FUNCIONANDO 13
#define MANUAL 19 // pode ser substituido por ligacao direta no vcc
// saidas temperatura
#define TLOW 17
#define TMEDIUM 27
#define THIGH 22
// entradas
#define PORTA 9 // a ideia eh fazer com ldr ver como funciona
#define TEMPERATURA 2
#define INTERRUPTOR 3
#define MANUALIDADE 4


void setup() {
    // Inicializa o pigpio
    if (gpioInitialise() < 0) {
        printf("Falha ao inicializar o pigpio.\n");
        return;
    }

    // Configuração dos botões como entradas com resistores pull-down
    gpioSetMode(INTERRUPTOR, PI_INPUT);
    gpioSetMode(MANUALIDADE, PI_INPUT);
    gpioSetMode(TEMPERATURA, PI_INPUT);
    gpioSetMode(PORTA, PI_INPUT);
    gpioSetPullUpDown(INTERRUPTOR, PI_PUD_DOWN);
    gpioSetPullUpDown(MANUALIDADE, PI_PUD_DOWN);
    gpioSetPullUpDown(TEMPERATURA, PI_PUD_DOWN);
    gpioSetPullUpDown(PORTA, PI_PUD_DOWN);

    // Configuração dos LEDs como saídas
    gpioSetMode(MANUAL, PI_OUTPUT);
    gpioSetMode(MAGNETRON, PI_OUTPUT);
    gpioSetMode(PRATOGIRANDO, PI_OUTPUT);
    gpioSetMode(FUNCIONANDO, PI_OUTPUT);
    gpioSetMode(PORTA, PI_OUTPUT);

    gpioSetMode(TLOW, PI_OUTPUT);
    gpioSetMode(TMEDIUM, PI_OUTPUT);
    gpioSetMode(THIGH, PI_OUTPUT);

    // Inicializa os LEDs como desligados
    gpioWrite(MANUAL, PI_LOW);
    gpioWrite(MAGNETRON, PI_LOW);
    gpioWrite(PRATOGIRANDO, PI_LOW);
    gpioWrite(FUNCIONANDO, PI_LOW);
    gpioWrite(PORTA, PI_LOW);

    gpioWrite(TLOW, PI_LOW);
    gpioWrite(TMEDIUM, PI_LOW);
    gpioWrite(THIGH, PI_LOW);

    time_sleep(1); // Espera de 1 segundo
}

void funcionando(){
    gpioWrite(FUNCIONANDO, PI_LOW);
    time_sleep(0.5);
    gpioWrite(FUNCIONANDO, PI_HIGH);
    time_sleep(0.5);
}

void ligar(){
    gpioWrite(MAGNETRON, PI_HIGH);
    gpioWrite(PRATOGIRANDO, PI_HIGH);
    gpioWrite(FUNCIONANDO, PI_HIGH);
}
    
void desligar(){
    gpioWrite(MAGNETRON, PI_LOW);
    gpioWrite(PRATOGIRANDO, PI_LOW);
    gpioWrite(FUNCIONANDO, PI_LOW);
}

void main_loop() {
    bool manual_on_off = false;
    int heat = 1;
    int time = 0;
    while (1) {
        time_sleep(0.1);

        // Leitura do botao de temperatura.
        while (gpioRead(TEMPERATURA))
        {
            heat++;
            if(heat == 4) heat = 1;
            while (gpioRead(TEMPERATURA)) time_sleep(0.001);
        }

        // Leitura do botao de manualidade
        while (gpioRead(MANUALIDADE)) // leitura do botao para manual para alterar
        {
            manual_on_off = !manual_on_off;
            while (gpioRead(MANUALIDADE)) time_sleep(0.001);
        }

        // Definicao do led Manual pelo valor de 'manual_on_off'
        if(manual_on_off){
            gpioWrite(MANUAL, PI_HIGH);
        }else{
            gpioWrite(MANUAL, PI_LOW);
        }

        // exibicao da temperatura pelo valor de 'heat'.
        switch (heat)
        {
        case 1:
            gpioWrite(TLOW, PI_HIGH);
            gpioWrite(TMEDIUM, PI_LOW);
            gpioWrite(THIGH, PI_LOW);
            break;
        case 2:
            gpioWrite(TLOW, PI_HIGH);
            gpioWrite(TMEDIUM, PI_HIGH);
            gpioWrite(THIGH, PI_LOW);
            break;
        case 3:
            gpioWrite(TLOW, PI_HIGH);
            gpioWrite(TMEDIUM, PI_HIGH);
            gpioWrite(THIGH, PI_HIGH);
            break;
        default:
            break;
        }

        // micro ondas funcionando no modo automatico 
        while (gpioRead(INTERRUPTOR) && !manual_on_off) {
            time_sleep(1);
            ligar();
            switch (heat){ // selecionando tempo de acordo com a temperatura.
                case 1: time=12; break;
                case 2: time=18; break;
                case 3: time=24; break;
                default: break;
            }
            while ( gpioRead(PORTA) && (time>0) && !gpioRead(INTERRUPTOR) )
            {
                funcionando();
                time--;
            }
            desligar();
            heat=1;
        }


        // micro ondas funcionando no modo manual 
        while (gpioRead(INTERRUPTOR) && manual_on_off) { 
            time_sleep(1);
            ligar();
            while( gpioRead(PORTA) && !gpioRead(INTERRUPTOR) ){
                funcionando(1);
            }
            desligar();
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

