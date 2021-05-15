/*
 * SE
 */

#include "board.h"
#include "arm_math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#define PORT(n)		((uint8_t) n)
#define PIN(n)		((uint8_t) n)
#define GRUPO(n)	((uint8_t) n)

#define PORT_TEC_F3   4
#define PIN_TEC_F3	  3

#define ADC_0		0
#define CHANNEL_0	0

#define ORDEN_FILTRO	4

arm_fir_instance_f32 Filtro_FIR;

QueueHandle_t Dato_Salida_FIR;
QueueHandle_t Dato_Salida_IIR;

SemaphoreHandle_t Semphr_ADC;



/*****************************************************************************
 * Private functions
 ****************************************************************************/

void ADC0_IRQHandler (void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR( Semphr_ADC, &xHigherPriorityTaskWoken );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();

	Chip_GPIO_Init(LPC_GPIO_PORT);

	Chip_SCU_PinMux( GRUPO(PORT_TEC_F3) , PIN(PIN_TEC_F3) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_ADC_Channel_Config (ADC_0 , CHANNEL_0);
}


unsigned int Leer_ADC (void)
{
	static unsigned int i = 0;
	unsigned int Valores_Cicliclos [8] = {5,6,4,5,6,4,7,3};

	return (Valores_Cicliclos [(i++) % ORDEN_FILTRO]);
}

void DAC_Output (unsigned int b)
{
	b ++;
}

void DAC_Output_DSP (unsigned int b)
{
	b ++;
}


/* FILTRO FIR */
static void FILTRO_FIR (void *pvParameters)
{
	static float Datos_ADC [ORDEN_FILTRO];
	static float b [ORDEN_FILTRO] = {0.25, 0.5, 1.0, 2.0};
	static float Salida_DSP;

	static unsigned int Indice = 0;
	static unsigned int i = 0;

	arm_fir_init_f32 (&Filtro_FIR, ORDEN_FILTRO, b, Datos_ADC, 1);

	while (1)
	{

		if (Indice < ORDEN_FILTRO) //Carga vector
		{
			Datos_ADC [Indice] = (float) Leer_ADC ();

			Indice ++;
		}

		if (Indice == ORDEN_FILTRO)//Una vez cargado
		{
			for (i = 0; i < (ORDEN_FILTRO - 1); i++)
				Datos_ADC [i] = Datos_ADC [i + 1];

			Datos_ADC [ORDEN_FILTRO - 1] = (float) Leer_ADC ();

			Salida_DSP = 0;

			arm_fir_f32 (&Filtro_FIR, Datos_ADC, &Salida_DSP, 1);
			xQueueSend(Dato_Salida_FIR, &Salida_DSP, portMAX_DELAY);

		}

	}
}

/* FILTRO IIR */
static void FILTRO_IIR (void *pvParameters)
{
	static float Datos_ADC [ORDEN_FILTRO];
	static float a [ORDEN_FILTRO] = {3.25, 1.0, 4.15, 5.20};
	static float b [ORDEN_FILTRO] = {2.0, 1.45, 2.10, 0.85};
	static float y[ORDEN_FILTRO], Salida;

	static unsigned int Indice = 0;
	static unsigned int i = 0;
	static unsigned int j = 0;
	static unsigned int k = 0;

	while (1)
	{

		if (Indice < ORDEN_FILTRO) //Carga vector
		{
			Datos_ADC [Indice] = (float) Leer_ADC ();

			Indice ++;
		}

		if (Indice == ORDEN_FILTRO)//Una vez cargado
		{
			for (i = 0; i < (ORDEN_FILTRO - 1); i++)
				Datos_ADC [i] = Datos_ADC [i + 1];

			Datos_ADC [ORDEN_FILTRO - 1] = (float) Leer_ADC ();

			Salida = 0;
			 y[0] = 0;

			for (i = 0; i < ORDEN_FILTRO; i++)
			{
				Datos_ADC [i] = Datos_ADC [i+1];
				y[i] = y[i+1];
			}


			for (j = 0; j < ORDEN_FILTRO; j++)
				Salida += Datos_ADC [i] * b[i];

			for (k = 0; k < ORDEN_FILTRO; k++)
				Salida += a [i] * y[i];


			xQueueSend(Dato_Salida_IIR, &Salida, portMAX_DELAY);
		}

	}
}

/* Procesamiento de Datos */
static void DAC_FIR_Output (void *pvParameters)
{
	static float Salida_Datos_DSP;

	while (1)
	{
		xQueueReceive (Dato_Salida_FIR, &Salida_Datos_DSP, portMAX_DELAY);
		DAC_Output_DSP ( (unsigned int) Salida_Datos_DSP);
	}
}

static void DAC_IIR_Output (void *pvParameters)
{
	static float Salida_Datos;

	while (1)
	{
		xQueueReceive (Dato_Salida_IIR, &Salida_Datos, portMAX_DELAY);
		DAC_Output ( (unsigned int) Salida_Datos);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();

	vSemaphoreCreateBinary( Semphr_ADC );

	Dato_Salida_FIR = xQueueCreate( 1, sizeof (float) );
	Dato_Salida_IIR = xQueueCreate( 1, sizeof (float) );

	xSemaphoreTake (Semphr_ADC , portMAX_DELAY);

	/* FIR*/
	xTaskCreate(FILTRO_FIR, "FILTRO_FIR", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* IIR */
	xTaskCreate(FILTRO_IIR, "FILTRO_IIR", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Salida de Datos */
	xTaskCreate(DAC_FIR_Output, "DAC_FIR_Output", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Salida de Datos */
	xTaskCreate(DAC_IIR_Output, "DAC_IIR_Output", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
