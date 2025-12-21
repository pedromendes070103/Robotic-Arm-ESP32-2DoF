# Projeto de Controlo de Braço Robótico com ESP32

Este repositório contém vários sketches e scripts para controlar um braço robótico, incluindo movimentação de motores passo a passo e controlo de servos.
O projeto foi desenvolvido no âmbito da unidade curricular **Técnicas de Prototipagem Rápida** da **Universidade de Aveiro**.

## Pré-requisitos

Antes de carregar os sketches no ESP32, é necessário garantir que o ambiente de desenvolvimento está corretamente configurado:

1. **Arduino IDE**: instalar a versão mais recente.
2. **Placa ESP32**:
   - Adicionar o link da board ESP32 no gestor de URLs adicionais da Arduino IDE:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Abrir o Gestor de Placas e instalar **ESP32 by Espressif Systems**.
3. **Bibliotecas necessárias**:
   - `ESP32Servo` (para controlo do servo da garra)
   - `WiFi` (se necessário para futuras funcionalidades)
   - `BluetoothSerial` (se necessário para controlo via Bluetooth)
4. **Configuração dos pinos**: verificar os pinos definidos nos sketches (`stepPin`, `dirPin`, `enablePin`, `servoPin`) e ajustá-los ao teu protótipo.

> Nota: As instruções acima são comuns a todos os sketches deste repositório (`servo_teste.ino`, `stepper.ino`, `stepper_red.ino`, `first_integration.ino`).

## Estrutura dos Ficheiros

### 1. `servo_teste`
Este sketch permite testar o servo motor da garra (360º) do braço robótico.

**Funcionalidades:**
- Configuração do **pino** ao qual o servo está ligado.
- Definição da **posição de abertura ou fecho** da garra.
- Valores afastados de 90º aumentam a **velocidade** do movimento do servo.
- `90º` corresponde à posição neutra (STOP).

**Nota técnica:**  
O controlo de posição não é preciso, pois o motor é do tipo de rotação contínua. A função `servo.write(valor)` altera apenas a **velocidade e sentido de rotação**, e o `delay()` define aproximadamente o tempo de movimento da garra.


### 2. `stepper.ino`
Este sketch permite o controlo de um motor stepper (ex. NEMA23) através de um driver externo, utilizando sinais de **STEP**, **DIRECTION** e **ENABLE**.  
O controlo principal é feito via **comunicação série** com o PC, permitindo ajustar dinamicamente o ângulo de rotação, a velocidade e o sentido de rotação.

**Funcionalidades:**
- Configuração dos pinos de controlo:
  - `stepPin` – geração de pulsos (STEP)
  - `directionPin` – definição do sentido de rotação (DIR)
  - `enablePin` – ativação do driver (ENABLE)
- Definição do ângulo de rotação em graus.
- Ajuste da velocidade de rotação em RPM.
- Alteração do sentido de rotação.
- Controlo em tempo real através do Monitor Série.

**Modos de utilização:**
- **Modo simples (teste básico):**  
  O ficheiro inclui uma versão comentada do código que permite testar o motor com valores fixos de ângulo e velocidade, sem recurso à comunicação série.  
  Este modo é útil para validação inicial do hardware e do driver.
- **Modo avançado (controlo via série):**  
  Permite alterar parâmetros em tempo real através de comandos enviados pelo Monitor Série.

**Comandos disponíveis (via Monitor Série):**
- `Rxxx` → roda o motor `xxx` graus (ex.: `R180`)
- `Vxx` → define a velocidade em RPM (ex.: `V20`)
- `D` → inverte o sentido de rotação
- `RESET` → repõe os valores iniciais

**Notas técnicas:**
- O número de passos é calculado com base no valor de `stepsPerRev` (200 passos por revolução).
- A velocidade é controlada através da frequência dos pulsos enviados ao pino STEP.
- A largura do pulso é definida por `pulseWidthMicros`, respeitando os requisitos do datasheet do driver.
- O movimento é realizado em malha aberta, sem controlo de posição absoluto.

Este ficheiro é destinado a testes e validação do controlo básico de motores stepper no contexto do braço robótico.

### 3. `stepper_red`
Este sketch permite testar um motor stepper **com redutor 1:20**, considerando explicitamente a relação de redução no cálculo do número de passos e da velocidade do motor.

**Funcionalidades:**
- Configuração dos **pinos STEP, DIR e ENABLE**.
- Definição do **ângulo de rotação à saída do redutor**.
- Definição da **velocidade desejada à saída (RPM)**.
- Conversão automática para:
  - Número de passos do motor.
  - Velocidade real do motor antes do redutor.

**Notas importantes:**
- O parâmetro `stepsPerRev` refere-se ao **motor stepper** (200 passos/volta).
- O parâmetro `gearRatio` define a relação do redutor (neste caso, 1:20).
- A velocidade do motor é calculada como:
RPM_motor = RPM_saida × gearRatio
- O controlo é feito por **tempo de ativação dos impulsos STEP**, não por realimentação de posição.

Este código destina-se principalmente à **validação do comportamento cinemático** do conjunto motor + redutor.

### 4. `first_integration.ino`
Este sketch realiza uma **primeira aplicação integrada**, coordenando os motores stepper da base e da articulação intermédia com o servo da garra.  
O objetivo é simular um **primeiro lançamento de um objeto (bola)**, permitindo validar a coerência cinemática e o comportamento do sistema completo.

**Funcionalidades:**
- Motor stepper da **base** com redutor 1:20.
- Motor stepper da **articulação intermédia** (sem redutor).
- Servo da **garra** para abrir e fechar o objeto.
- Movimentos sequenciais predefinidos:
  1. Rotação da base +90°.
  2. Fecho parcial da garra.
  3. Rotação da articulação +96°.
  4. Abertura da garra.
  5. Retorno às posições iniciais.
- Velocidade dos motores definida em RPM à saída, convertida internamente para velocidade real do motor.
- Temporizações ajustadas de acordo com as especificações do driver, garantindo operação fiável.

**Notas importantes:**
- O código utiliza **malha aberta**, sem realimentação de posição.
- O movimento do motor da base considera o **redutor 1:20** no cálculo dos passos.
- O servo da garra utiliza valores afastados de 90º para definir a velocidade de abertura/fecho.

**Objetivo:** Testar a **integração completa** do braço robótico, avaliando a coordenação entre motores e garra antes de implementar sequências mais complexas.


### `integr_2.ino` – Segunda Aplicação Integrada (Comandos Seriais)

Este sketch permite controlar o braço robótico através de **comandos seriais**, tornando o uso mais **intuitivo e user-friendly**.

**Funcionalidades:**
- Rodar o motor da base (M1) para um ângulo específico:
R<angulo>
Ex.: `R120` roda a base 120°.
- Executar um lançamento simulado (movimento coordenado de M2 + garra):
L

**Características importantes:**
- Motor da base com **redutor 1:20**, motor intermédio (M2) sem redutor.
- Garra controlada por **servo**, com posições predefinidas: aberta, fechada parcial e neutra.
- Movimentos executados **sequencialmente** para garantir suavidade e força suficiente.

**Como usar:**
1. Abrir o *Serial Monitor** da Arduino IDE a 115200 bps.
2. Enviar comandos `"R<angulo>"` ou `"L"` para controlar o braço.
3. Ajustar parâmetros no sketch se necessário (velocidade RPM, pinos, posições da garra).

> Nota: Esta aplicação permite ao utilizador executar movimentos complexos apenas com **comandos simples**, sem necessidade de gerir manualmente cada motor.
