/*
 * @brief
 * Ejemplo 2, mismo que el anterior de colas/semaforos
 *
 * La Tarea 1 lee el pulsador TEC1 (Up)
 * La Tarea 2 lee el pulsador TEC_2 (Down)
 * La Tarea 3 pasa la cantidad de cuentas a la Tarea 4.
 * La Tarea 4 realiza el parpadeo del led
 *
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
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

QueueHandle_t Contador_Up;
QueueHandle_t Contador_Down;

SemaphoreHandle_t Semaforo_T3T1;
SemaphoreHandle_t Semaforo_T3T2;
SemaphoreHandle_t Semaforo_T3Muestra;
SemaphoreHandle_t Semaforo_TMuestraT3;

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


/* Task1 Tarea_On thread */
static void Tarea_Up (void *pvParameters) {

	static unsigned int Cuenta = 0;
	static unsigned int Buffer;
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

		vTaskDelay( 20 / portTICK_RATE_MS );

		xQueueReceive(Contador_Up, &Buffer, 0); //Vaciara xQueueSend antes de escribir!
		xQueueSend(Contador_Up, &Cuenta, portMAX_DELAY);
	}
}

/* Task2 toggle thread */
static void Tarea_Down (void *pvParameters) {

	static unsigned int Cuenta = 0;
	static unsigned int Buffer;
	static unsigned int Estado_Anterior;

	while (1)
	{
		xSemaphoreTake(Semaforo_T3T2, portMAX_DELAY);

		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC2, PIN_TEC2) == 0 && Estado_Anterior != 0)
		{
			Estado_Anterior = 0;
			Cuenta ++;
		}

		else
		{
			if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, PORT_TEC2, PIN_TEC2) == 1)
				Estado_Anterior = 1;
		}

		vTaskDelay( 20 / portTICK_RATE_MS );

		xQueueReceive(Contador_Down, &Buffer, 0);
		xQueueSend(Contador_Down, &Cuenta, portMAX_DELAY);

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
			xSemaphoreGive(Semaforo_T3Muestra); //PERMITE A LA TAREA MUESTREO EJEC (1)
			xSemaphoreTake(Semaforo_TMuestraT3, portMAX_DELAY); //BLOQUEA EL SISTEMA HASTA QUE SE EJEC PARPADEO (2)
		}

		vTaskDelay( 20 / portTICK_RATE_MS );
	}
}

/* Task4 toggle thread */
static void Tarea_Muestreo (void *pvParameters) {

	static unsigned int Parpadeos;
	static unsigned int Cuenta_Up;
	static unsigned int Cuenta_Down;
	static unsigned int i;

	while (1)
	{
		xSemaphoreTake(Semaforo_T3Muestra, portMAX_DELAY); //(PAR 1)

		xQueueReceive (Contador_Up,  &Cuenta_Up, portMAX_DELAY );
		xQueueReceive (Contador_Down,  &Cuenta_Down, portMAX_DELAY );

		if (Cuenta_Up > Cuenta_Down) //unsigned solo admite comp
		{
			Parpadeos = Cuenta_Up - Cuenta_Down;
		}

		else
			Parpadeos = 0;

		for (i = 2*Parpadeos; i; i--)
		{
			Chip_GPIO_SetPinToggle (LPC_GPIO_PORT, PORT(PORT_LED3), PIN(PIN_LED3));
			vTaskDelay( 300 / portTICK_RATE_MS );
		}

		xSemaphoreGive(Semaforo_TMuestraT3); //(PAR 2)
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
	vSemaphoreCreateBinary(Semaforo_TMuestraT3);

	Contador_Up = xQueueCreate ( 1, sizeof (unsigned int) );
	Contador_Down = xQueueCreate ( 1, sizeof (unsigned int) );

	xSemaphoreTake(Semaforo_T3T1, portMAX_DELAY); // inicializo semaforos
	xSemaphoreTake(Semaforo_T3T2, portMAX_DELAY);
	xSemaphoreTake(Semaforo_T3Muestra, portMAX_DELAY);
	xSemaphoreTake(Semaforo_TMuestraT3, portMAX_DELAY);

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
