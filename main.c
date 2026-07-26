/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//POINTERS RELATED TO ADC1
volatile uint32_t* RCC_APB2ENR=(volatile uint32_t*)0X40023844;
volatile uint32_t* ADC1_SR=(volatile uint32_t*)0X40012000;
volatile uint32_t* ADC1_CR1=(volatile uint32_t*)0X40012004;
volatile uint32_t* ADC1_CR2=(volatile uint32_t*)0X40012008;
volatile uint32_t* ADC1_SMPR2=(volatile uint32_t*)0X40012010;
volatile uint32_t* ADC1_SQR3=(volatile uint32_t*)0X40012034;
volatile uint32_t* ADC1_DR=(volatile uint32_t*)0X4001204C;
//POINTERS RELATED TO GPIOA
volatile uint32_t* RCC_AHB1ENR=(volatile uint32_t*)0X40023830;
volatile uint32_t* GPIOA_MODER=(volatile uint32_t*)0X40020000;
volatile uint32_t* GPIOA_AFRL=(volatile uint32_t*)0X40020020;
//POINTERS FOR USART2
volatile uint32_t* RCC_APB1ENR=(volatile uint32_t*)0X40023840;
volatile uint32_t* USART2_SR=(volatile uint32_t*)0X40004400;
volatile uint32_t* USART2_DR=(volatile uint32_t*)0X40004404;
volatile uint32_t* USART2_BRR=(volatile uint32_t*)0X40004408;
volatile uint32_t* USART2_CR1=(volatile uint32_t*)0X4000440C;
volatile uint32_t* USART2_CR2=(volatile uint32_t*)0X40004410;
volatile uint32_t* NVIC_ISER1=(volatile uint32_t*)0XE000E104;
//BUFFER AND STATE FOR UART TRANSMISSION
volatile char uart_buffer[16];
volatile uint8_t tx_index = 0;
volatile uint8_t tx_length = 0;
volatile uint16_t adc_value = 0;
void format_voltage(uint16_t adc_val);
//POINTERS FOR TIM2
//************POINTERS FOR TIM2
volatile uint32_t* NVIC_ISER0=(volatile uint32_t*)0XE000E100;//IRQ NUMBER IS 28
volatile uint32_t* TIM2_CR1=(volatile uint32_t*)0X40000000;//TIM2 CONTROL REGISTER
volatile uint32_t* TIM2_DIER=(volatile uint32_t*)0X4000000C;//TIM2 DIER REGISTER
volatile uint32_t* TIM2_SR=(volatile uint32_t*)0X40000010;//TIM2 STATUS REGISTERS
volatile uint32_t* TIM2_PSC=(volatile uint32_t*)0X40000028;//PSC REGISTER
volatile uint32_t* TIM2_ARR=(volatile uint32_t*)0X4000002C;//ARR REGISTER

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  *RCC_APB2ENR|=(1U<<8);//ADC1 CLOCK ENABLED
  *RCC_AHB1ENR|=(1U<<0);//GPIOA CLOCK ENABLED
  *GPIOA_MODER&=~(3U<<0);//CLEARING BITS OF MODER FOR PA0
  *GPIOA_MODER|=(3U<<0);//PA0 SET TO ANALOG MODE
  *ADC1_CR1&=~(3U<<24);//CLEARING BITS 24 AND 25
  *ADC1_SMPR2 &= ~(7u << 0);   // channel 0 sample time = 56 cycles
  *ADC1_SMPR2|=(3U<<0);
  *ADC1_SQR3 &= ~(0x1F << 0);  // clear SQ1 field (bits 4:0)
  *ADC1_SQR3 |= (0 << 0);      // set channel 0
  *GPIOA_MODER&=~(3U<<4);
  *GPIOA_MODER&=~(3U<<6);
  *GPIOA_MODER|=(2U<<4);//PA2 SET TO AF MODE
  *GPIOA_MODER|=(2U<<6);//PA3 SET TO AF MODE
  *GPIOA_AFRL&=~(15U<<8);
  *GPIOA_AFRL&=~(15U<<12);
  *GPIOA_AFRL|=(7U<<8);//PA2 SET TO USART2 TX
  *GPIOA_AFRL|=(7U<<12);//PA3 SET TO USART2 RX
  *RCC_APB1ENR|=(1U<<17);//USART2 CLOCK ENABLED
  *USART2_BRR=0X0683;//BAUD RATE TO BE 9600 AT 16MHz
  *USART2_CR1|=(1U<<13);//UART ENABLED
  *USART2_CR1|=(1U<<2);//RX ENABLED
  *USART2_CR1|=(1U<<3);//TX ENABLED
  *RCC_APB1ENR|=(1U<<0);//TIM2 ENABLED
  *TIM2_PSC=15999;
  *TIM2_ARR=499;//500ms at 1kHz tick rate
  *TIM2_DIER|=(1U<<0);//UPDATE INTERRUPT ENABLED FOR TIM2
  *NVIC_ISER0|=(1U<<28);//PROCESSOR NOW MANAGES INTERRUPTS FROM TIM2
  *TIM2_CR1|=(1U<<0);//COUNTER ENABLED FOR TIM2 — enable last

  //usart2 priority number is 38. SO IT IS IN ISER1 AND 38-32=6
  *NVIC_ISER1|=(1U<<6);//TELLING PROCESSOR TO MANAGE INTERRUPTS FROM USART2


  *ADC1_CR2|=(1U<<0);//ADC enabled
  for (volatile int i = 0; i < 100; i++);







  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void TIM2_IRQHandler(void){
	if((*TIM2_SR>>0)&0X01){
		*TIM2_SR&=~(1U<<0);//CLEAR TIMER INTERRUPT FLAG
		*ADC1_CR2 |= (1U<<30);              // start new conversion
		while(!((*ADC1_SR>>1)&0x01));       // wait for EOC
        adc_value = (uint16_t)(*ADC1_DR & 0x0FFF);  // read result (also clears EOC)
        format_voltage(adc_value);          // build the string, reset tx_index/tx_length
		*USART2_CR1|=(1U<<7);//KICKSTARTING TRANSMISSION
	}
}
void USART2_IRQHandler(void){
	if(((*USART2_SR>>7)&0X01)){
		if(tx_index < tx_length){
			*USART2_DR = uart_buffer[tx_index++];
		}
		else {
			*USART2_CR1 &= ~(1U<<7);        // done sending, disable TXE interrupt
		}
	}
}
void format_voltage(uint16_t adc_val)
{
    uint32_t mv = (adc_val * 3300UL) / 4095UL;  // millivolts, 0-3300
    uint32_t volts_int = mv / 1000;
    uint32_t frac = mv % 1000;

    uint8_t i = 0;
    uart_buffer[i++] = '0' + volts_int;
    uart_buffer[i++] = '.';
    uart_buffer[i++] = '0' + (frac / 100);
    uart_buffer[i++] = '0' + ((frac / 10) % 10);
    uart_buffer[i++] = '0' + (frac % 10);
    uart_buffer[i++] = ' ';
    uart_buffer[i++] = 'V';
    uart_buffer[i++] = '\r';
    uart_buffer[i++] = '\n';

    tx_length = i;
    tx_index = 0;
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
