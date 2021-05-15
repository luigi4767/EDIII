/*****************************************************************************
 * PROCESAMIENTO DE SEÑALES MEDIANTE FILTRO FIR Version 1
 * Proyecto EDIII funcional de captura/procesamiento de muestras adquiridas
 * desde el adc, procesadas mediante un filtro digital FIR y transmision de
 * los datos mediante uart,para el posterior post procesamiento.
 * Autores: Luis Abanto
 * 			Franco Rota
 *
 * La Version 2 contendrá la adaptación del firmware con FreeRTOS
 ****************************************************************************/

#include "chip.h"
#include "arm_math.h"
#include "math_helper.h"
#include "stdio.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define  SYSTEM_BAUD_RATE 	9600
#define ADC_ID LPC_ADC0
#define ADC_CHANNEL 1
#define USART_ID LPC_USART2
#define SAMPLING_RATE 20000 //Muestreo a 20kHz


/** Definicion de MACROS **/
#define TEST_LENGTH_SAMPLES  320
#define BLOCK_SIZE            32
#define NUM_TAPS              29

static float32_t testOutput[TEST_LENGTH_SAMPLES];
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1]; //buffer de estado

/* Buffer de coeficientes FIR con MATLAB fir1(28, 6/24) */
const float32_t firCoeffs32[NUM_TAPS]={-0.00179256693870527,-0.00187888351193098,-0.00202850558385614,-0.00167121003424694,1.02275721635981e-18,0.00385526032868034,0.0106060380434773,0.0205854549583695,0.0335846185154550,0.0487894686034313,0.0648420439362281,0.0800227460083969,0.0925224258960228,0.100751512354513,0.103623194848330,0.100751512354513,0.0925224258960228,0.0800227460083969,0.0648420439362281,0.0487894686034313,0.0335846185154550,0.0205854549583695,0.0106060380434773,0.00385526032868034,1.02275721635981e-18,-0.00167121003424694,-0.00202850558385614,-0.00187888351193098,-0.00179256693870527};

/* -------------------------------*
 * Variables Globales del LPF FIR *
 * -------------------------------*/
uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

/* -------------------------------*
 * INICIALIZACION DE PERIFERICOS  *
 * -------------------------------*/
void uart_init(void){
   Chip_UART_Init(USART_ID);
   Chip_UART_SetBaud(USART_ID, SYSTEM_BAUD_RATE);
   Chip_UART_SetupFIFOS(USART_ID, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
   Chip_UART_TXEnable(USART_ID); // Enable UART Transmission

   Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              /* P7_1: UART2_TXD */
   Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); /* P7_2: UART2_RXD */

   //Enable UART Rx Interrupt
   Chip_UART_IntEnable(USART_ID,UART_IER_RBRINT);   //Receiver Buffer Register Interrupt

   //Chip_UART_IntEnable(USART_ID,UART_IER_RLSINT ); //LPC43xx User manual page 1118
   NVIC_SetPriority(USART2_IRQn, 6);

   // Enable Interrupt for UART channel
   //NVIC_EnableIRQ(USART2_IRQn);
   //NVIC_EnaIRQ(USART2_IRQn);
}

void adc_init(void) {
	static ADC_CLOCK_SETUP_T ADCSetup;
	Chip_ADC_Init(ADC_ID, &ADCSetup);
	Chip_ADC_SetSampleRate(ADC_ID, &ADCSetup, SAMPLING_RATE);
    Chip_ADC_EnableChannel(ADC_ID, ADC_CHANNEL, ENABLE);
	Chip_ADC_SetResolution(ADC_ID, &ADCSetup, ADC_10BITS);
}

/* Polling routine for ADC example */
static int App_Polling_Test(uint16_t bufferADC[320]){
	uint16_t dataADC;
	Chip_ADC_SetBurstCmd(ADC_ID, DISABLE);

	while (1) {
		Chip_ADC_SetStartMode(ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		while (Chip_ADC_ReadStatus(ADC_ID, ADC_CHANNEL, ADC_DR_DONE_STAT) != SET) {} /* Waiting for A/D conversion complete */
		Chip_ADC_ReadValue(ADC_ID, ADC_CHANNEL, &dataADC); /* Read ADC value */
		return dataADC;
	}
}


/* --------------------
 * Declaración de main
 * -------------------*/
uint32_t i;
arm_fir_instance_f32 S;
float32_t  *inputF32, *outputF32;


unsigned char Mensaje[] =  "\r\n\r\nDatos adquiridos por el ADC: \r\n";
unsigned char Mensaje2[] = "\r\n\r\nSeñal filtrada: \r\n";
unsigned char Fin[] = 	   "\r\nFin de conversion \r\n\r\n";

int32_t main(void){

	float32_t testInput_f32_1kHz_8kHz[TEST_LENGTH_SAMPLES];
	uint16_t bufferADC[TEST_LENGTH_SAMPLES];
	uint16_t data[TEST_LENGTH_SAMPLES];
	int muestra=0;
	unsigned char valores[20];
	unsigned char filtro[20];
	int contador1=0;
	int contador2=0;
	int contador3=0;

	SystemCoreClockUpdate();
	uart_init();
	adc_init();

	Chip_UART_SendBlocking (USART_ID , Mensaje, strlen(Mensaje)); //printf(Mensaje)

	while(contador1 < 320){
		muestra = (int) App_Polling_Test(bufferADC[320]);
		testInput_f32_1kHz_8kHz[contador1]=muestra*3.3/1024;
		data[contador1] = muestra; //array con datos de muestra
		contador1++;
	}

	while(contador2 < 320){
		uint8_t num = sprintf(valores, "%.10f,", testInput_f32_1kHz_8kHz[contador2]);
		Chip_UART_SendBlocking (USART_ID , valores, num);
		contador2++;
	}

	/* Iniciamos punteros a buffer de entrada y salida: */
	inputF32 = &testInput_f32_1kHz_8kHz[0];
	outputF32 = &testOutput[0];

	/* Llamamos a la funcion FIR_init para inicializar una instancia de la estructura: */
	arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);

	/* --------------------------------------------------------------
	** Llamada la funcion del proceso FIR para cada muestra blockSize
	** ------------------------------------------------------------ */
	for(i=0; i < numBlocks; i++)
		arm_fir_f32(&S, inputF32 + (i * blockSize), outputF32 + (i * blockSize), blockSize);

	Chip_UART_SendBlocking (USART_ID , Mensaje2, strlen(Mensaje2)); //printf(Mensaje2)

	while(contador3 < 320){
		uint8_t num2 = sprintf(filtro, "%.10f,", outputF32[contador3]);
		Chip_UART_SendBlocking (USART_ID , filtro, num2);
		contador3++;
	}


	Chip_UART_SendBlocking (USART_ID , Fin, strlen(Fin)); //printf("Fin de conversion");

	return 0;
}

