/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 *Este ejemplo usa la libreria semph.h
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#define PORT(n)		((uint8_t) n)
#define PIN(n)		((uint8_t) n)
#define GRUPO(n)	((uint8_t) n)

SemaphoreHandle_t Semaforo_S1;
SemaphoreHandle_t Semaforo_S2;
/*****************************************************************************
 * Private functions
 ****************************************************************************/
SemaphoreHandle_t S1;
SemaphoreHandle_t S2;
/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();

	Chip_GPIO_Init(LPC_GPIO_PORT);

	Chip_SCU_PinMux( GRUPO(1) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
	Chip_SCU_PinMux( GRUPO(1) , PIN(6) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );

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


/* LED1 Tarea_On thread */
static void Tarea_On (void *pvParameters) {

	while (1)
	{
		xSemaphoreTake(Semaforo_S1, portMAX_DELAY);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(11),TRUE );
		vTaskDelay(500 / portTICK_RATE_MS);
		xSemaphoreGive(Semaforo_S2);
	}
}

/* LED2 toggle thread */
static void Tarea_Off(void *pvParameters) {

	while (1)
	{
		xSemaphoreTake(Semaforo_S2, portMAX_DELAY);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(11),FALSE );
		vTaskDelay(500 / portTICK_RATE_MS);
		xSemaphoreGive(Semaforo_S1);
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

	vSemaphoreCreateBinary(Semaforo_S1);
	vSemaphoreCreateBinary(Semaforo_S2);

	xSemaphoreTake(Semaforo_S2, portMAX_DELAY); // S2 inicializo con estado Take

	/* LED1 toggle thread */
	xTaskCreate(Tarea_On, "Encendido", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(Tarea_Off, "Apagado", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
