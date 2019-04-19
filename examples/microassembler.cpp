#include <iostream>
#include <fstream>
#include <string>

#define MREAD 2
#define MWRITE 4
#define MFETCH 1

using namespace std;

typedef unsigned int word;
typedef unsigned char byte;
typedef unsigned long microcode;

microcode rom[512]; //microprograma máximo = 512 microcódigos (512 x 36 bits). Para facilitar manipulação de memória, cada microcódigo é 
                    //armazenado em 64 bits, sendo zerados os 28 bits de alta ordem. Tamanho máximo do microprograma = 512 x 8 bytes = 4 KBytes

microcode workcode = 0;
word mpc = 0;
word next_address = 1;

void address_encode();

void save() //grava rom em arquivo
{
    ofstream from("microprog.rom", ios::binary);

    from.write( (char*)&(rom[0]), 512*sizeof(microcode) );

    from.close();
}

void new_(word addr = 512) //codifica end. do prox. microcódigo; grava microcode no endereço especificado e começa novo
{

    address_encode();

    if(addr == 512)
    {
        rom[mpc] = workcode;
        mpc++;
    }
    else
        rom[addr] = workcode;

    workcode = 0;

    next_address = mpc + 1; //por padrão, o próximo endereço da instrução é o atual mais um, podendo ser modificado posteriormente alterando next_address
}

void address_encode()
{
    microcode code = next_address;

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 27;  //11111111111111111111111111111111 11111000000000000000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000111111111111111111111111111
    aux = aux << 36;    //11111111111111111111111111110000 00000000000000000000000000000000
    mask = mask | aux;  //11111111111111111111111111110000 00000111111111111111111111111111

    code = code << 27;  //0000000000000000000000000000cccc ccccc000000000000000000000000000

    workcode = workcode & mask;
    workcode = workcode | code;
}

void renc_src(string src) //codifica registrador origem
{
    microcode code = 0;

    if(src == "mdr")
        code = 0;
    else if(src == "pc")
        code = 1;
    else if(src == "mbr")
        code = 2;
    else if(src == "mbru")
        code = 3;
    else if(src == "sp")
        code = 4;
    else if(src == "lv")
        code = 5;
    else if(src == "cpp")
        code = 6;
    else if(src == "tos")
        code = 7;
    else if(src == "opc")
        code = 8;

    workcode = workcode & 0xFFFFFFFFFFFFFFF0;
    workcode = workcode | code;
}

void renc_dest(string dest) //codifica registrador destino
{
    microcode code = 0;

    if(dest == "h")
       code = 256;
    else if(dest == "opc")
       code = 128;
    else if(dest == "tos")
       code = 64;
    else if(dest == "cpp")
       code = 32;
    else if(dest == "lv")
       code = 16;
    else if(dest == "sp")
       code = 8;
    else if(dest == "pc")
       code = 4;
    else if(dest == "mdr")
       code = 2;
    else if(dest == "mar")
       code = 1;

    code = code << 7; //00000000000000000000000000000000 0000000000000000ccccccccc0000000

    workcode = workcode | code;
}

void alu_encode(microcode code) //codifica comando da alu
{
    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 16;  //11111111111111111111111111111111 11111111111111110000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000000000000001111111111111111
    aux = aux << 22;    //11111111111111111111111111111111 11111111110000000000000000000000
    mask = mask | aux;  //11111111111111111111111111111111 11111111110000001111111111111111

    code = code << 16;  //00000000000000000000000000000000 0000000ccccccccc0000000000000000

    workcode = workcode & mask;
    workcode = workcode | code;
}

void iset(string dest, string src) // dest = src (src pode ser h)
{
    if(src == "h")
    {
        renc_dest(dest);
        alu_encode(24);
    }
    else
    {
        renc_src(src);
        renc_dest(dest);
        alu_encode(20);
    }
}

void niset(string dest, string src) // dest = neg(src) (src pode ser h)
{
    if(src == "h")
    {
        renc_dest(dest);
        alu_encode(26);
    }
    else
    {
        renc_src(src);
        renc_dest(dest);
        alu_encode(44);
    }
}

void add_h(string dest, string src) // dest = h + source
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(60);
}

void iadd_h(string dest, string src) //dest = h + source + 1
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(61);
}

void inc_h(string dest) //dest = h + 1
{
    renc_dest(dest);
    alu_encode(57);
}

void inc(string dest, string src) //dest = src + 1
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(53);
}

void sub_h(string dest, string src) //dest = src - h
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(63);
}

void dec(string dest, string src) //dest = src - 1
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(54);
}

void mh(string dest) //dest =  -h
{
    renc_dest(dest);
    alu_encode(59);
}

void iand(string dest, string src) //dest = h and src
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(12);
}

void ior(string dest, string src) //dest = h or src
{
    renc_src(src);
    renc_dest(dest);
    alu_encode(28);
}

void zero(string dest) // dest = 0
{
    renc_dest(dest);
    alu_encode(16);
}

void one(string dest) //dest = 1
{
    renc_dest(dest);
    alu_encode(49);
}

void mone(string dest) //dest = -1
{
    renc_dest(dest);
    alu_encode(50);
}

void sll8() //adiciona deslocamento SLL8 ao microcodigo (01 - bits 23 e 24 do microcodigo)
{
    microcode code = 1; //01

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 22;  //11111111111111111111111111111111 11111111110000000000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000000001111111111111111111111
    aux = aux << 24;    //11111111111111111111111111111111 11111111000000000000000000000000
    mask = mask | aux;  //11111111111111111111111111111111 11111111001111111111111111111111

    code = code << 22;  //00000000000000000000000000000000 00000000010000000000000000000000

    workcode = workcode | code;
}

void igoto(word n_address) //atribui próximo endereço diretamente a um especificado
{
    next_address = n_address;
}

void jmpz(string reg, word else_address) //codifica bit jamz já determinando o próximo endereço e a operação da alu cujo resultado deve ser analisado se é zero
{
    next_address = else_address;

    if(reg != "") 
    {
       renc_src(reg);
       renc_dest("");
       alu_encode(20);
    }

    microcode code = 1; //001

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 24;  //11111111111111111111111111111111 11111111000000000000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000000111111111111111111111111
    aux = aux << 27;    //11111111111111111111111111111111 11111000000000000000000000000000
    mask = mask | aux;  //11111111111111111111111111111111 11111000111111111111111111111111

    code = code << 24;  //00000000000000000000000000000000 00000001000000000000000000000000

    workcode = workcode & mask;
    workcode = workcode | code;
}

void jmpn(string reg, word else_address) //codifica bit jamn já determinando o próximo endereço e a operação da alu cujo resultado deve ser analisado se é diferente de zero
{
    next_address = else_address;

    if(reg != "") 
    {
       renc_src(reg);
       renc_dest("");
       alu_encode(20);
    }

    microcode code = 2; //010

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 24;  //11111111111111111111111111111111 11111111000000000000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000000111111111111111111111111
    aux = aux << 27;    //11111111111111111111111111111111 11111000000000000000000000000000
    mask = mask | aux;  //11111111111111111111111111111111 11111000111111111111111111111111

    code = code << 24;  //00000000000000000000000000000000 00000010000000000000000000000000

    workcode = workcode & mask;
    workcode = workcode | code;
}

void gotoMBR(bool bit = false) //Faz o pulo direto para um endereço guardado em MBR. Se bit = true, então MBR | 0x100, ou seja, pula 256 em relação ao endereço guardado em MBR
{
    next_address = 0;

    microcode code = 4; //100

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 24;  //11111111111111111111111111111111 11111111000000000000000000000000
    mask = ~mask;       //00000000000000000000000000000000 00000000111111111111111111111111
    aux = aux << 27;    //11111111111111111111111111111111 11111000000000000000000000000000
    mask = mask | aux;  //11111111111111111111111111111111 11111000111111111111111111111111

    code = code << 24;  //00000000000000000000000000000000 00000100000000000000000000000000

    workcode = workcode & mask;
    workcode = workcode | code;

    if(bit)
    {

        code = 0x0000000000000100;

        mask = 0xFFFFFFFFFFFFFFFF;
        aux  = 0xFFFFFFFFFFFFFFFF;

        mask = mask << 27;  //11111111111111111111111111111111 11111000000000000000000000000000
        mask = ~mask;       //00000000000000000000000000000000 00000111111111111111111111111111
        aux = aux << 36;    //11111111111111111111111111110000 00000000000000000000000000000000
        mask = mask | aux;  //11111111111111111111111111110000 00000111111111111111111111111111

        code = code << 27;  //0000000000000000000000000000cccc ccccc000000000000000000000000000

        workcode = workcode & mask;
        workcode = workcode | code;

    }
}

void memop(byte rwf) //Codifica bits de controle de memória. MREAD | MWRITE | MFETCH
{
    microcode code = rwf;

    microcode mask = 0xFFFFFFFFFFFFFFFF;
    microcode aux  = 0xFFFFFFFFFFFFFFFF;

    mask = mask << 4;  //11111111111111111111111111111111 111111111111111111111111111110000
    mask = ~mask;      //00000000000000000000000000000000 000000000000000000000000000001111
    aux = aux << 7;    //11111111111111111111111111111111 111111111111111111111111110000000
    mask = mask | aux; //11111111111111111111111111111111 111111111111111111111111110001111

    code = code << 4;  //00000000000000000000000000000000 00000000000000000000000000rwf0000

    workcode = workcode & mask;
    workcode = workcode | code;
}

int main()
{

    //main = 0 (0x00) =======================================================================================
    inc("pc","pc");
    memop(MFETCH);
    gotoMBR();
    new_(); //main1

    //nop = 1 (0x01) =========================================================================================
    igoto(0);
    new_(); //nop1

    //iadd = 2 (0x02) ========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //iadd1

    iset("h","tos");
    new_(); //iadd2

    renc_dest("mdr");
    add_h("tos","mdr");
    memop(MWRITE);
    igoto(0);
    new_(); //iadd3

    //isub = 5 (0x05) =========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //isub1

    iset("h","tos");
    new_(); //isub2

    renc_dest("mdr");
    sub_h("tos","mdr");
    memop(MWRITE);
    igoto(0);
    new_(); //isub3

    //iand = 8 (0x08) =========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //iand1

    iset("h","tos");
    new_(); //iand2

    renc_dest("mdr");
    iand("tos","mdr");
    memop(MWRITE);
    igoto(0);
    new_(); //iand3

    //ior = 11 (0x0B) =========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //ior1

    iset("h","tos");
    new_(); //ior2

    renc_dest("mdr");
    ior("tos","mdr");
    memop(MWRITE);
    igoto(0);
    new_(); //ior3

    //dup = 14 (0x0E) =========================================================================================
    renc_dest("mar");
    inc("sp","sp");
    new_(); //dup1

    iset("mdr","tos");
    memop(MWRITE);
    igoto(0);
    new_(); //dup2

    //pop = 16 (0x10) ==========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //pop1

    new_(); //pop2

    iset("tos","mdr");
    igoto(0);
    new_(); //pop3

    //swap = 19 (0x13) =========================================================================================
    dec("mar","sp");
    memop(MREAD);
    new_(); //swap1

    iset("mar","sp");
    new_(); //swap2

    iset("h","mdr");
    memop(MWRITE);
    new_(); //swap3

    iset("mdr","tos");
    new_(); //swap4

    dec("mar","sp");
    memop(MWRITE);
    new_(); //swap5

    iset("tos","h");
    igoto(0);
    new_(); //swap6

    //bipush = 25 (0x19) ========================================================================================
    renc_dest("sp");
    inc("mar","sp");
    new_(); //bipush1

    inc("pc","pc");
    memop(MFETCH);
    new_(); //bipush2

    renc_dest("mdr");
    iset("tos","mbr");
    memop(MWRITE);
    igoto(0);
    new_(); //bipush3

    //iload = 28 (0x1C) =========================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); //iload0
 
    iset("h","lv");
    new_(); //iload1

    add_h("mar","mbru");
    memop(MREAD);
    new_(); //iload2

    renc_dest("mar");
    inc("sp","sp");
    new_(); //iload3

    memop(MWRITE);
    new_(); //iload4

    iset("tos","mdr");
    igoto(0);
    new_(); //iload5

    //istore = 34 (0x22) =======================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); //istore0

    iset("h","lv");
    new_(); //istore1

    add_h("mar","mbru");
    new_(); //istore2

    iset("mdr","tos");
    memop(MWRITE);
    new_(); //istore3

    renc_dest("sp");
    dec("mar","sp");
    memop(MREAD);
    new_(); //istore4

    iset("tos","mdr");
    igoto(0);
    new_(); //istore5

    //wide = 40 (0x28) ==========================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); //wide1

    gotoMBR(true);
    new_(); //wide2

    inc("pc","pc");
    memop(MFETCH);
    new_(); //wide_iload1

    iset("h","mbru");
    sll8();
    new_(); //wide_iload2

    ior("h","mbru");
    new_(); //wide_iload3

    add_h("mar","lv");
    memop(MREAD);
    igoto(30);
    new_(); //wide_iload4

    inc("pc","pc");
    memop(MFETCH);
    new_(); //wide_istore1

    iset("h","mbru");
    sll8();
    new_(); //wide_istore2

    ior("h","mbru");
    new_(); //wide_istore3

    add_h("mar","lv");
    igoto(35);
    new_(); //wide_istore4

    //ldc_w = 50 (0x32) =============================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); //ldc_w1

    iset("h","mbru");
    sll8();
    new_(); //ldc_w2

    ior("h","mbru");
    new_(); //ldc_w3

    add_h("mar","cpp");
    memop(MREAD);
    igoto(30);
    new_(); //ldc_w4

    //iinc = 54 (0x36) ==============================================================================================
    iset("h","lv");
    new_(); //iinc1

    add_h("mar","mbru");
    memop(MREAD);
    new_(); //iinc2

    inc("pc","pc");
    memop(MFETCH);
    new_(); //iinc3

    iset("h","mdr");
    new_(); //iinc4

    inc("pc","pc");
    memop(MFETCH);
    new_(); //iinc5

    add_h("mdr","mbr");
    memop(MWRITE);
    igoto(0);
    new_(); //iinc6

    //goto = 60 (0x3C) ==============================================================================================
    dec("opc","pc");
    new_(); //goto1

    inc("pc","pc");
    memop(MFETCH);
    new_(); //goto2

    iset("h","mbr");
    sll8();
    new_(); //goto3

    inc("pc","pc");
    memop(MFETCH);
    new_(); //goto4

    ior("h","mbru");
    new_(); //goto5

    add_h("pc","opc");
    memop(MFETCH);
    new_(); //goto6

    igoto(0);
    new_(); //goto7

    //iflt = 67 (0x43) =============================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //iflt1

    iset("opc","tos");
    new_(); //iflt2

    iset("tos","mdr");
    new_(); //iflt3

    jmpn("opc", 82); // se OPC Não é zero, vá para 338, senão vá para 82
    new_(); //iflt4

    //ifeq = 71 (0x47) =============================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //ifeq1

    iset("opc","tos");
    new_(); //ifeq2

    iset("tos","mdr");
    new_(); //ifeq3

    jmpz("opc", 82); //se OPC eh Zero, vá para 338, senão vá para 82
    new_(); //ifeq4

    //if_icmpeq = 75 (0x4B) ========================================================================================
    renc_dest("mar");
    dec("sp","sp");
    memop(MREAD);
    new_(); //if_icmpeq1

    renc_dest("mar");
    dec("sp","sp");
    new_(); //if_icmpeq2

    iset("h","mdr");
    memop(MREAD);
    new_(); //if_icmpeq3

    iset("opc","tos");
    new_(); //if_icmpeq4

    iset("tos","mdr");
    new_(); //if_icmpeq5

    sub_h("","opc");
    jmpz("", 82); //se OPC-H eh Zero, vá para 338, senão vá para 82
    new_(); //if_icmpeq6

    //instrução nula no end. 81. Deveria ser T, que estará no endereço 338 (apenas para facilitar endereçamento)
    new_(); 

    //T = 338 (0x152) ==============================================================================================
    dec("opc","pc");
    igoto(61);
    new_(338); //T

    //F = 82 (0x52) ================================================================================================
    inc("pc","pc");
    new_(); //F

    inc("pc","pc");
    memop(MFETCH);
    new_(); //F2

    igoto(0);
    new_(); //F3

    //invokevirtual = 85 (0x55) ====================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); //invokevirtual1

    iset("h","mbru");
    sll8();
    new_(); //invokevirtual2

    ior("h","mbru");
    new_(); //invokevirtual3

    add_h("mar","cpp");
    memop(MREAD);
    new_(); //invokevirtual4

    inc("opc","pc");
    new_(); //invokevirtual5

    iset("pc","mdr");
    memop(MFETCH);
    new_(); //invokevirtual6

    inc("pc","pc");
    memop(MFETCH);
    new_(); //invokevirtual7

    iset("h","mbru");
    sll8();
    new_(); //invokevirtual8

    ior("h","mbru");
    new_(); //invokevirtual9

    inc("pc","pc");
    memop(MFETCH);
    new_(); //invokevirtual10

    sub_h("tos","sp");
    new_(); //invokevirtual11

    renc_dest("tos");
    inc("mar", "tos");
    new_(); //invokevirtual12

    inc("pc","pc");
    memop(MFETCH);
    new_(); //invokevirtual13

    iset("h","mbru");
    sll8();
    new_(); //invokevirtual14

    ior("h","mbru");
    new_(); //invokevirtual15

    iadd_h("mdr","sp");
    memop(MWRITE);
    new_(); //invokevirtual16

    renc_dest("mar");
    iset("sp","mdr");
    new_(); //invokevirtual17

    iset("mdr","opc");
    memop(MWRITE);
    new_(); //invokevirtual18

    renc_dest("mar");
    inc("sp","sp");
    new_(); //invokevirtual19

    iset("mdr","lv");
    memop(MWRITE);
    new_(); //invokevirtual20

    inc("pc","pc");
    memop(MFETCH);
    new_(); //invokevirtual21

    iset("lv","tos");
    igoto(0);
    new_(); //invokevirtual22

    //ireturn = 107 (0x6B) ========================================================================================================
    renc_dest("mar");
    iset("sp","lv");
    memop(MREAD);
    new_(); //ireturn1

    new_(); //ireturn2

    renc_dest("lv");
    iset("mar","mdr");
    memop(MREAD);
    new_(); //ireturn3

    inc("mar","lv");
    new_(); //ireturn4

    iset("pc","mdr");
    memop(MREAD | MFETCH);
    new_(); //ireturn5

    iset("mar","sp");
    new_(); //ireturn6

    iset("lv","mdr");
    new_(); //ireturn7

    iset("mdr","tos");
    memop(MWRITE);
    igoto(0);
    new_(); //ireturn8

    //init = 115 (0x73) ===========================================================================================================
    //init (+3 bytes vazios) word1 word2 word3 word4, onde words = cpp, lv, pc e sp respectivamente. Configura regiões de memória.
    renc_dest("opc");
    inc("mar","opc");
    memop(MREAD);
    new_(); //init1 (115)

    iset("cpp","mdr");
    new_(); //init2

    renc_dest("opc");
    inc("mar","opc");
    memop(MREAD);
    new_(); //init3

    iset("lv", "mdr");
    new_(); //init4

    renc_dest("opc");
    inc("mar","opc");
    memop(MREAD);
    new_(); //init5

    iset("pc","mdr");
    memop(MFETCH);
    new_(); //init6

    renc_dest("opc");
    inc("mar","opc");
    memop(MREAD);
    new_(); //init7  

    iset("sp","mdr");
    new_(); //init8  

    igoto(0); //init9
    new_();

/*goto absoluto (7 linhas) ==========================================================================================
    inc("pc","pc");
    memop(MFETCH);
    new_(); 

    iset("opc","mbr");
    sll8();
    new_(); //goto1

    inc("pc","pc");
    memop(MFETCH);
    new_();

    iset("h","mbru");
    new_();

    ior("pc","opc");
    memop(MFETCH);
    new_();

    igoto(0);
    new_();
//*/

    save();

    return 0;
}
