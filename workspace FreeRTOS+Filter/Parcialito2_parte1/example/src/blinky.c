/*
 * SE IMPLEMENTA UN FILTRO FIR SIN CMSIS USANDO FREERTOS COLAS Y SEMAFOROS
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

#define PORT_TEC1   0
#define PORT_TEC2	0
#define PORT_TEC3	0
#define PORT_LED3	1

#define PIN_TEC1	4
#define PIN_TEC2	8
#define PIN_TEC3	9
#define PIN_LED3	12

#define ADC_0		0
#define CHANNEL_0	0

#define ORDEN_FILTRO	8

QueueHandle_t Dato_Salida;

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

	Chip_SCU_PinMux( GRUPO(1) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(6) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(2) , PIN(12) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );

	Chip_SCU_PinMux( GRUPO(4) , PIN(3) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_ADC_Channel_Config (ADC_0 , CHANNEL_0);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(4));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(8));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(9));

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(11));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(12));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(0) , PIN(14));

	Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(11) , (bool) false);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(12) , (bool) false);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(0) , PIN(14) , (bool) false);
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


/* Procesamiento de Datos */
static void Procesa_Datos (void *pvParameters)
{
	static float Datos_ADC [ORDEN_FILTRO];
	static float b [ORDEN_FILTRO] = {0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125};
	static float Salida;

	static unsigned int Indice = 0;
	static unsigned int i = 0;

	while (1)
	{
		//xSemaphoreTake (Semphr_ADC , portMAX_DELAY);

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

			for (i = 0; i < ORDEN_FILTRO; i++)
				Salida += Datos_ADC [i] * b[i];

			xQueueSend(Dato_Salida, &Salida, portMAX_DELAY);
		}

	}
}

/* Procesamiento de Datos */
static void Salida_Datos (void *pvParameters)
{
	static float Salida_Datos;

	while (1)
	{
		xQueueReceive (Dato_Salida, &Salida_Datos, portMAX_DELAY);
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

	Dato_Salida = xQueueCreate( 1, sizeof (float) );

	xSemaphoreTake (Semphr_ADC , portMAX_DELAY);

	/* Procesamiento de Datos */
	xTaskCreate(Procesa_Datos, "Procesamiento", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Salida de Datos */
	xTaskCreate(Salida_Datos, "Output", configMINIMAL_STACK_SIZE,
			NULL , (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
