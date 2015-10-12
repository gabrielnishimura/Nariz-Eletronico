/***********************************************
 * Nariz Eletronico v1.4
 * Autor: Gabriel Takaoka Nishimura
 * Data: 16/09/2015
 * *********************************************/
#include <TimerOne.h>
#define N_MEDIDORES 4
// defina o numero de medidores e depois escolha os pinos que estarao sendo usados na variavel medidorGas
uint8_t medidorGas[N_MEDIDORES] = {A1, A2, A3, A4}; // numeracao bate com a da plaquina
int rele_gas = 13;
unsigned long t_comeco, t_ciclo, t_trigger;
int configuracao = 0,
    t_exposicao = 0, // em milisegundos
    t_recuperacao = 0, // em milisegundos
    n_ciclos = 0, // uma exposicao e uma recuperacao
    n_leituras = 0, // por ciclo
    ciclo_atual = 1,
    imprimir_via_serial = 0, 
    i; // contador de ciclos

/** Funcao setup do Arduino, configura os pinos dos medidores como entrada, pinos dos 
 * reles como saida e habilita a comunicacao serial. */
void setup() {
    for (i = 0; i < N_MEDIDORES; i++) {
        pinMode(medidorGas[i], INPUT);
    }
    pinMode(rele_gas, OUTPUT);
    // Inicializa comunicacao serial em 9600bauds
    Serial.begin(9600);
    espera_senha_processing();
    Serial.print("Bem vindo ao Nariz Eletronico! Vamos configurar o sistema?\nQual o tempo de exposicao do gas? (em segundos)\n");
}

/** Funcao loop do Arduino. Ela realiza a configuracao dos parametros, 
 * determina o ciclo atual, atua no rele do gas, realiza a leitura dos 
 * medidores do gas e envia essas medidas via serial (caso a flag imprimir_via_serial esteja ativa) 
 * Um exemplo de curva de medidas seria a seguinte
 * |    ___________________|               |
 * |   _                   |___            |
 * |  _                    |   ___         |
 * | _                     |      ___      |
 * |_                      |         ______|
 * |_______________________|_______________|
 * |-------t_exp=5s--------|---t_desc=3s---|
 * |--------------t_ciclo=8s---------------|
 * 1000 / 1 = 1000 > 5000
 * 6000 / 1 = 6000 > 5000
 * 11000 % 8000 = 5000
 * 
 ***/
void loop() {
    if(configuracao < 4) { // para configurar o nariz
        configura_nariz(configuracao);
    } else { // caso nao seja necessario configurar mais
        long t = millis() - t_comeco;
        if (imprimir_via_serial == 1) { // caso seja necessario imprimir a saida
            Serial.print((float)t / (float)1000);
            Serial.print(" ");
            for (i = 0; i < N_MEDIDORES; i++) {
                Serial.print(analogRead(medidorGas[i])); // imprime a medicao do gas
                if (i != N_MEDIDORES - 1) {
                    Serial.print(" ");
                }
            }
            Serial.println();
            imprimir_via_serial = 0;
        }
        if (t % t_ciclo >= t_exposicao) {
            digitalWrite(rele_gas, LOW); // deixando o gas descansar
        } else if (t % t_ciclo < t_exposicao) {
            ciclo_atual = t / t_ciclo + 1;
            if (ciclo_atual >= n_ciclos + 1) {
                Timer1.stop();
                Serial.println("&");
            } else {
                digitalWrite(rele_gas, HIGH); // expondo o gas ao nariz
            }
        }
    }
}

/** Funcao que inicializa a variavel t_comeco com o tempo atual, ativa o rele do gas 
 * e inicializa o timer1 */
void comecaProcesso() {
    t_ciclo = t_exposicao + t_recuperacao; // em milisegundos 
    t_trigger = t_ciclo * 1000 / n_leituras; // em microsegundos
    t_comeco = millis();
    digitalWrite(rele_gas, HIGH); // expondo o gas ao nariz
    Timer1.initialize(t_trigger); // uma leitura a cada t_total / n_leituras, em microsengundos
    Timer1.attachInterrupt(enviar_serial); // a cada vez que se passa o intervalo, chama essa funcao
}

/** Funcao que atribui 1 a flag de impressao. Ela sera usada no loop */
void enviar_serial() {
    imprimir_via_serial = 1;
}

/** Configura os parametros modificaveis do Arduino. Pede, em ordem: 
 * 1) tempo de exposicao
 * 2) tempo de descanso
 * 3) numero de ciclos
 * 4) numero de medicoes */
void configura_nariz(int etapa_de_configuracao) {
    if (configuracao == 0) { // configurar tempo de exposicao
        if (Serial.available()) { // espera por dado via serial
            t_exposicao = Serial.parseInt() * 1000; // transforma dado disponivel via serial em um inteiro
            configuracao++;
            Serial.print("Ok, o gas sera exposto ao nariz por ");
            Serial.print(t_exposicao / 1000);
            Serial.print("s.\n\rQual o tempo de descanso?(em segundos)\n\r");
        }
    } else if (configuracao == 1) { // configurar tempo de descanso
        if (Serial.available()) {
            t_recuperacao = Serial.parseInt() * 1000;
            configuracao++;
            Serial.print("Ok, o nariz descansara por ");
            Serial.print(t_recuperacao / 1000);
            Serial.print("s.\n\rQual o numero de ciclos?\n\r");
        }
    } else if (configuracao == 2) { // configurar numero de ciclos
        if (Serial.available()) {
            n_ciclos = Serial.parseInt();
            configuracao++;
            Serial.print("Ok, serao ");
            Serial.print(n_ciclos);
            Serial.print("ciclos.\n\rQuantas medidas o nariz fara por ciclo?\n\r");
        }
    } else if (configuracao == 3) { // configurar intervalo entre leituras
        if (Serial.available()) {
            n_leituras = Serial.parseInt();
            configuracao++;
            Serial.print("Ok, o nariz medira o gas ");
            Serial.print(n_leituras);
            Serial.print("vezes.\n\r>>>>>");
            delay(500);
            comecaProcesso();
            //Serial.print(""); // codigo para iniciar processo de gravacao no processing
        }
    }
}

/** Funcao que espera receber a string "1234" via serial.
  * Funciona como uma senha para iniciar o processo de configuracao do Arduino*/
void espera_senha_processing() {
    int esperar = 1;
    while(esperar == 1) {
        if (Serial.available() && Serial.parseInt() == 1234) {
            esperar = 0;
        }
    }
}
