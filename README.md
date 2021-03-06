# FSE - Projeto 3 (Final)

O objetivo do projeto final é, através dos conhecimentos adquiridos em aula, desenvolver uma automação para residencias. Para realizar o escopo proposto, utiliza-se uma ESP32 como dispositivo de automaçã, o qual consideramos ser nosso "server".

Para tornar a automação ainda mais real, no projeto, o led da ESP representa uma lâmpada, e o botão representa o sensor de presença. Na interface gráfica do servidor, realiza-se a interação entre os dispositivos cadastrados, além da capacidade de acompanhar umidade e temperatura de certo ambiente, neste cenário, todas as informações são coletadas e medidas na ESP. Há o alarme que, quando acionado e o sensor de presença ativado, dispara um som.

Ademais, o servidor central realiza a comunicação com a ESP utilizando o protocolo MQTT.

![Figura](/imagens/diagrama_arquitetura.png)

### Alunos

Ana Carolina Carvalho da Silva - 19/0063441

Rogério S. dos Santos Júnior - 17/0021751

## Configurações

- **Linguagem:**
  ESP - C  
  Central - JavaScript

- **Instruções de execução:**

1. Clonar o repositório do GitHub
2. Iniciar o servidor central por meio do docker-compose
3. Iniciar a ESP com o código na pasta "esp"
4. Em poucos segundos aparecerá no frontend a solicitação de cadastro da ESP e os menus de interação

## Instruções para iniciar o servidor central

Entre na pasta da ESP32:

```bash
    cd server
```

Rode o servidor do frontend:

```bash
    docker-compose up
```

A aplicação estará disponivel no `localhost:300` do seu dispositivo.

## Instruções para iniciar a esp

Entre na pasta da ESP32:

```bash
    cd server
```

Para a execução dos passos seguintes, é necessário que tenha ativado antes as variáveis de ambiente da **ESP-IDF**. Então dentro da pasta da **ESP-IDF** é necessário que execute o seguinte comando:

```bash
    . $HOME/esp/esp-idf/export.sh
```

Execute a configuração das variáveis de ambiente pelo `menuconfig`:

```bash
    idf.py menuconfig
```

Compile o código do servidor:

```bash
    idf.py build
```

Carregue o executável na placa e rode o comando para acompanhar a execução. Note que o -p é o caminho da ESP no seu computador:

```bash
    idf.py -p /dev/ttyUSB0 flash monitor
```

### Clone do repositório

`git clone https://github.com/rogerioo/Embarcados_Projeto03`

### Vá para o diretório do projeto do servidor central

`cd Embarcados_Projeto03/server`

## Video demonstrativo

O vídeos está disponível na raiz do projeto.
