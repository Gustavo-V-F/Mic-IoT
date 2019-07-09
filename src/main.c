#include "../lib/def_principais.h"

FILE *lcd_stream;

void f_leSensor(void);
void f_lePacote(void);
void f_enviaPacote(void);
void f_recebePacote(void);
void f_escreveTela(void);

typedef struct
{
	uint8_t lePacoteOK:1;
	uint8_t enviaPacoteOK:1;
	uint8_t erroPacote:1;
	uint8_t erroPacoteOK:1;
	uint8_t naoUtilizado:4;
}TESTE_Type;

volatile TESTE_Type teste; 

/* Definição dos estados */
typedef enum {
	leSensor,
	lePacote,
	enviaPacote,
	recebePacote,
	escreveTela,
	NUM_STATES
} state_t;

/* Definição da estrutura que mantém o vetor de estados */
typedef struct {
	state_t mqf;
	void (*func)(void);
}fsm_t;

/* Mapeamento entre estado e funções */
fsm_t mqf[] = {
	{ leSensor, f_leSensor },
	{ lePacote, f_lePacote },
	{ enviaPacote, f_enviaPacote },
	{ recebePacote, f_recebePacote },
	{ escreveTela, f_escreveTela }
};

/* Estado atual */
volatile state_t curr_state = leSensor;
volatile uint16_t dados = 0;
uint16_t acelDados[3] = {0}, gyroDados[3] ={0};

int main(){

	timer0_hardware_init();
	modbus_rtu_init();
	/* Inicialização do LCD */
	//lcd_stream = inic_stream();
	//inic_LCD_4bits();
	teste.lePacoteOK = 0;
	teste.enviaPacoteOK = 0;
	teste.erroPacote = 0;

	sei();

	MPU6050_init();

	for(;;){
		(*mqf[curr_state].func)();
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t i = 0;
	static uint16_t modbusCheck;
	modbusCheck =  (modbus_rtu_check() & 0x00FF);
	switch (modbusCheck)
	{
	case 0x0001:
		teste.enviaPacoteOK = 0;
		teste.erroPacoteOK = 1;
		curr_state = enviaPacote;
		break;
	case 0x0002:
		dados = get_modbus_rtu_data();
		teste.lePacoteOK = 0;
		teste.erroPacoteOK = 1;
		curr_state = enviaPacote;
		break;
	
	case 0x00FF:
		teste.lePacoteOK = 1;
		teste.enviaPacoteOK = 0;
		teste.erroPacote = 0;
		teste.erroPacoteOK = 0;
		curr_state = recebePacote;
		break;

	case 0x00FE:
		teste.lePacoteOK = 1;
		teste.enviaPacoteOK = 0;
		teste.erroPacote = 1;
		teste.erroPacoteOK = 0;
		curr_state = recebePacote;
		break;

	default:
		break;
	}

	switch (curr_state) {
		case leSensor:
			if (i > 1){
				i = 0;
				curr_state = lePacote;
			}
			break;

		case lePacote:
			if (i > 126){
				i = 0;
				//teste.lePacoteOK = 0;
				//curr_state = enviaPacote;
			}
			break;

		case enviaPacote:
			if (i > 255){
				i = 0;
				teste.lePacoteOK = 0;
				//teste.enviaPacoteOK = 0;
				curr_state = escreveTela;
			}
			break;

		case recebePacote:
			if (i > 126){
				i = 0;			
				//curr_state = escreveTela;
			}
			break;

		case escreveTela:
			if (i > 1){
				i = 0;
				curr_state = leSensor;
			}
			break;

		default:
			break;
	}

	i++;
}

void f_leSensor(void){
	//sensor
}

void f_lePacote(void){
	if(teste.lePacoteOK == 0)
		modbus_rtu_read(0x0001);

	teste.lePacoteOK = 1;
	
	_delay_ms(30);
}

void f_enviaPacote(void){
	static uint8_t j = 0;
	if(teste.enviaPacoteOK == 0){
		if(j > 3)
			j = 0;

		modbus_rtu_write(0x05 + j, (gyroDados[j]));
		j++;
	}
	teste.enviaPacoteOK = 1;
	MPU6050_get_raw_data(acelDados, gyroDados);
	_delay_ms(30);
}

void f_recebePacote(void){
	uint8_t erro = 0xFF; 

	if(teste.erroPacote == 1)
		erro = 0xFE;

	if(teste.erroPacoteOK == 0)
		modbus_rtu_error(erro);
	_delay_ms(1000);
}

void f_escreveTela(void){
}