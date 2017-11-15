////////////////////////// Bibliotecas Utilizadas //////////////////////////
#include "mbed.h"               // Inclusao da biblioteca padrao.
#include "TextLCD.h"            // Inclusao da biblioteca para controle do display LCD. Biblioteca TextLCD deve ser importada. Autor Simon Ford.

////////////////////////// Declaracao de entradas e saidas //////////////////////////
InterruptIn up(PTD7);           // Configura a porta PTD7 como entrada digital e habilita interrupcao para botao UP.
InterruptIn down(PTD6);         // Configura a porta PTD6 como entrada digital e habilita interrupcao para botao DOWN.
InterruptIn mesure(PTA12);      // Configura a porta PTA12 como entrada digital e habilita interrupcao para monitorar velocidade.
TextLCD lcd(PTC12, PTC13, PTC5, PTC6, PTC10, PTC11);   // PTC12 = RS, PTC13 = E, PTC5 = D4, PTC6 = D5, PTC10 = D6, PTC11 = D7.
DigitalOut led_r(LED1);
DigitalOut led_g(LED2);
DigitalOut led_b(LED3);
Ticker calc_timer;              // Interrupcao para calcular a velocidade de forma periodica.
AnalogOut ang_speed(PTE30);     // Saida analogica para controle de velocidade do motor.

////////////////////////// Declaracao de constantes (Opcoes de Configuracao) //////////////////////////
const float PULSE_ROT = 9.0;    // Quantidade de pulsos gerados no detector em uma rotacao completa do motor.
const float UPDATE_TIME = 1;    // Intervalo de tempo para chamar a rotina de calculo de velocidade e atualizar seu valor (em segundos).
const uint16_t V_MAX = 65535;   // Ajuste do valor maximo para a saida analogica (corresponde a VCC).
const uint16_t V_MIN = 19660;   // Ajuste do valor minimo para a saida analogica (VCC / 3).
const uint16_t V_STEP = 3000;   // Ajuste do incremento na saida analogica.

////////////////////////// Declaracao de variaveis globais //////////////////////////
float count = 0.0;              			  // Variavel para armazenar a quantidade de pulsos ocorridos durante o periodo definido em "UPDATE_TIME".
float speed_read = 0.0;         			  // Variavel para armazenar o valor da velocidade em RPM.
uint16_t speed_out = (uint16_t)((V_MIN + V_MAX) / 2.0);   // Variavel para armazenar o valor da saida analogica.

////////////////////////// Declaracao dos prototipos de funcoes //////////////////////////
void speed_up(void);            // Rotina para incrementar a velocidade.
void speed_down(void);          // Rotina para decrementar a velocidade.
void speed_count(void);         // Rotina para incrementar o valor de "count" a cada pulso recebido em "mesure".
void calc_speed(void);          // Rotina para calcular a velocidade em RPM de acordo com os valores de "count" e "UPDATE_TIME".

////////////////////////// Inicio do programa ////////////////////////// 
 int main() {  

 // Inicializacao do sistema.
    float speed_rec = speed_read;   // Variavel auxiliar para detectar alteracoes no valor da velocidade. 
    up.fall(&speed_up);             // Anexa a rotina "speed_up" a interrupcao gerada pela borda de descida do pulso na entrada "up".
    down.fall(&speed_down);         // Anexa a rotina "speed_down" a interrupcao gerada pela borda de descida do pulso na entrada "down".
    mesure.rise(&speed_count);      // Anexa a rotina "speed_count" a interrupcao gerada pela borda de subida do pulso na entrada "mesure".
    calc_timer.attach(&calc_speed, UPDATE_TIME);    // Anexa a rotina "calc_speed" a interrupcao "calc_timer" e ajusta o periodo.
    ang_speed.write_u16(speed_out);          // Ajusta a velocidade inicial igual a media aritmetica entre "V_MIN" e "V_MAX".

    lcd.cls();                      // Limpa a tela do display LCD.    
    lcd.locate(0,0);                // Posiciona o cursor na primeira coluna e primeira linha.
    lcd.printf("      PSI       "); // Escreve no display.
    wait(1);                        // Aguarda 4s.

    led_r=1;  // apaga todos os LEDs.
    led_b=1;
    led_g=1;
     
    // Loop principal.
    while (1)                       
    {

        if(speed_read != speed_rec) // Permite a atualizacao do display apenas quando ocorre alguma alteracao no valor da velocidade.
        {
            speed_rec = speed_read;
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("   VELOCIDADE   ");
            lcd.locate(0,1);
            lcd.printf("%1.0f RPM", speed_read);

        }
    }
}

/////////////////////// Funcoes //////////////////////////
void speed_up() 
{
     led_g=0;  // Pisca LED verde para indicar aumento de velocidade
     wait_ms(500);
     led_g=1;
     if(speed_out < (V_MAX - V_STEP))    // Aumenta o valor da velocidade apenas ate "V_MAX".
     {
       speed_out = speed_out + V_STEP;
       ang_speed.write_u16(speed_out);
     }
     else 
     {
       led_r=0;  // Pisca LED vermelho e azul para indicar velocidade maxima.
       led_b=0;
       wait(1);
       led_r=1;
       led_b=1;
     }
}
void speed_down() 
{
     led_r=0;  // Pisca LED vermelho para indicar diminuicao de velocidade
     wait_ms(500);
     led_r=1;
     if(speed_out > V_MIN)    // Diminui a velocidade até velocidade mínima
     {
       speed_out = speed_out - V_STEP;
       ang_speed.write_u16(speed_out);
     }
     else 
     {
       led_g=0;  // Pisca LED vermelhor e azul para indicar velocidade maxima.
       led_b=0;
       wait(1);
       led_r=1;
       led_b=1;
     }   
}
void speed_count() 
{
    count = count + 1.0;    // Incrementa contagem dos pulsos.
}
void calc_speed()
{
    speed_read = (count / PULSE_ROT) * ((1.0 / UPDATE_TIME) * 60.0);    // Velocidade em RPM.
    count = 0.0;    // Reinicia a contagem.    
}