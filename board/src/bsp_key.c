#include "bsp_key.h"
#include "delay.h"

static GPIO_TypeDef* KEY_PORT[KEY_MAX]={GPIOB,     GPIOB,      GPIOB,     GPIOC,     GPIOB};
static const u16 KEY_PIN[KEY_MAX]={GPIO_Pin_2,GPIO_Pin_10,GPIO_Pin_1,GPIO_Pin_5,GPIO_Pin_11};
                                  /////////À≥–Ú  …œœ¬◊Û”“÷–
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  u8 i;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
	for(i = 0;i < KEY_MAX; i++)
	{
  	GPIO_InitStructure.GPIO_Pin = KEY_PIN[i]; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       //100M
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(KEY_PORT[i], &GPIO_InitStructure);
	}
}

u8 Get_Key(Key_Def key)
{
  	if ((KEY_PORT[key]->IDR&KEY_PIN[key])!=0)return 1;
  	else return 0;
}



