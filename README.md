# Pillbox Alarm System

Este projeto consiste em um sistema de alarme para uma caixa de medicamentos (pillbox) que permite configurar e definir alarmes para horários específicos. O sistema utiliza um motor de passo para mover a tampa da caixa e liberar os compartimentos para o usuário acessar os medicamentos.

## Descrição

O projeto é baseado no microcontrolador ESP8266 e utiliza a biblioteca AccelStepper para controlar o motor de passo. O sistema permite 4 modos de funcionamento: Programação, Recarga, Alarme e Espera.

A configuração da rede é gerenciada pela biblioteca WiFiManager, que permite que o usuário configure a rede Wi-Fi através de um ponto de acesso (o nome da rede criada é Pillbox_AP).

O sistema utiliza o protocolo HTTP para se comunicar com um servidor remoto, de onde recupera as configurações de alarme definidas pelo usuário. Para tal, utiliza-se a biblioteca ESP8266HTTPClient.

O sistema também utiliza a biblioteca ArduinoJson para fazer o parse dos dados recebidos do servidor.

### Modo Programação

No modo de programação, o sistema:

- Ativa o módulo Wi-Fi e tenta conectar-se a uma rede configurada previamente;

- **Na primeira vez** que o sistema conecta-se à rede, ele recupera um **código único** do servidor remoto por meio de uma requisição HTTP. Esse código é exibido no display e armazenado na memória flash do MCU, e será utilizado para identificar a caixa de medicamentos no servidor (como um id);

- Toda vez que o modo programação é ativado, o sistema usa o código único (id do porta medicamentos) para obter um token de autenticação através de uma requisição HTTP para o servidor. O token só é retornado caso um usuário já tenha se vinculado à caixa de medicamentos (inserindo o código único no aplicativo mobile);

- Caso não haja um usuário vinculado à caixa de medicamentos, o sistema exibe o código único no display e aguarda o usuário vincular-se à caixa de medicamentos através do aplicativo mobile;

- Após recuperar o token, o sistema tenta recuperar as configurações de alarme do servidor;

- Após recuperar as configurações, o sistema desativa o Wi-Fi para economizar energia, e pergunta se o usuário gostaria de entrar no modo recarga. Caso o usuário não confirme apertando o botão, depois de 5 segundos o sistema entra no modo WAITING, onde fica aguardando o próximo alarme.

- **Obs.:** Caso a caixa já possua um código identificador e já exista um usuário vinculado, todo o processo se resume em recuperar o token e as configurações de alarme, o que acontece bem rápido.

### Modo Recarga (Refill)

Caso o usuário precise recarregar a caixa de remédios, ele pode entrar no modo de recarga. Nesse modo, o sistema irá avançar o motor de passo para permitir que o usuário coloque os medicamentos em cada compartimento. Para passar para o próximo compartimento, o usuário deve apertar o botão. Após passar por todos os compartimentos, o sistema retorna ao modo WAITING.

### Modo Alarme

Quando o horário de um alarme definido chega, o sistema entra no modo de alarme e só para quando o usuário precionar o botão.

Após precionar o botão para parar o alarme, o motor de passo é acionado para abrir o primeiro compartimento programado para aquele horário e aguardará o botão ser precionado novamente para passar para próximo. Esse processo será repetido até que todos os compartimentos programados tenham sido abertos. 

Após passar por todos, o motor de passo gira a tampa até a posição inicial e o sistema retorna ao modo WAITING.

### Modo WAITING (Espera)

Nesse modo, o sistema fica apenas aguardando o próximo alarme e verificando se o botão foi precionado para entrar no modo programação ou recarga. O sistema pode entrar nesse modo após o modo de programação ou após o modo de alarme.
