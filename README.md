# Projeto de Controlo de Braço Robótico com ESP32

Este repositório contém vários sketches e scripts para controlar um braço robótico, incluindo movimentação de motores passo a passo e controlo de servos.
O projeto foi desenvolvido no âmbito da unidade curricular **Técnicas de Prototipagem Rápida** da **Universidade de Aveiro**.

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
