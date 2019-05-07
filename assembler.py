# -*- coding: utf-8 -*-
import sys

#dicionario das instruções com seus respectivos codigos hexadecimais e operando(s)
dic_instrucoes = {
    'nop':           [0x01, []], 'iadd':          [0x02, []], 'isub':          [0x05, []], 'iand':          [0x08, []],
    'ior':           [0x0b, []], 'dup':           [0x0e, []], 'pop':           [0x10, []], 'swap':          [0x13, []],
   
    'bipush':        [0x19, ['byte']],            'iload':         [0x1c, ['varnum']],     'istore':        [0x22, ['new_varnum']],
    'wide':          [0x28, []],                  'ldc_w':         [0x32, ['index']],      'iinc':          [0x36, ['varnum', 'const']],
    'goto':          [0x3c, ['offset']],          'iflt':          [0x43, ['offset']],     'ifeq':          [0x47, ['offset']],
    'if_icmpeq':     [0x4b, ['offset']],          'invokevirtual': [0x55, ['disp']],       'ireturn':       [0x6b, []]
}

dic_labels = {}    # dicionario para armazenar as labels e os respectivos endereços
dic_variaveis = {} # dicionario para armazenar as variáveis e os respectivos endereços
lista_bytes = []   # lista que armazena os endereços das labels, variaveis, instruções e operandos
tela_erro = ""     


contador_linha = 0 
contador_byte = 0  

proxima_variavel = 0 

# funções
def add_erro(tipo_erro, linha_erro):        # adiciona mensagem de erro e mostra a linha que contem o erro  
    global contador_linha, tela_erro

    tela_erro += ("Há um erro na linha " + str(linha_erro) + ": " + tipo_erro + "\n")
    return tela_erro

def checar_comentario(string):              # checa se a linha é um comentário
    try:
        return string.startswith("//")
    except TypeError:
        raise TypeError

def checar_label(string):                   # checa se um elemento é uma label
    global contador_linha, dic_variaveis, dic_instrucoes

    if string in dic_variaveis.keys():
        add_erro("Label com mesmo nome de uma variável", contador_linha)
        return False
    else:
        return string not in dic_instrucoes.keys() and string.replace("_", "").isalnum()

def add_label(label):                        # sendo uma label, é adicionada ao dicionário de labels
    global contador_byte, dic_labels

    dic_labels[label] = contador_byte + 1

def checar_variavel_valida(string):          # checa se um elemento é uma variavel válida
    global dic_labels, contador_linha

    if string in dic_labels.keys():
        add_erro("Variável com mesmo nome de uma label", contador_linha)
        return False
    else:
        return string[0].isalpha() and string.replace("_","").isalnum()

def checar_instrucao_valida(instrucao):       # checa se a instrução está no dicionario de instruções, definido no inicio
    global dic_instrucoes

    return instrucao in dic_instrucoes.keys()

def add_instrucao(instrucao):                  # endereço de memoria da instrução é adicionada à lista de bytes
    global contador_byte,  dic_instrucoes

    lista_bytes.append(dic_instrucoes[instrucao][0])
    contador_byte += 1

def checar_operando_valido(instrucao, operando, tipo_operando, numero_operandos):             # checa se o operando da instrução é válido
    global dic_variaveis
    
    if numero_operandos == len(operando):
        if numero_operandos == 0:
            return True
        
        flag = True
        
        for i in range (0, numero_operandos, 1):
            if tipo_operando[i] == "varnum":
                if operando[i] not in dic_variaveis:
                    flag = False
            
            elif tipo_operando[i] == "new_varnum":
                if not checar_variavel_valida(operando[i]):
                    flag = False
            
            elif tipo_operando[i] ==  "byte" or tipo_operando[i] == "const" or tipo_operando[i] == "index" or tipo_operando[i] == "disp":
                if not operando[i].isnumeric():
                    flag = False

            else:
                if not checar_label(operando[i]):
                    flag = False

        return flag
    else: 
        return False

def add_operando(instrucao,operando, tipo_operando, numero_operandos):              # o endereço do operando é adicionado à lista de bytes e
    global dic_variaveis, contador_byte, proxima_variavel                           # dependendo do tipo do operando, é incrementado 1 ou 2 bytes ao contador de bytes

    for i in range(0, numero_operandos, 1):
        if tipo_operando[i] == "varnum":
            lista_bytes.append(dic_variaveis[operando[i]])
            contador_byte += 1

        elif tipo_operando[i] == "new_varnum":
            if operando[i] not in dic_variaveis.keys():
                dic_variaveis[operando[i]] = proxima_variavel
                proxima_variavel += 1
                        
            lista_bytes.append(dic_variaveis[operando[i]])
            contador_byte += 1
            
        elif tipo_operando[i] ==  "byte" or tipo_operando[i] == "const":
            lista_bytes.append(int(operando[i]))
            contador_byte += 1            

        elif tipo_operando[i] == "disp" or tipo_operando[i] == "index": 
            lista_bytes.append(int(operando[i]) & 0xff)
            lista_bytes.append(int(operando[i]) >> 8)
            contador_byte += 2
        else:
            lista_bytes.append([operando[i], contador_byte])
            contador_byte += 2

def gerar_executavel():                             # criando o arquivo executável
    global lista_bytes, contador_linha, contador_byte

    bytes_para_gravacao = bytearray()               # array de bytes que será escrita no arquivo
    
    tamanho_arquivo = (contador_byte + 20).to_bytes(4, "little", signed = True)     # arquivo deve iniciar com 4 bytes indicando o tamanho do programa
    
    bytes_para_gravacao += tamanho_arquivo 

    bytes_inicializacao = [0x7300, 0x0006, 0x1001, 0x0400, 0x1001 + len(dic_variaveis.keys())]      # bytes pré-definidos de inicialização 
    
    try:
        for byte in bytes_inicializacao:
            bytes_para_gravacao += byte.to_bytes(4, "little", signed = True)         # adicionando os bytes de inicialização

        for byte in lista_bytes:                          
            if type(byte) == list:                                                   # checa se o operando é do tipo offset
                if byte[0] not in dic_labels.keys():
                    add_erro("Label não encontrada", contador_linha)                 # erro se a label não existe  
                else:
                    byte_label = dic_labels[byte[0]] - byte[1]                       # deve-se pegar a distância entre o endereço da label e da instrução em que a label é operando
                    bytes_para_gravacao += byte_label.to_bytes(2, "big", signed = True)   # a distância é adicionada
            else:
                bytes_para_gravacao.append(byte)                                     # se não é label, é adicionado à array 
    except Exception:
        print("Erro ao gerar o arquivo")
        print(tela_erro)

    arquivo_executavel = open(sys.argv[1][:-4] + ".exe", 'wb')
    arquivo_executavel.write(bytes_para_gravacao)
    arquivo_executavel.close()

def main():
    global contador_linha

    try:                                                         # tenta abrir o arquivo
        arq_asm = open(sys.argv[1], 'r')
    except IndexError:                                           # não é passado parametro para leitura
        print("Não há arquivo para abrir")
        raise IndexError
    except IOError:                                              # o arquivo passado não existe 
        print("Não é possível abrir o arquivo")
        raise IOError

    for linha in arq_asm:
        contador_linha += 1

        linha_lista = linha.lower().split()                       # não é case sensitive
        
        try:                                                       
            if checar_label(linha_lista[0]):                      # vai checar se a linha possui label
                add_label(linha_lista[0])                         # se possuir, label é adicionada ao seu dicionario
                del linha_lista[0]
        except IndexError:                                        # caso a linha não possua label, passar-se-á para a próxima
            pass

        if linha_lista != [] and not checar_comentario(linha_lista[0]):
            instrucao = linha_lista[0]
            
            # a partir daqui, cada instrução é checada e, sendo válida, faz-se a checagem do(s) operando(s)
            if checar_instrucao_valida(instrucao):
                tipo_operando = dic_instrucoes[instrucao][1]
                numero_operandos = len(tipo_operando)
                operando = linha_lista[1 : numero_operandos + 1]

                if checar_operando_valido(instrucao, operando, tipo_operando, numero_operandos):
                    add_instrucao(instrucao)
                    add_operando(instrucao, operando, tipo_operando, numero_operandos)

                else:
                    add_erro("Operando inválido", contador_linha)
            
            else:
                add_erro("Instrução inválida", contador_linha)

    arq_asm.close()  

    if tela_erro == "":
        gerar_executavel()
    
    else:
        print(tela_erro)
            
if __name__ == '__main__':                      # looping
    main()   
 



