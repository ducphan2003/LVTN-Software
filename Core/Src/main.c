/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_16x2.h"
#include "KEYPAD.h"
#include "stdio.h"
char Key;
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
char Rx_data[10];
char Tx_data[20];

KEYPAD_Name KeyPad;
char KEYMAP[NUMROWS][NUMCOLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'X','0','E','D'}
};
char keyStatus = 0;    // Status = 0 --> write code || Status = 1 --> write password || Status = 2 --> write done
char loginStatus = 0;  // Status = 0 --> not login yet || Status = 1 --> send code-password || Status = 2 --> logged in || Status = 3 --> logged out
char state = 0;
char code[20];
char password[20];

volatile uint16_t ADC_val[3];
float Vmax, Imax, Temp;
uint8_t Vrms, Irms;
int32_t timeV, timeI;
int8_t cosphi;
uint16_t count20ms;
uint16_t count;
int8_t 	cosin[]={	100, 100, 100, 100, 100, 100, 99,	99,	99,	99,	98,	98,	98,	97,	97,	97,	96,	96,	95,	95,	94,	93,	93,	92,	91,	91,	90,	89,	88,	87,	87,	86,	85,	84,	83,	82,	81,	80,	79,	78,	77,	75,	74,	73,	72,
									71,	69,	68,	67,	66, 64,	63,	62,	60,	59,	57,	56,	54,	53,	52,	50,	48,	47,	45,	44,	42,	41,	39,	37,	36,	34,	33,	31,	29,	28,	26,	24,	23,	21,	19, 17, 16, 14, 12, 10, 9, 7, 5, 3, 2,
									0, -2, -3, -5, -7, -9, -10, -12, -14, -16, -17, -19, -21, -23, -24, -26, -28, -29, -31, -33, -34, -36, -37, -39, -41, -42, -44, -45, -47, -48, -50,	-52, -53,	-54, -56,	-57, -59,	-60, -62,	-63, -64, -66, -67, -68, -69,
									-71, -72, -73, -74, -75, -77, -78, -79, -80, -81, -82, -83, -84, -85, -86, -87, -87, -88, -89, -90, -91, -91, -92, -93, -93, -94, -95, -95, -96, -96, -97, -97, -97, -98, -98, -98, -99, -99, -99, -99, -100, -100, -100, -100, -100,
									-100, -100, -100, -100, -100, -100, -99, -99, -99, -99, -98, -98, -98, -97, -97, -97, -96, -96, -95, -95, -94, -93, -93, -92, -91, -91, -90, -89, -88, -87, -87, -86, -85, -84, -83, -82, -81, -80, -79, -78, -77, -75, -74, -73, -72,
									-71, -69, -68, -67, -66, -64, -63, -62, -60, -59, -57, -56, -54, -53, -52, -50, -48, -47, -45, -44, -42, -41, -39, -37, -36, -34, -33, -31, -29, -28, -26, -24, -23, -21, -19, -17, -16, -14, -12, -10, -9, -7, -5, -3, -2,
									0, 2, 3, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 34, 36, 37, 39, 41, 42, 44, 45, 47, 48, 50, 52, 53, 54, 56, 57, 59, 60, 62, 63, 64, 66, 67, 68, 69,
									71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 87, 88, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98 ,98, 99, 99, 99, 99, 100, 100, 100, 100, 100};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*--------------------------------------------------------- Calculator cos phi ---------------------------------------------------------*/
void Calcosphi(){
	float dem =(timeI - timeV)*0.72;
	if(dem<0){
		dem = dem - (2*dem);
	}
	if(dem>359) {dem = 359;}
	cosphi = cosin[(int)dem];
}
/*--------------------------------------------------------- Print to LCD ---------------------------------------------------------*/
void Lcd_test(){
	Lcd_clear_display();
	Lcd_gotoxy(0,0);
	Lcd_write_string("T=");
	Lcd_write_int((int)Temp);
	Lcd_write_string("doC");
	
	Lcd_gotoxy(9,0);
	Lcd_write_string("PF=");
	if(cosphi == 100) Lcd_write_string("1.0");
	else if(cosphi==-100) Lcd_write_string("-1.0");
	else if(cosphi>=0){
		Lcd_write_string("0.");
		Lcd_write_int(cosphi);
	}
	else if(cosphi<0){
		Lcd_write_string("-0.");
		int8_t td = cosphi - (2*cosphi);
		Lcd_write_int(td);
	}
	
	Lcd_gotoxy(0,1);
	Lcd_write_string("V=");
	Lcd_write_int(Vrms);
	Lcd_write_string("V");
	
	Lcd_gotoxy(8,1);
	Lcd_write_string("I=");
	Lcd_write_int(Irms);
	Lcd_write_string("A");
}
/*--------------------------------------------------------- Read KeyPad ---------------------------------------------------------*/
void readKeypad() {
	Key = KEYPAD4X4_Readkey(&KeyPad);
	if (Key) {
		if ((Key == 'X')&&(keyStatus == 0)) {
			code[strlen(code)-1] = '\0';
		}
		else if ((Key == 'X')&&(keyStatus == 1)) {
			password[strlen(password)-1] = '\0';
		}
		else if ((Key == 'E')&&(keyStatus == 0)) {
			keyStatus = 1;
		}
		else if ((Key == 'E')&&(keyStatus == 1)) {
			keyStatus = 2;
		}
		else if (keyStatus == 0) {
			code[strlen(code)] = Key;
		}
		else if (keyStatus == 1) {
			password[strlen(password)] = Key;
		}
		if(strlen(code) != 0){
			Lcd_clear_display();
			Lcd_gotoxy(0,0);
			Lcd_write_string(code);
			Lcd_gotoxy(0,1);
			Lcd_write_string(password);
		}
	}
	
	HAL_Delay(30);
}
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
	Lcd_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_val, 3);
	HAL_UART_Receive_IT(&huart3, (uint8_t*)Rx_data, 1);
	KEYPAD4X4_Init(&KeyPad, KEYMAP, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_3, GPIOA, GPIO_PIN_15, GPIOB, GPIO_PIN_9, GPIOB, GPIO_PIN_8, GPIOB, GPIO_PIN_7, GPIOB, GPIO_PIN_6);
	
	Lcd_clear_display();
	Lcd_gotoxy(0,0);
	Lcd_write_string("Nhap code");
	Lcd_gotoxy(0,1);
	Lcd_write_string("& password");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(keyStatus != 2) {
			readKeypad();
		}
		if((keyStatus == 2)&&(loginStatus == 0)) {
			strcpy(Tx_data, code);
			Tx_data[strlen(Tx_data)] = '&';
			strcat(Tx_data, password);
			HAL_UART_Transmit(&huart3, (uint8_t*)Tx_data, strlen(Tx_data), 200);
			loginStatus = 1; //--------------------------
		}
		if(loginStatus ==2) {
			if(state == 1) {
				Vrms = (((Vmax*3.3/4095)+1.2)*5.6+1.1)*12.9636;
	//			Irms = Imax*37.9886; //Imax*3.3/4095/6*10/5*200/can(2) (mA)
				Irms = ((Imax*3.3/4095)-1.6)*64.5; //100A --> 0.05 * 22 + 1.65 = Imax.3.3/4095 (mA)
				Calcosphi();
				Temp = ((ADC_val[2]*113/4095)-2)/0.387; //Vo=(Rt*3.3*10)/2*(570)			V=(ADC_val*3.3)/4095		Rt=T*0.387
				Vmax = 0;
				Imax = 0;
				timeI = 0;
				timeV = 0;
				
				char buffTemp[10];
				sprintf(buffTemp, "%0.1f", Temp);
				char buffVol[10];
				sprintf(buffVol, "&%d", Vrms);
				char buffCur[10];
				sprintf(buffCur, "&%d", Irms);
				char buffPf[10];
				sprintf(buffPf, "&%d", cosphi);

				char Tx[40];
				strcpy(Tx, buffTemp);
				strcat(Tx, buffVol);
				strcat(Tx, buffCur);
				strcat(Tx, buffPf);
				HAL_UART_Transmit(&huart3, (uint8_t*)Tx, strlen(Tx), 200);
				
				Lcd_test();
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
				HAL_Delay(1000);
				state = 0;
			}
			else if(state ==0){
				count20ms = 0;
				__HAL_TIM_SET_COUNTER(&htim2, 0);
					HAL_TIM_Base_Start_IT(&htim2);
				state = 2;
			}
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM2){
/*---------------------------------------------------------  Timer 20us ---------------------------------------------------------*/
		count20ms++;
		if(Vmax <= ADC_val[0]){
			Vmax = ADC_val[0];
			timeV = count20ms;
		}
		if(Imax <= ADC_val[1]){
			Imax = ADC_val[1];
			timeI = count20ms;
		}
		if(count20ms > 499){
			state = 1;
		}
		else {
			__HAL_TIM_SET_COUNTER(&htim2, 0);
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
 if(huart->Instance == huart3.Instance)
 {
	 if((Rx_data[0] == 76)&&(loginStatus == 1)){
		 loginStatus = 2;
	 }
	 if((Rx_data[0] == 79)&&(loginStatus == 2)){
		 loginStatus = 3;
	 }
	 Rx_data[strlen(Rx_data)-1] = '\0';
   HAL_UART_Receive_IT(&huart3, (uint8_t*)Rx_data, 1);
 }
}
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

#ifdef  USE_FULL_ASSERT
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
