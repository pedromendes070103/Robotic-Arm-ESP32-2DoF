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
   - `BLEDevice`,`BLEUtils` e `BLEServer`  (se necessário para controlo via Bluetooth)
   - 
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


### 5. `integr_2.ino`

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

### 6. `ble_LightBlue.ino`

Este sketch implementa uma adaptação da segunda aplicação integrada para permitir o **controlo do braço robótico via Bluetooth Low Energy (BLE)**. É compatível com aplicações como o LightBlue no smartphone.

**Funcionalidades:**
- Receção de comandos simples pelo BLE:
  - `"R<angulo>"` → roda o motor da base (M1) para o ângulo especificado.
  - `"L"` → executa a sequência de lançamento (movimento do motor intermédio M2 + garra).
  - `"RESET"` → retorna a garra à posição neutra.
- Coordenação sequencial de motores e garra para garantir movimentos suaves e controlados.
- Utiliza as bibliotecas `BLEDevice.h`, `BLEUtils.h` e `BLEServer.h` para criar o servidor BLE e gerir características e callbacks.

**Observações:**
- Antes de carregar o sketch, certifique-se de ter instalado as bibliotecas necessárias no Arduino IDE.
- Este sketch permite ao utilizador controlar o braço remotamente, mantendo a lógica já testada na aplicação integrada anterior (`integr_2.ino`).

### 7. `wifi.ino`

Esta aplicação permite controlar o braço robótico através de uma **página web** acessível a partir de qualquer dispositivo ligado à rede WiFi criada pelo ESP32.  

**Funcionalidades:**
- Criação de um ponto de acesso WiFi próprio (`ESP32_ROBOT`), permitindo ligação de smartphones, tablets ou PCs.
- Controlo do **motor da base (M1)** para um ângulo específico, introduzido pelo utilizador.
- Execução de um **lançamento simulado**, envolvendo o motor intermédio (M2) e a garra.
- **Reset da garra** para a posição de parada.
- Interface web intuitiva, com campos de input e botões para envio de comandos.

**Exemplo de uso:**
1. Ligar à rede WiFi `ESP32_ROBOT` com a password `12345678`.
2. Abrir o navegador e aceder ao endereço `192.168.4.1`.
3. Inserir o ângulo desejado para M1 e clicar em **Rotate M1**, ou clicar em **Launch** para executar o lançamento.
4. Clicar em **Reset** para reposicionar a garra.

**Observações técnicas:**
- Antes de carregar o sketch, certifique-se de ter instalado as bibliotecas necessárias no Arduino IDE.
- O ESP32 atua como servidor web e processa os comandos sequencialmente, garantindo movimentos controlados e repetíveis.
- O tempo de execução e a velocidade dos motores são calculados automaticamente pelo código com base na rotação desejada e na relação de redução (gear ratio).

### 8. `onlyreeds.ino`

Este sketch permite que se garanta o correto funcionamento do sistema de homing, testando os reed switches para cada motor e para a garra.

**Sensores utilizados (KMS-30):**
- Reed switch para a garra (fim de abertura)
- Reed switch para o motor intermédio (M2)
- Reed switch para o motor da base (M1)

**Descrição do teste:**
- Cada reed switch está ligado com uma extremidade a 3.3V e a outra ao GPIO do ESP32 configurado como `INPUT_PULLUP` a ao GND.
- Ao aproximar um íman do sensor, o estado do pino muda, permitindo detetar a posição “home” ou “fim de curso”.
- Durante os testes, a aplicação imprime no Serial Monitor quando cada sensor é DETETADO ou LIVRE.
- Pequeno delay de 20 ms incluído para debounce e evitar spam de mensagens.

**Observação:** Alguns sensores podem apresentar lógica inversa, dependendo das características do reed switch (NO/NC). Ajustes no código podem ser necessários para interpretar corretamente os estados.


