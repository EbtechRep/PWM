#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/pwm.h"

#define PINO_SERVO 22   // Pino GPIO conectado ao servomotor
#define PINO_LED 12      // Pino GPIO conectado ao LED
#define PWM_CICLO 24999  // 20ms (50Hz) para servos
#define DIVISOR_CLK 100  // Divisor de clock para obter o período correto

// Função para inicializar o PWM do servo e do LED
void inicializa() {
    // Configura o PWM do servo
    gpio_set_function(PINO_SERVO, GPIO_FUNC_PWM);
    uint slice_num_servo = pwm_gpio_to_slice_num(PINO_SERVO);
    pwm_config config_servo = pwm_get_default_config();
    pwm_config_set_clkdiv(&config_servo, DIVISOR_CLK);
    pwm_config_set_wrap(&config_servo, PWM_CICLO);
    pwm_init(slice_num_servo, &config_servo, true);

    // Configura o PWM do LED
    gpio_set_function(PINO_LED, GPIO_FUNC_PWM);
    uint slice_num_led = pwm_gpio_to_slice_num(PINO_LED);
    pwm_config config_led = pwm_get_default_config();
    pwm_config_set_clkdiv(&config_led, DIVISOR_CLK);
    pwm_config_set_wrap(&config_led, PWM_CICLO);
    pwm_init(slice_num_led, &config_led, true);
}

// Função para converter microssegundos em nível PWM
uint32_t microssegundos_para_nivel(uint32_t us) {
    return (uint32_t)(us * 1.25f); // 1µs ≈ 1.25 unidades
}

// Função para mapear o ângulo do servo para o brilho do LED (0 a 255)
uint8_t mapeia_brilho_led(int angulo) {
    return (uint8_t)(angulo * 255 / 180); // Mapeia 0°-180° para 0-255
}

//Função Principal
int main() {
    uint slice_num_servo = pwm_gpio_to_slice_num(PINO_SERVO);

    // Inicializa o PWM do servo e do LED
    inicializa();

    // Move o servo para 2400µs (2.4ms) - 180 graus
    pwm_set_gpio_level(PINO_SERVO, microssegundos_para_nivel(2400));
    pwm_set_gpio_level(PINO_LED, 255 * PWM_CICLO / 255); // LED no brilho máximo
    sleep_ms(5000);

    // Move o servo para 1470µs (1.47ms) - 90 graus
    pwm_set_gpio_level(PINO_SERVO, microssegundos_para_nivel(1470));
    pwm_set_gpio_level(PINO_LED, 127 * PWM_CICLO / 255); // LED na metade do brilho
    sleep_ms(5000);

    // Move o servo para 500µs (0.5ms) - 0 graus
    pwm_set_gpio_level(PINO_SERVO, microssegundos_para_nivel(500));
    pwm_set_gpio_level(PINO_LED, 0); // LED apagado
    sleep_ms(5000);

    // Transição suave de 0° a 180° com incremento de ±5µs e atraso de 10ms
    uint32_t nivel_inicial = microssegundos_para_nivel(500);  // 625
    uint32_t nivel_final = microssegundos_para_nivel(2400);   // 3000

    while (true) {
        // Movimento de 0° a 180°
        for (uint32_t i = nivel_inicial; i <= nivel_final; i += 5) {
            pwm_set_gpio_level(PINO_SERVO, i);
            uint8_t brilho = mapeia_brilho_led((i - nivel_inicial) * 180 / (nivel_final - nivel_inicial));
            pwm_set_gpio_level(PINO_LED, brilho * PWM_CICLO / 255);
            sleep_ms(10);  // Atraso de 10ms para suavidade
        }

        // Movimento inverso
        for (uint32_t i = nivel_final; i >= nivel_inicial; i -= 5) {
            pwm_set_gpio_level(PINO_SERVO, i);
            uint8_t brilho = mapeia_brilho_led((i - nivel_inicial) * 180 / (nivel_final - nivel_inicial));
            pwm_set_gpio_level(PINO_LED, brilho * PWM_CICLO / 255);
            sleep_ms(10);  
        }
    }

    return 0;
}