/*
 * @brief Ejemplo con colas y 3 tareas
 * La Tarea 1 lee el pulsador TEC1 (Up)
 * La Tarea 2 lee el pulsador TEC_2 (Down)
 * La Tarea 3 pasa la cantidad de cuentas a la Tarea 4.
 * La Tarea 4 realiza el parpadeo del led
 *
 * Se utilizan colas entre tarea 1 y 2, y entre tarea 3 y 4
 * Para pasarse los datos de las cuentas
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 *Este ejemplo usa la libreria queue.h
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#define PORT(n)		((uint8_t) n)
#define PIN(n)		((uint8_t) n)
#define GRUPO(n)	((uint8_t) n)

#define PORT_TEC1	0
#define PORT_TEC2	0
#define PORT_TEC3	0
#define PORT_LED3	1

#define PIN_TEC1	4
#define PIN_TEC2	8
#define PIN_TEC3	9
#define PIN_LED3	12

#define orden_filtro 8

QueueHandle_t Contador;
QueueHandle_t Contador_Up;
QueueHandle_t Contador_Down;

SemaphoreHandle_t Semaforo_T3T1;
SemaphoreHandle_t Semaforo_T3T2;
SemaphoreHandle_t Semaforo_T3Muestra;


/*****************************************************************************
 * Private functions
 ****************************************************************************/

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



/* Task Media Movil */
static void Media_Movil (void *pvParameters) {

	static unsigned int sample[orden_filtro] = 0;
	static unsigned int output,ultimo_sample;

	while (1)
	{
		for (int j = 0; j < orden_filtro -1; k ++)
		{
			sample[j] = sample[j+1];
		}

		sample[orden_filtro-1] = ultimo_sample;

		output = 0;

		for( int k = 0; k < orden_filtro; k ++)
		{
			output += sample[k]*coef[k];
		}
}


/* Task1 Tarea_On thread */
static void Tarea_Up (void *pvParameters) {

	static unsigned int Cuenta = 0;
	static unsigned int Estado_Anterior;

	while (1)
	{
		xSemaphoreTake(Semaforo_T3T1, portMAX_DELAY);

		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC1, PIN_TEC1) == 0 && Estado_Anterior != 0)
		{
			Estado_Anterior = 0;
			Cuenta ++;
		}

		else
		{
			if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC1, PIN_TEC1) == 1)
				Estado_Anterior = 1;
		}

		xQueueSend(Contador_Up, &Cuenta, portMAX_DELAY);
		vTaskDelay( 20 / portTICK_RATE_MS );
		xQueueReceive (Contador_Down,  &Cuenta, portMAX_DELAY );
	}
}

/* Task2 toggle thread */
static void Tarea_Down (void *pvParameters) {

	static unsigned int Cuenta = 0;
	static unsigned int Estado_Anterior;

	while (1)
	{
		xSemaphoreTake(Semaforo_T3T2, portMAX_DELAY);

		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC2, PIN_TEC2) == 0 && Estado_Anterior != 0)
		{
			Estado_Anterior = 0;

			if ( Cuenta > 0)
				Cuenta --;
		}

		else
		{
			if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC2, PIN_TEC2) == 1)
				Estado_Anterior = 1;
		}

		xQueueSend (Contador_Down, &Cuenta, portMAX_DELAY);

		xQueueReceive (Contador,  &Cuenta, 0);
		xQueueSend (Contador, &Cuenta, 0);

		vTaskDelay( 20 / portTICK_RATE_MS );
		xQueueReceive (Contador_Up,  &Cuenta, portMAX_DELAY );

	}
}

/* Task3  se ejecuta 1ms c/ 4ms*/
static void Tarea_T3 (void *pvParameters) {

	while (1)
	{
		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC3, PIN_TEC3) == 1)
		{
			xSemaphoreGive(Semaforo_T3T1);
			xSemaphoreGive(Semaforo_T3T2);
		}

		else
		{
			xSemaphoreGive(Semaforo_T3Muestra); //En este punto se ejecuta y se bloquea porque ejecuta otra tarea
			vTaskDelay( 8000 / portTICK_RATE_MS );//Por eso no hace falta usar estado_anterior como TEC1/2
		}
	}
}

/* Task4 toggle thread */
static void Tarea_Muestreo (void *pvParameters) {

	static unsigned int Parpadeos;
	static unsigned int i;

	while (1)
	{
		xSemaphoreTake(Semaforo_T3Muestra, portMAX_DELAY);

		xQueueReceive (Contador_Up,  &Parpadeos, portMAX_DELAY );

		for (i = 2*Parpadeos; i; i--)
		{
			Chip_GPIO_SetPinToggle (LPC_GPIO_PORT, PORT(PORT_LED3), PIN(PIN_LED3));
			vTaskDelay( 300 / portTICK_RATE_MS );
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void) {
	prvSetupHardware();

	vSemaphoreCreateBinary(Semaforo_T3T1);
	vSemaphoreCreateBinary(Semaforo_T3T2);
	vSemaphoreCreateBinary(Semaforo_T3Muestra);

	Contador = xQueueCreate ( 1, sizeof (unsigned int) );
	Contador_Up = xQueueCreate ( 1, sizeof (unsigned int) );
	Contador_Down = xQueueCreate ( 1, sizeof (unsigned int) );

	xSemaphoreTake(Semaforo_T3T1, portMAX_DELAY); // inicializo semaforos
	xSemaphoreTake(Semaforo_T3T2, portMAX_DELAY);
	xSemaphoreTake(Semaforo_T3Muestra, portMAX_DELAY);

	/* Task1 toggle thread */
	xTaskCreate(Tarea_Up, "Tarea_Up", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Task2 toggle thread */
	xTaskCreate(Tarea_Down, "Tarea_Down", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Task3 toggle thread */
	xTaskCreate(Tarea_T3, "Tarea_T3", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Task4 toggle thread */
	xTaskCreate(Tarea_Muestreo, "Tarea_Muestreo", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	vTaskStartScheduler();

	return 1;
}
