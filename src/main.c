/* Name: Mohammad Badr Eddin
* Fourth Lab Assignment(2nd chance)
* Implementation of a CAN node using STM32F407VG
* Software Atollic TrueSTUDIO for STM32 is used in this assignment
*/
/* Includes */
#include "stm32f4xx.h"

/* Private macro */
/* Private variables */
volatile uint32_t counter = 0;
/* Private function prototypes */
/* Private functions */
void Config_CAN(void);
void Config_GPIO_LED(void);
void Config_GPIO_PB(void);
void exti_conf(void);


int main(void)
{
	SystemInit();
	exti_conf();
	Config_GPIO_LED();
	Config_GPIO_PB();
	Config_CAN();

  /* Infinite loop */

  while (1)
  {
	__WFI(); // Wait For Interrupt
  }
}

void CAN1_RX0_IRQHandler(void){

	while(!CAN_GetITStatus(CAN1, CAN_IT_FF0)){}

	CanRxMsg RxMessage1;
	CanRxMsg RxMessage2;

// Received Message check and order:
// config received msg
	RxMessage1.DLC = 4;
	RxMessage1.StdId = 0x23;
	RxMessage1.FMI = 0;
	RxMessage1.IDE = CAN_Id_Standard;
	RxMessage1.FMI = 0;
	RxMessage1.RTR = CAN_RTR_Data;

	RxMessage2.DLC = 5;
	RxMessage2.StdId = 0x56;
	RxMessage2.FMI = 0;
	RxMessage2.IDE = CAN_Id_Standard;
	RxMessage2.FMI = 0;
	RxMessage2.RTR = CAN_RTR_Data;


	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage1);
	if(RxMessage1.StdId == 0x23){
		counter++;
	}

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage2);
// Transmit a message
//	uint32_t ptr*;
if(RxMessage2.StdId == 0x56){
	//ptr = (uint32_t*) &RxMessage2.
	CanTxMsg Message2;
			Message2.DLC = 2; // Set data length code (DLC)
			Message2.ExtId = 0; // Set extended identifier (not used in this case)
			Message2.StdId = 0x32;// Set standard identifier
			Message2.IDE = CAN_Id_Standard; // Set identifier type as standard
			Message2.RTR = CAN_RTR_Data; // Set data frame type

			//Message2.Data[0]= RxMessage2.Data[0];
			//Message2.Data[1] = RxMessage2.Data[4];

			for(uint8_t i = 0 ; i < 5 -1 ; i++ ){

				for(uint8_t j = i + 1; j < 5 ; j++){

					if(RxMessage2.Data[i] > RxMessage2.Data[j]){

						uint8_t empty = RxMessage2.Data[i];
						RxMessage2.Data[i] = RxMessage2.Data[j];
						RxMessage2.Data[j] = empty;
					}
				}
				Message2.Data[0]= RxMessage2.Data[0];
				Message2.Data[1] = RxMessage2.Data[4];

			}
		 CAN_Transmit(CAN1,&Message2);
		 GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
}
		CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
}
void EXTI0_IRQHandler(void){

	CanTxMsg Massage;
		Massage.DLC = 4; // Set data length code (DLC)
		Massage.ExtId = 0; // Set extended identifier (not used in this case)
		Massage.StdId = 0x31;// Set standard identifier
		Massage.IDE = CAN_Id_Standard; // Set identifier type as standard
		Massage.RTR = CAN_RTR_Data; // Set data frame type
		uint32_t *pointer;
		pointer = (uint32_t*) &Massage.Data;
		*pointer = counter;

	CAN_Transmit(CAN1,&Massage);
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
	EXTI_ClearITPendingBit(EXTI_Line0);

}

void exti_conf(void){

	EXTI_InitTypeDef exti;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0 );
	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti);

	NVIC_EnableIRQ(EXTI0_IRQn);

}
void Config_GPIO_PB(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef LED12;

	LED12.GPIO_Pin = GPIO_Pin_0;
	LED12.GPIO_OType = GPIO_OType_PP;
	LED12.GPIO_Mode = GPIO_Mode_IN;
	LED12.GPIO_PuPd = GPIO_PuPd_NOPULL;
	LED12.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &LED12);

}

void Config_GPIO_LED(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef LED12;

	LED12.GPIO_Pin = GPIO_Pin_12;
	LED12.GPIO_OType = GPIO_OType_PP;
	LED12.GPIO_Mode = GPIO_Mode_OUT;
	LED12.GPIO_PuPd = GPIO_PuPd_NOPULL;
	LED12.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOD, &LED12);

}
void Config_CAN(void){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//Enable the clock for the CAN GPIOs
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //Enable the GPIO AHB clock of GPIOA

	// Configure the GPIO Pins for CAN
	GPIO_InitTypeDef PinD;

	PinD.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 ;
	PinD.GPIO_Mode = GPIO_Mode_AF;
	PinD.GPIO_OType = GPIO_OType_PP;
	PinD.GPIO_PuPd = GPIO_PuPd_NOPULL;
	PinD.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOD,&PinD);

	// Connect the GPIO Pins with CAN1 module
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1); //alternate function to CAN1-RX
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1); //alternate function to CAN1-TX

	// CAN Init Configuration
	CAN_InitTypeDef CANConfig;
	CANConfig.CAN_TTCM = DISABLE;
	CANConfig.CAN_TXFP = DISABLE;
	CANConfig.CAN_ABOM = DISABLE;
	CANConfig.CAN_AWUM = DISABLE;
	CANConfig.CAN_BS1 = CAN_BS1_11tq;
	CANConfig.CAN_BS2 = CAN_BS2_2tq;
	CANConfig.CAN_Mode = CAN_Mode_Normal;
	CANConfig.CAN_NART = DISABLE;
	CANConfig.CAN_RFLM = DISABLE; // http://www.bittiming.can-wiki.info/
	CANConfig.CAN_Prescaler = 6; // Set baud rate to 500 Kbd with a 72 MHz system clock
	CANConfig.CAN_SJW = CAN_SJW_1tq;
	CAN_Init(CAN1,&CANConfig);

	// CAN Filter Configuration// CAN Filter Configuration
	CAN_FilterInitTypeDef CANFilter;
	CANFilter.CAN_FilterActivation = ENABLE;
	CANFilter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;

	// These fields represent the identifier of the filter. By setting both to 0x0000, the filter will match any CAN message ID.
	CANFilter.CAN_FilterIdHigh = 0x0000;
	CANFilter.CAN_FilterIdLow = 0x0000;
	// these fields define the mask for the filter and the meaning that
	// all bits of the received message ID will be compared with the corresponding bits in the filter.
	CANFilter.CAN_FilterMaskIdHigh = 0x0000;
	CANFilter. CAN_FilterMaskIdLow = 0x0000;

	CANFilter.CAN_FilterMode = CAN_FilterMode_IdMask;
	CANFilter.CAN_FilterNumber = 0;
	CANFilter.CAN_FilterScale=CAN_FilterScale_16bit;

	CAN_FilterInit(&CANFilter);

	// Enable the CAN RX0 interrupts
	 NVIC_EnableIRQ(CAN1_RX0_IRQn);
	 // Enable CAN1 FIFO 0 interrupt
	 CAN_ITConfig(CAN1, CAN_IT_FF0, ENABLE);

}
