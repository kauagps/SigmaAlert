Resumo da Estrutura do Projeto:
1. No lado WEB (cliente):
Um formulário com um campo de texto (input) para digitar a mensagem


Botão de envio → essa mensagem será enviada para o Arduino


2. No lado do Arduino:
Recebe a requisição HTTP contendo a mensagem


Exibe no LCD


Aciona o buzzer (fica apitando)


Fica aguardando até que um botão físico seja pressionado


Ao pressionar o botão:


O buzzer para


(Opcional) LCD limpa ou mostra "Mensagem confirmada"



Hardware necessário no Arduino:
Módulo Ethernet (W5100)


Display LCD (16x2 ou I2C)


Buzzer


Botão físico


Resistor para o botão (pull-down ou usar INPUT_PULLUP)

