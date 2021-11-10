
#include "main.h"

uint8_t x;               //Переменная куда будет записываться значение с порта С
uint8_t pin = 131;       //Секретный код 10000011
uint8_t a,i = 0;         //Переменные для счетчика (сколькораз светодиод должен включаться и выключаться)
uint32_t pin_time = 1;   //контроль всечения светодиода 1 - горит, 0 - не горит
uint16_t count1=2000-1;  //Константа для установки таймера на 1 секунду
uint16_t count2=10000-1; //Константа для установки таймера на 5 секунд

int main(void) //основная программа
{
	
	// Включаем тактирование порта А
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // сброс режимов порта
    GPIOA->CRL &= ~(GPIO_CRL_MODE | GPIO_CRL_CNF);
	// установка скорости порта 
    GPIOA->CRL |= GPIO_CRL_MODE;
	
	// Включаем тактирование порта C
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Сброс режимов порта C
	GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); 
	GPIOC->CRH |= GPIO_CRH_CNF13_0;
    
	GPIOC->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
	GPIOC->CRL |= GPIO_CRL_CNF0_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
	GPIOC->CRL |= GPIO_CRL_CNF1_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
	GPIOC->CRL |= GPIO_CRL_CNF2_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
	GPIOC->CRL |= GPIO_CRL_CNF3_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
	GPIOC->CRL |= GPIO_CRL_CNF4_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
	GPIOC->CRL |= GPIO_CRL_CNF5_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
	GPIOC->CRL |= GPIO_CRL_CNF6_0;
	GPIOC->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
	GPIOC->CRL |= GPIO_CRL_CNF7_0;
	
	init_IT_for_exemple();
    init_timer();
    	
  while (1)
  {
  }
}

void init_IT_for_exemple(void) //настройка прерывания 
{
            RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_AFIOEN;    //Тактирование AFIO
            AFIO->EXTICR[3] = 0x0020;                            //Разрешаем прерывание 13 пина порта С
            EXTI->PR = EXTI_PR_PR13;	                         //Сбрасываем флаг прерывания
            EXTI->RTSR|=(EXTI_RTSR_TR13);                        //Прерывание по спаду импульса
            EXTI->IMR|=(EXTI_IMR_MR13);                          //Включаем прерывание 
            NVIC_EnableIRQ(EXTI15_10_IRQn);                      //Разрешаем прерывание в контроллере прерываний
}
void init_timer() //настройка таймера
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Разрешение прохождение сигнала Timer4

	TIM4->PSC = (SystemCoreClock/2) / 1000 - 1; // ((8МГц/2)/1000-1)=3999 предделитель. 
    //Получаем 2000 тиков в секунду, т.е.  один тик = 0.5 мс
    TIM4->ARR = count1;                           // Установка таймера на 1 секунду, 2000*0.5мс = 1 сек, т.е. прерывание будет выполняться раз в секунду 
	TIM4->DIER |= TIM_DIER_UIE;                   // разрешаем прерывание по переполнению
	NVIC_EnableIRQ(TIM4_IRQn);                    // глобально разрешаем прерывание
    TIM4->CR1 |= TIM_CR1_CEN;                     // запускаем таймер
}
void led(void) //Функция ВКЛ. и ВЫКЛ. светодиода
{
            while(i<a*2)                   //пока i меньше а*2 цикл выполняется 
                { if(pin_time == 1)        // если pin_time равен 1, то тело цикла выполняется 
                    {GPIOA->BSRR = 0xff;   //установка 1 на пины от 0 до 7 (11111111 = 0xff)
                     pin_time = 0;         //устанавливаем pin_time = 0, это нужно для того чтоб при следующем прохождении тело цикла if не выполнялось    
                     i++;                  // i = i + 1
                     break;}               //выход из цикла 
                else                       // если pin_time не равен 1
                    {GPIOA->BRR = 0xff;    // установка 0 на пины от 0 до 7 
                     pin_time = 1;         //устанавливаем pin_time = 1
                     i++;                  // i = i + 1
                     break;}               //выход из цикла
                 }           
}
void time(uint16_t count) //Функция установки таймера 
{
		    TIM4->ARR = count;
 
}
// обработчик прерывания
void EXTI15_10_IRQHandler (void)
{
            EXTI->PR |= GPIO_PIN_13;      //Сбрасываем флаг прерывания
            x = GPIOC->IDR &(GPIO_IDR_IDR0 | GPIO_IDR_IDR1 | GPIO_IDR_IDR2 | GPIO_IDR_IDR3 | GPIO_IDR_IDR4 | GPIO_IDR_IDR5 | GPIO_IDR_IDR6 | GPIO_IDR_IDR7) ; // считываем значиние с порта С (0-7 пинов)
            pin_time = 1;                 //устанавливаем pin_time = 1
            i = 0;                        //устанавливаем i = 0
            if (x==pin)                   //если введенное число равно секретному коду, тело цикла выполняется
                {		    
                    a = 1;                //значения "a" определяется сколько раз должен загореться светодтод  
		            led();                //вызываем функцию ВКЛ. и ВЫКЛ. светодиода
                    time(count2);         //вызываем функцию установки таймера. Функция принимает  count1=2000-1
                }
            else                  
                {   a = 3;                //устанавливаем a = 3
		            led();                //вызываем функцию ВКЛ. и ВЫКЛ. светодиода
                    time(count1);         //вызываем функцию установки таймера. Функция принимает  count2=10000-1
                }
            TIM4->CNT=0;                  // сбрасываем значение таймера
}
// обработчик прерывания таймера
void TIM4_IRQHandler(void) {
            TIM4->SR &= ~TIM_SR_UIF;      // сбрасываем прерывание
            led();                        //вызываем функцию ВКЛ. и ВЫКЛ. светодиода
}
