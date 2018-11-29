/********************************************************************************

M�quina virtual com mem�ria cache

	Este c�digo implementa uma m�quina virtual (interpretador) capaz de buscar,
decodificar e executar um conjunto de instru��es armazenado em uma mem�ria cache,
que foi constru�da para carregar informa��es da mem�ria principal de programa.
O c�digo e este documento foram desenvolvidos pelos alunos Amanda de C�ssia Mendes 
Mota, Luiz Felipe Ribeiro de Oliveira e Milena Machado Ferreira, graduandos do 6� per�odo
do curso de Engenharia da Computa��o do Instituto Nacional de Telecomunica��es - INATEL. 
	Este trabalho tem como principal objetivo aprimorar os conhecimentos adquiridos 
na disciplina de Arquitetura de Computadores II. 
***********************************************************************************

Detalhes do set de instru��o

	Tipo: CISC

	Tamanho das instru��es: 32 bits
	
	C�digo das instru��es:
	
		ADD: 	0001
		SUB: 	0010
		PROD:   0011
		DIV:    0100
	

	 Estrutura da Instru��o: 
	     
             MSB                                                                                LSB
		   
		(Tipo instr.) (End. Mem 1) (End. Reg 1)(End. Mem 2) (End. Reg 2) (End Reg Dest.)  (End Mem Dest.)
          
           4 bits        4 bits       4 bits       4 bits        4 bits       4 bits           8 bits
           
		   
         - Exemplo: 0b00010000000000010001001000000010 >>> 0001|0000|0000|0001|0001|0010|00000010
           	 	
   	 	Realiza load da memoria (0000 >> end. Mem 0) no registro (0000 >> end. Reg 0)
   	 	Realiza load da memoria (0001 >> end. Mem 1) no registro (0001 >> end. Reg 1)
   	 	Realiza a soma (0001 >> tipo da instru��o) e armazena no registro (0010 >> end. Reg 2)
   	 	Realiza store do registrador (0010 >> end. Reg 0) na memoria (00000010 >> end. Mem 0)
   	 	
	Estrutura da Cache
	 
   	    v       TAG     WORD1      WORD2  
 	 ______________________________________
	|    	|        |         |          |
    | 1 bit	| 30 bit | 32 bit  |  32 bit  |   LINHA 1
	|_______|________|_________|__________|
    |    	|        |         |          |
    | 1 bit	| 30 bit | 32 bit  |  32 bit  |   LINHA 0
	|_______|________|_________|__________|

	
	Estrutura da TAG
	
    _______________________________
    |        |         |          |
    | 30 bit | 1  bit  |   1 bit  |   
	|________|_________|__________|
	
	Explica��o do carregamento da cache
	
	Ap�s utilizar o valor de PC para obter os valores de "linha", "palavra" e "tag", a fun��o "validaDados" � chamada. 
	Nela confere-se a autenticidade dos dados, caso estejam incorretos, a fun��o " loadCache" � chamada e nela os dados 
	s�o carregados a partir da mem�ria de programa.

		 	 
 	 	 	 
********************************************************************************/

#include <iostream>

using namespace std;

struct Linha_Memoria_Cache
{
	bool bitValid;
	unsigned int tag;
	unsigned int data[2];
};

// Memoria de programa
unsigned int ProgMemory[] = {0b00010000000000010001001000000010, //soma
							 0b00100000000000010001001000000011, //subtra��o
							 0b00110000000000010001001000000100, //produto
							 0b01000000000000010001001000000101, //divisao
							 0b00010000000000010001001000000010, //soma
							 0b00100000000000010001001000000011, //subtra��o
							 0b00110000000000010001001000000100, //produto
							 0b01000000000000010001001000000101,  //divisao
							 0b00010000000000010001001000000010, //soma
							 0b00100000000000010001001000000011, //subtra��o
							 0b00110000000000010001001000000100, //produto
							 0b01000000000000010001001000000101  //divisao
							 };
							 
// Memoria de dados
unsigned int DataMemory[] = { 6, 3, 0, 0, 0, 0, 0, 0};

// Registradores
unsigned int PC; //Program Counter = aponta para o endere�o da pr�xima instru��o a ser executada
unsigned int Instr; //Instru��o = aponta para o endere�o da instru��o que est�/ser� executada
unsigned int InstrType; //Tipo da Instru��o = guarda o tipo da instru��o que est�/ser� executada
unsigned int endMemA; // Endere�o de Mem�ria do primeiro termo "A" da opera��o = 
unsigned int endRegA; //Endere�o do Registrador "A" = 
unsigned int endMemB; // Endere�o de Mem�ria do segundo termo "B" da opera��o = 
unsigned int endRegB; // Endere�o do Registrador "B" = 
unsigned int endMemDest; // Endere�o de Mem�ria Destino = 
unsigned int endRegDest; // Endere�o de Registrador Destino = 
unsigned int Reg[10];

//Cache
Linha_Memoria_Cache memoriaCache[2];
unsigned int linha; //Linha da memoriaCache
unsigned int palavra; //palavra da memoriaCache

unsigned int tagMostrar;


void decode(void);
void execute(void);
void loadCache(int pc);
void validaDados(int pc);

int main()
{
	setlocale(LC_ALL,"Portuguese");
		 
	cout<<"Mem�ria antes da execu��o: ";
	for(int i=0;i < 8;i++)
		cout << DataMemory[i] << " ";
	cout<<endl;
		
	//Inicializa��o dos registros
	PC = 0b00000000000000000000000000000000;
	for(char i = 0; i < 10; i++)
	{
		Reg[i] = 0;
	}
	
	for(char i = 0; i < 2; i++)
	{
		memoriaCache[i].bitValid = false;
	}
	
	while(PC < 12)
	{
		linha = PC >> 1;
		linha = linha & 0b00000000000000000000000000000001;
		palavra = PC & 0b00000000000000000000000000000001;
		
		tagMostrar = PC >> 2;
		
		cout<<PC<<endl;
		
		validaDados(PC);
		
		Instr = memoriaCache[linha].data[palavra]; //Busca da instru��o
		
		PC = PC + 1;
		
		decode();   
		execute();
	}
	
	cout<<endl;
	cout<<"Memoria depois da execu��o: ";
	for(char i=0;i < 8;i++)
		cout << DataMemory[i] << " ";
	cout<<endl;

    return 0;       
}

void decode(void)
{
	InstrType = Instr >> 28;
	
	endRegA = Instr >> 20; 
	endRegA = endRegA & 0b00000000000000000000000000001111;
	endMemA = Instr >> 24;
	endMemA = endMemA & 0b00000000000000000000000000001111; 	

	endRegB = Instr >> 12;
	endRegB = endRegB & 0b00000000000000000000000000001111; 
	endMemB = Instr >> 16;
	endMemB = endMemB & 0b00000000000000000000000000001111; 

	endRegDest = Instr >> 8;
	endRegDest = endRegDest & 0b00000000000000000000000000001111; 
	
	endMemDest = Instr & 0b00000000000000000000000011111111; 
}

void execute(void)
{
	//Load A
	Reg[endRegA] = DataMemory[endMemA];
	
	//Load B
	Reg[endRegB] = DataMemory[endMemB];
	
	if(InstrType == 1)//Soma 0001
	{
		Reg[endRegDest] = Reg[endRegA] + Reg[endRegB];
		//store
		DataMemory[endMemDest] = Reg[endRegDest];
	}
	else if(InstrType == 2)//subtra��o 0010
	{
		Reg[endRegDest] = Reg[endRegA] - Reg[endRegB];
		//store
		DataMemory[endMemDest] = Reg[endRegDest];
	}
	else if(InstrType == 3)//produto 0011
	{
		Reg[endRegDest] = Reg[endRegA] * Reg[endRegB];
		//store
		DataMemory[endMemDest] = Reg[endRegDest];
		
	}
	else if(InstrType == 4)//divisao 0100
	{
		Reg[endRegDest] = Reg[endRegA] / Reg[endRegB];
		//store
		DataMemory[endMemDest] = Reg[endRegDest];
	}	
}

void loadCache(int pc)
{
	memoriaCache[linha].bitValid = true;
	memoriaCache[linha].tag = PC >> 2;
	memoriaCache[linha].data[0] =  ProgMemory[pc];
	memoriaCache[linha].data[1] =  ProgMemory[pc+1];	
}

void validaDados(int pc)
{
	bool bitValid = memoriaCache[linha].bitValid;
	int tagAux = memoriaCache[linha].tag;
	
	if(bitValid == false)//inconsistencia
	{
		loadCache(pc);
	}//& 0b11111111111111111111111111111100
	else if(tagAux != (PC >> 2))//miss
	{
		loadCache(pc);
	}
}


