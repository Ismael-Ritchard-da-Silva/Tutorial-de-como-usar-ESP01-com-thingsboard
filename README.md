# Tutorial-de-como-usar-ESP01-com-thingsboard
Repositório desenvolvido por Maria Clara Nadim Martiniano e Ismael Ritchard da Silva, apresentando um guia completo de integração do módulo ESP-01 (ESP8266) com a plataforma ThingsBoard Cloud. O projeto inclui configuração do dispositivo, implementação do código-fonte, esquemas de ligação e envio de telemetria via protocolo MQTT.


## Materiais necessários:
- ESP-01 (ESP8266)
- Microcontrolador (usada no tutorial foi STM32 Nucleo-F401RE)
- Protoboard
- Adaptador USB
- Arduino IDE
- Conta no ThingsBoard
- Potenciometro B100k



  
## Parte 3 - Configuração no ThingsBoard
### Acessar a aba Devices
No menu lateral, clique em **Devices** para visualizar os dispositivos cadastrados.

![Passo 1](imagens/passo1.png)

---
### Adicionar novo dispositivo

![Passo 2](imagens/passo2.png)
Clique no botão **+ Add device** no canto superior direito.
Na janela que abrir:
- Preencha o campo **Name** (ex: ESP01)
- Mantenha o **Device profile** como `default`
- Clique em **Add**
![Passo 3](imagens/passo3.png)


---

### Abrir detalhes do dispositivo

Após criar o dispositivo, clique sobre o nome dele (ESP01).
Na tela de detalhes, clique em: **Copy access token**
Esse **token** será utilizado no código do ESP-01 para autenticação na plataforma.


---
### Confirmar dispositivo ativo
Após a criação, o dispositivo aparecerá na lista com o status **Active**.
Isso indica que o dispositivo está cadastrado corretamente e pronto para receber dados.

![Passo 4](imagens/passo4.png)

---
## Importante

O **Access Token** copiado no Passo 3 deve ser inserido no código do ESP-01:
