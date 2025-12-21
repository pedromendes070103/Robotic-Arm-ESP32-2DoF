# Projeto de Controlo de Braço Robótico com ESP32

Este repositório contém vários sketches e scripts para controlar um braço robótico, incluindo movimentação de motores passo a passo e controlo de servos.
O projeto foi desenvolvido no âmbito da cadeira **Técnicas de Prototipagem Rápida** da **Universidade de Aveiro**.

## Estrutura dos Ficheiros

### 1. `servo_teste`
Este sketch permite testar o servo motor da garra (360º) do braço robótico.

**Funcionalidades:**
- Configuração do **pino** ao qual o servo está ligado.
- Definição da **posição de abertura ou fecho** da garra.
- Valores afastados de 90º aumentam a **velocidade** do movimento do servo.
- `90º` corresponde à posição neutra (STOP).

**Exemplo de uso:**
```cpp
#include <ESP32Servo.h>
Servo servo;
void setup() {
  servo.attach(17);  // GPIO17
}
void loop() {

  servo.write(100); // OPEN
  delay(1500);
  
  servo.write(90); // STOP
  delay(3000);

  servo.write(87); // CLOSE
  delay(1100);

  servo.write(90); // STOP
  delay(3000);
}
