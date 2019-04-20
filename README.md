# Trabalho 1 - Emulador MIC-1 e Assembler MAC-1

- [Trabalho 1 - Emulador MIC-1 e Assembler MAC-1](#trabalho-1---emulador-mic-1-e-assembler-mac-1)
  - [Descrição Geral](#descri%C3%A7%C3%A3o-geral)
  - [PARTE 1 - Emulador MIC-1](#parte-1---emulador-mic-1)
  - [PARTE 2 - Assembler MAC-1](#parte-2---assembler-mac-1)
  - [PARTE 3 - Programando para sua máquina](#parte-3---programando-para-sua-m%C3%A1quina)
  - [Exemplos (by prof.: Yuri)](#exemplos-by-prof-yuri)
  - [More info](#more-info)

## Descrição Geral

Você deverá implementar um emulador da microarquitetura MIC-1 e um montador para a linguagem assembly MAC-1, executável em MIC-1 através de tradução realizada pelo microprograma "[microprog.rom](examples/microprog.rom)" disponibilizado.

## PARTE 1 - Emulador MIC-1

A microarquitetura MIC-1 é uma arquitetura de microprocessador baseado em microprograma de controle proposta por Tanembaum em seu livro Organização Estruturada de Computadores (a partir da quarta edição. O autor também propõe em seu livro um microprograma que torna MIC-1 capaz de executar um conjunto de instruções de uma máquina semelhante à Máquina Virtual Java (JVM), à qual ele dá o nome de MAC-1.

Na primeira parte do seu trabalho, você deve implementar o emulador de MIC-1 seguindo sua especificação detalhada no capítulo 4 do livro (seção 4.1). Para testar seu emulador, considere os seguintes arquivos:

- [microprog.rom](examples/microprog.rom): microprograma de controle binário que executa em MIC-1 qualquer arquivo binário executável gerado a partir da montagem de programa escrito em linguagem assembly de MAC-1. Tal microprograma está gravado como um array de 512 posições de 64 bits (unsigned long int). Portanto, essa é a especificação do seu armazenamento de controle
- [prog.exe](examples/prog.exe): programa binário para executar em MAC-1. Como o microprograma de MIC-1 simula MAC-1, MIC-1 é capaz de executar tal programa quando carregado na memória principal.
- [prog.asm](examples/prog.asm): código assembly do programa [prog.exe](examples/prog.exe).

Se o seu emulador estiver funcionando adequadamente, ao carregar "[microprog.rom](examples/microprog.rom)" no armazenamento de controle (simulado como um array de unsigned long int) e "[prog.exe](examples/prog.exe)" na memória principal (simulada como um array de bytes), o programa deve ser executado corretamente. Acompanhe o funcionamento estudando a lógica de "[prog.asm](examples/prog.asm)".

## PARTE 2 - Assembler MAC-1

A linguagem assembly de MAC-1 é definida no capítulo 4, seção 4.2. Você deverá implementar um montador que gera o código binário (bytecode) de um programa escrito com tal conjunto de instruções, seguindo cuidadosamente sua especificação. Assim, você poderá escrever um programa em assembly MAC-1, montar e executar em seu emulador MIC-1.

As instruções MAC-1 (especificadas na tabela 4.11, pág.205 da sexta edição) fazem referências a endereços do armazenamento de controle que contêm o primeiro microcódigo do microprograma que implementa a respectiva instrução MAC-1. Estes são os bytes (ou seja, os endereços) de cada instrução disponível (bytes diferentes dos expostos pelo livro por conveniência de implementação):

| *Instrução em Assembly MAC-1* | *byte* |
| --- | --- |
| nop | 0x01 |
| iadd | 0x02 |
| isub | 0x05 |
| iand | 0x08 |
| ior | 0x0B |
| dup | 0x0E |
| pop | 0x10 |
| swap| 0x13 |
| bipush | 0x19 |
| iload | 0x1C |
| istore | 0x22 |
| wide | 0x28 |
| ldc_w | 0x32 |
| iinc | 0x36 |
| goto | 0x3C |
| iflt | 0x43 |
| ifeq | 0x47 |
| if_icmpeq | 0x4B |
| invokevirtual | 0x55 |
| ireturn | 0x6B |

\* Instruções em vermelho ainda não foram testadas. Logo, erros podem existir nos respectivos trechos que as implementam no microprograma de controle. Não aconselho testar seu montador com programas assembly que utilizem tais instruções. Você mesmo pode fazer o teste dessas outras instruções quando tiver certeza que seu montador está funcionando corretamente.

Existe um trecho do microprograma de controle responsável por fazer a inicialização de MIC-1, colocando os valores adequados nos registradores que controlam a pilha teoricamente controlada por MAC-1, isto é CPP, LV, PC e SP. O primeiro microcódigo do trecho do microprograma que realiza tal processo está no byte 0x73 do armazenamento de controle. Assim, todo arquivo executável de MAC-1 deve conter, antes do programa propriamente dito, um trecho de programa padrão para chamar essa inicialização, sendo o byte 1 contendo o valor 0x73 (endereço do trecho de microprograma que faz a inicialização) e as palavras 1, 2, 3 e 4 (bytes 4 a 7, 8 a 11, 12 a 15 e 16 a 19, respectivamente) contendo os valores iniciais de CPP, PC, LV e SP. Os bytes do programa propriamente dito começariam, portanto, no byte 20. Em C, a escrita desse trecho de inicialização na memória ficaria:

```shell

memory[1] = 0x73; //init

memory[4] = 0x0006; //(CPP inicia com o valor 0x0006 guardado na palavra 1 - bytes 4 a 7.)

word tmp = 0x1001; //LV

memcpy(&(memory[8]), &tmp, 4); //(LV inicia com o valor de tmp guardado na palavra 2 - bytes 8 a 11)

tmp = 0x0400; //PC

memcpy(&(memory[12]), &tmp, 4); //(PC inicia com o valor de tmp guardado na palavra 3 - bytes 12 a 15)

tmp = 0x1001 + num_of_vars; //SP

//SP (Stack Pointer) é o ponteiro para o topo da pilha.

//A base da pilha é LV e ela já começa com algumas variáveis empilhadas (dependendo do programa).

//Cada variável gasta uma palavra de memória. Por isso a soma de LV com num_of_vars.

memcpy(&(memory[16]), &tmp, 4); //(SP inicia com o valor de tmp guardado na palavra 4 - bytes 16 a 19)

```

Perceba que, embora o programa a ser gravado pelo montador no arquivo comece no byte 20, no momento de abrir e ser lido para a memória do emulador para ser executado, ele deve ser colocado a partir do byte `0x0401`, ou seja, a partir de PC+1` (o microprograma sempre começa incrementando PC antes de puxar o endereço de microprograma da memória principal; pelo mesmo motivo a instrução init fica no byte 1 e não no byte 0 da memória principal). O trecho de inicialização fica nas posições originais normalmente.

Além disso, antes da memória propriamente dita, o arquivo executável gerado pelo montador possui um valor Q de 4 bytes (um unsigned int) indicando o tamanho completo do programa a ser carregado (inicialização + programa propriamente dito). Assim, se o programa propriamente dito tem P bytes de tamanho, Q = (20 + P). Isso permite uma cópia em lote de toda uma sequência de bytes, evitando, na hora de abrir, de ter que copiar o arquivo em um laço byte a byte até eof.

Resumindo o formato do arquivo executável:

| | | |
| --- | --- | --- |
| Q (4 bytes) | Inicialização | Programa (P bytes) |

Os 20 bytes de inicialização devem ficar nas 20 primeiras posições de memória do emulador. Os P bytes do programa devem ser jogados para as P posições de memória a partir da posição indicada pelo valor de PC determinado nos bytes 12 a 15 da inicialização (embora estejam em sequência no arquivo).

Como não existe instrução de declaração de variáveis, uma primeira lida no código assembly deve ser feita para determinar quantas variáveis diferentes existem. Seus endereços serão determinados em sequência a partir de LV e o microprograma já faz esse controle relativo. Assim, se as variáveis que aparecem são i, j e k, nessa ordem, seus endereços quando montadas serão 0x00, 0x01 e 0x02, respectivamente. O microprograma faz o trabalho de somar esses valores a LV quando alguma instrução de referência a variável é chamado (istore ou iload).

\* Caso tenha curiosidade em ver como é feita a montagem do microprograma de controle, aqui está o código fonte do programa que gera o arquivo "[microprog.rom](examples/microprog.rom)": microassembler.cpp .

## PARTE 3 - Programando para sua máquina

Crie outros pequenos programas em assembly MAC-1, monte com o seu assembler e execute com o seu emulador de MIC-1.

## Exemplos (by prof.: Yuri)

- [Exemplos](examples/README.md)

## More info

- Teacher page: [lia.ufc.br/~yuri](http://www.lia.ufc.br/~yuri/)
- [lia.ufc.br/~yuri/20181/arquitetura/trab1.html](http://www.lia.ufc.br/~yuri/20181/arquitetura/trab1.html)