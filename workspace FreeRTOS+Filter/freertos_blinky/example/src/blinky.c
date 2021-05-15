/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

#define PORT(n)		((uint8_t) n)
#define PIN(n)		((uint8_t) n)
#define GRUPO(n)	((uint8_t) n)






/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */


void Configuracion_IO (void)
	{
		SystemCoreClockUpdate();

		Chip_GPIO_Init(LPC_GPIO_PORT);

				Chip_SCU_PinMux( GRUPO(1) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		        Chip_SCU_PinMux( GRUPO(1) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		    	Chip_SCU_PinMux( GRUPO(1) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		     	Chip_SCU_PinMux( GRUPO(1) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		     	Chip_SCU_PinMux( GRUPO(1) , PIN(6) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );

		        Chip_SCU_PinMux( GRUPO(2) , PIN(10) ,SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		     	Chip_SCU_PinMux( GRUPO(2) , PIN(11) ,SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		     	Chip_SCU_PinMux( GRUPO(2) , PIN(12) ,SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );
		      	Chip_SCU_PinMux( GRUPO(2) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );
		        Chip_SCU_PinMux( GRUPO(2) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );
		      	Chip_SCU_PinMux( GRUPO(2) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );

		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(11));//Led AMARILLO - led 2
		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(12));//Led VERDE - led 3
		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(0) , PIN(14));//Led ROJO - led 1

		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(0));//LED0R
		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(1));//LED0G
		    	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(2));//LED0B

		        Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, PORT(0), PIN(14));
		        Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, PORT(1), PIN(11));

		        Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1), PIN(11), (bool) false);
		        Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(0), PIN(14), (bool) false);
		}










	//Chip_SCU_PinMux( GRUPO(1) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );// Tecla 1
	//Chip_SCU_PinMux( GRUPO(1) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );// Tecla 2
	//Chip_SCU_PinMux( GRUPO(1) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );// Tecla 3
	//Chip_SCU_PinMux( GRUPO(1) , PIN(6) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 );// Tecla 4

	//Chip_SCU_PinMux( GRUPO(2) , PIN(10) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 ); //Led 1
	//Chip_SCU_PinMux( GRUPO(2) , PIN(11) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 ); //Led 2
	//Chip_SCU_PinMux( GRUPO(2) , PIN(12) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC0 ); //Led 3

	//Chip_SCU_PinMux( GRUPO(2) , PIN(0) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );// Led0_R
	//Chip_SCU_PinMux( GRUPO(2) , PIN(1) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );// Led0_G
	//Chip_SCU_PinMux( GRUPO(2) , PIN(2) , SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS , SCU_MODE_FUNC4 );// Led0_B

	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(4));//Tecla 1
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(8));//Tecla 2
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(0) , PIN(9));//Tecla 3
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , PORT(1) , PIN(9));//Tecla 4

	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(11));//Led AMARILLO - led 2
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(1) , PIN(12));//Led VERDE - led 3
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(0) , PIN(14));//Led ROJO - led 1

	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(0));//LED0R
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(1));//LED0G
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT , PORT(5) , PIN(2));//LED0B

	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(11) , (bool) false);
	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(1) , PIN(12) , (bool) false);
	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(0) , PIN(14) , (bool) false);

	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(5) , PIN(0) , (bool) false);   // estado 0 LED0R
	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(5) , PIN(1) , (bool) false);  //estado 0 LED0G
	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, PORT(5) , PIN(2) , (bool) false);	//estado 0 LED0G. Lo apago para que no interfiera



		/* LED1 toggle thread */
		static void vLEDTask1(void *pvParameters) {

			while (1)
			{
				Chip_GPIO_SetPinToggle (LPC_GPIO_PORT, PORT(0) , PIN(14) );

				/* Delay de 500mseg */
				vTaskDelay(500 / portTICK_RATE_MS);
			}
		}

		/* LED2 toggle thread */
		static void vLEDTask2(void *pvParameters) {


			while (1)
			{
				Chip_GPIO_SetPinToggle (LPC_GPIO_PORT, PORT(1) , PIN(11) );

				/* Delay de 500mseg */
				vTaskDelay(700 /portTICK_RATE_MS);
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
	Configuracion_IO ();


	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, "vTaskLed1", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, "vTaskLed2", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
