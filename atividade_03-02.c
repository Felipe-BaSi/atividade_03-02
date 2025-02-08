#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"
#include "pico/stdio_usb.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include <stdlib.h>

// Definição dos pinos I2C e endereço do display OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Definição do número de pixels na matriz de LEDs e pino de saída
#define NUM_PIXELS 25
#define OUT_PIN 7

// Definição da UART e taxa de transmissão
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1 

// Definição dos pinos dos LEDs RGB
const uint led_g = 11;
const uint led_b = 12;
const uint led_r = 13;

// Variável para controlar o estado dos LEDs
ssd1306_t ssd;

bool led_g_on = false, led_b_on = false, led_r_on = false;
int stop[4];

// Definição dos pinos dos botões
const uint but_A = 5;
const uint but_B = 6;

// Variável para controle de tempo no tratamento de interrupções
volatile uint32_t last_time = 0;

// Função para converter valores de cor (0.0 a 1.0) em um valor de 32 bits para a matriz de LEDs
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para atualizar a matriz de LEDs com os valores de cor fornecidos
void update_led_matrix(double *r, double *g, double *b, PIO pio, uint sm) {
    uint32_t valor_led;
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(b[i], r[i], g[i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para exibir números na matriz de LEDs
void display_numbers(char c, double *r, double *g, double *b, PIO pio, uint sm){
    int i;
    
    // Definição das matrizes para cada número de 0 a 9
    double num_0[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0, 
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_1[NUM_PIXELS] = {0.0, 0.0, 0.1, 0.0, 0.0,
                                0.0, 0.0, 0.1, 0.0, 0.0, 
                                0.0, 0.0, 0.1, 0.0, 0.0,
                                0.0, 0.0, 0.1, 0.0, 0.0,
                                0.0, 0.0, 0.1, 0.0, 0.0};

    double num_2[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.0, 0.0, 
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_3[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0, 
                                0.0, 0.1, 0.1, 0.0, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_4[NUM_PIXELS] = {0.0, 0.1, 0.0, 0.0, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0};

    double num_5[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.0, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_6[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.0, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_7[NUM_PIXELS] = {0.0, 0.1, 0.0, 0.0, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_8[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};

    double num_9[NUM_PIXELS] = {0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.0, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0,
                                0.0, 0.1, 0.0, 0.1, 0.0,
                                0.0, 0.1, 0.1, 0.1, 0.0};
                            

    // Escolhe qual número exibir baseado no valor de 'c'
    switch(c){
        case '0':
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_0[i];
                b[i] = 0.0;
            }
        break;

        case '1':
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_1[i];
                b[i] = 0.0;
            }
        break;

        case '2': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_2[i];
                b[i] = 0.0;
            }
        break;

        case '3': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_3[i];
                b[i] = 0.0;
            }
        break;

        case '4': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_4[i];
                b[i] = 0.0;
            }
        break;

        case '5': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_5[i];
                b[i] = 0.0;
            }
        break;

        case '6': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_6[i];
                b[i] = 0.0;
            }
        break;

        case '7': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_7[i];
                b[i] = 0.0;
            }
        break;

        case '8': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_8[i];
                b[i] = 0.0;
            }
        break;

        case '9': 
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = num_9[i];
                b[i] = 0.0;
            }
        break;

        default:
            for(i = 0; i < NUM_PIXELS; i++){
                r[i] = 0.0;
                g[i] = 0.0;
                b[i] = 0.0;
            }
        break;
    }
    update_led_matrix(r, g, b, pio, sm);
}

// Função de tratamento de interrupção dos botões
static void gpio_irq_handler(uint gpio, uint32_t events);

int main() {

    double r[NUM_PIXELS] = {0}, g[NUM_PIXELS] = {0}, b[NUM_PIXELS] = {0};
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    char c;

    // Inicialização dos pinos dos LEDs como saída
    gpio_init(led_r);
    gpio_set_dir(led_r, GPIO_OUT);
    gpio_init(led_g);
    gpio_set_dir(led_g, GPIO_OUT);
    gpio_init(led_b);
    gpio_set_dir(led_b, GPIO_OUT);

    // Inicialização dos pinos dos botões como entrada com pull-up
    gpio_init(but_A);
    gpio_set_dir(but_A, GPIO_IN);
    gpio_pull_up(but_A);
    gpio_init(but_B);
    gpio_set_dir(but_B, GPIO_IN);
    gpio_pull_up(but_B);

    // Inicialização da comunicação serial e UART
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);

    // Inicialização do I2C
    i2c_init(I2C_PORT, 400 * 1000);

    // Configuração dos pinos da UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(but_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(but_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicialização do programa PIO para controlar a matriz de LEDs
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Mensagem inicial na UART
    uart_puts(UART_ID, "Aperte uma técla\r\n");

    // Configuração dos pinos I2C e inicialização do display OLED
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define a função do pino como I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define a função do pino como I2C
    gpio_pull_up(I2C_SDA); // Ativa o pull-up na linha de dados
    gpio_pull_up(I2C_SCL); // Ativa o pull-up na linha de clock
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Loop principal
    while(1){

        // Verifica se há dados disponíveis na UART e se os LEDs estão desligados
        if(uart_is_readable(UART_ID) && led_g_on == false && led_b_on == false){

            c = uart_getc(UART_ID);

            // Exibe o caractere no display OLED
            ssd1306_draw_char(&ssd, c, 20, 30);
            ssd1306_send_data(&ssd);

            // Envia a tecla pressionada de volta pela UART
            uart_puts(UART_ID, "Tecla pressionada: ");
            uart_putc(UART_ID, c);
            uart_puts(UART_ID, "\r\n");

            // Exibe o número correspondente na matriz de LEDs
            display_numbers(c, r, g, b, pio, sm);
            
        }
        
        // Verifica se há dados disponíveis na USB e se os LEDs estão desligados
        if (stdio_usb_connected() && led_g_on == false && led_b_on == false){

            if (scanf("%c", &c) == 1) {

                // Exibe a tecla pressionada no console
                printf("Tecla pressionada: '%c'\n", c);

                // Exibe o caractere no display OLED
                ssd1306_draw_char(&ssd, c, 20, 30);
                ssd1306_send_data(&ssd);
                
                // Exibe o número correspondente na matriz de LEDs
                display_numbers(c, r, g, b, pio, sm);

            }
        }
    }
}

// Função de tratamento de interrupção dos botões
void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Debounce: verifica se o tempo desde a última interrupção é maior que 200ms
    if(current_time - last_time > 200000){

        // Verifica se o botão A foi pressionado
        if(gpio == but_A){
            
            led_g_on = !led_g_on;
            
            gpio_put(led_g, led_g_on);

            if(led_g_on == 1){

                uart_puts(UART_ID, "Led verde ligado\r\n");
                printf("Led Verde ligado\n");

                ssd1306_fill(&ssd, false);
                ssd1306_send_data(&ssd);
    
                ssd1306_draw_string(&ssd, "Verde aceso", 20, 30);
                ssd1306_send_data(&ssd);
                stop[1] = false;

            } else {

                uart_puts(UART_ID, "Led verde desligado\r\n");
                printf("Led Verde desligado\n");

                ssd1306_fill(&ssd, false);
                ssd1306_send_data(&ssd);
                stop[2] = false;
            }

        // Verifica se o botão b foi pressionado
        } else if (gpio == but_B){

            led_b_on = !led_b_on;
            
            gpio_put(led_b, led_b_on);

            if(led_b_on == 1){

                uart_puts(UART_ID, "Led azul ligado\n");
                printf("Led Azul ligado\n");

                
                ssd1306_fill(&ssd, false);
                ssd1306_send_data(&ssd);

                ssd1306_draw_string(&ssd, "Azul aceso", 20, 30);
                ssd1306_send_data(&ssd);

            } else {

                uart_puts(UART_ID, "Led azul desligado\n");
                printf("Led Azul desligado\n");

                ssd1306_fill(&ssd, false);
                ssd1306_send_data(&ssd);

            }

        }

        last_time = current_time;

    }
}
