#include "../lib/def_principais.h"

FILE *lcd_stream;

void f_stateA(void);
void f_stateB(void);
void f_stateC(void);

/* Definição dos estados */
typedef enum {
	STATE_A,
	STATE_B,
	STATE_C,
	NUM_STATES
} state_t;

/* Definição da estrutura mantenedora do vetor de estados */
typedef struct {
	state_t mqf;
	void (*func)(void);
}fsm_t;

/* Mapeamento entre estado e funções */
fsm_t mqf[] = {
	{ STATE_A, f_stateA },
	{ STATE_B, f_stateB },
	{ STATE_C, f_stateC },
};

/* Estado atual */
volatile state_t curr_state = STATE_A;

int main(){

	timer0_hardware_init();

	/* Inicialização do LCD */
	lcd_stream = inic_stream();
	inic_LCD_4bits();

	sei();

	for(;;){
		(*mqf[curr_state].func)();
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t i = 0;

	switch (curr_state) {
		case STATE_A:
			if (i > 4){
				i = 0;
				curr_state = STATE_B;
			}

			break;

		case STATE_B:
			if (i > 10){
				i = 0;
				curr_state = STATE_C;
			}
			break;

		case STATE_C:
			if (i > 1){
				i = 0;
				curr_state = STATE_A;
			}
			break;

		default:
			break;
	}

	i++;
}

void f_stateA(void){
}

void f_stateB(void){
}

void f_stateC(void){
}