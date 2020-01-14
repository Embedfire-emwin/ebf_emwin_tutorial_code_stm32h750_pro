/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   LCD应用函数接口，支持ARGB888/RGB888/RGB565/ARGB4444/ARGB1555 (不含中文显示)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "./lcd/bsp_lcd.h"

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y)) 

#define ABS(X)  ((X) > 0 ? (X) : -(X))

LTDC_HandleTypeDef  Ltdc_Handler;
DMA2D_HandleTypeDef Dma2d_Handler;

/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */

/* 不同液晶屏的参数 */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5寸屏参数 */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//液晶分辨率，宽
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//液晶分辨率，高
    
    .m_palette_btn_width = 90,//触摸画板按键的宽度
    .m_palette_btn_height = 50,//触摸画板按键的高度
  },
  
   /* 7寸屏参数（与5寸一样） */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//液晶分辨率，宽
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//液晶分辨率，高
    
    .m_palette_btn_width = 90,//触摸画板按键的宽度
    .m_palette_btn_height = 50,//触摸画板按键的高度    
    
  },

  /* 4.3寸屏参数 */
  {
      /*根据液晶数据手册的参数配置*/
    .hbp = 8,  //HSYNC后的无效像素
    .vbp = 2,  //VSYNC后的无效行数

    .hsw = 41,  	//HSYNC宽度
    .vsw = 10,   //VSYNC宽度

    .hfp = 4,  	//HSYNC前的无效像素
    .vfp = 4,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 15, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 15, //Argb8888等四字节像素时推荐使用的液晶时钟频率
    
    .lcd_pixel_width = 480,//液晶分辨率，宽
    .lcd_pixel_height = 272,//液晶分辨率，高
    
    .m_palette_btn_width = 50,//触摸画板按键的宽度
    .m_palette_btn_height = 30,//触摸画板按键的高度    
  }
};

LCD_TypeDef cur_lcd = INCH_5;

 /**
  * @brief  初始化控制LCD的IO
  * @param  无
  * @retval 无
  */
static void LCD_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 使能LCD使用到的引脚时钟 */
                          //红色数据线
  LTDC_R0_GPIO_CLK_ENABLE();LTDC_R1_GPIO_CLK_ENABLE();LTDC_R2_GPIO_CLK_ENABLE();\
  LTDC_R3_GPIO_CLK_ENABLE();LTDC_R4_GPIO_CLK_ENABLE();LTDC_R5_GPIO_CLK_ENABLE();\
  LTDC_R6_GPIO_CLK_ENABLE();LTDC_R7_GPIO_CLK_ENABLE();LTDC_G0_GPIO_CLK_ENABLE();\
  LTDC_G1_GPIO_CLK_ENABLE();LTDC_G2_GPIO_CLK_ENABLE();LTDC_G3_GPIO_CLK_ENABLE();\
  LTDC_G3_GPIO_CLK_ENABLE();LTDC_G5_GPIO_CLK_ENABLE();LTDC_G6_GPIO_CLK_ENABLE();\
  LTDC_G7_GPIO_CLK_ENABLE();LTDC_B0_GPIO_CLK_ENABLE();LTDC_B1_GPIO_CLK_ENABLE();\
  LTDC_B2_GPIO_CLK_ENABLE();LTDC_B3_GPIO_CLK_ENABLE();LTDC_B4_GPIO_CLK_ENABLE();\
  LTDC_B5_GPIO_CLK_ENABLE();LTDC_B6_GPIO_CLK_ENABLE();LTDC_B7_GPIO_CLK_ENABLE();\
  LTDC_CLK_GPIO_CLK_ENABLE();LTDC_HSYNC_GPIO_CLK_ENABLE();LTDC_VSYNC_GPIO_CLK_ENABLE();\
  LTDC_DE_GPIO_CLK_ENABLE();LTDC_BL_GPIO_CLK_ENABLE();
/* GPIO配置 */

 /* 红色数据线 */                        
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  
  GPIO_InitStruct.Pin =   LTDC_R0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R0_AF;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R1_AF;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R2_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R2_AF;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R3_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R3_AF;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R4_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R4_AF;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R5_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R5_AF;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R6_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R6_AF;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R7_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R7_AF;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
  
  //绿色数据线
  GPIO_InitStruct.Pin =   LTDC_G0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G0_AF;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G1_AF;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G2_AF;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G3_AF;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G4_AF;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G5_AF;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G6_AF;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G7_AF;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
  
  //蓝色数据线
  GPIO_InitStruct.Pin =   LTDC_B0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B0_AF;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B1_AF;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B2_AF;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B3_AF;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B4_AF;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B5_AF;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B6_AF;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B7_AF;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
  
  //控制信号线
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_CLK_AF;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_HSYNC_AF;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_VSYNC_AF;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_DE_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_DE_AF;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
  
  //背光BL                            
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  
  GPIO_InitStruct.Pin = LTDC_BL_GPIO_PIN; 
  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
  
}
/**
  * @brief  时钟配置
  * @retval 无
  */
void LCD_ClockConfig(void)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

	/* 液晶屏时钟配置 */
	/* 5寸屏的典型PCLK为27 MHz，因此PLL3R配置为提供此时钟 */ 
	/* AMPIRE640480 LCD clock configuration */
	/* PLL3_VCO Input = HSE_VALUE/PLL3M = 1 Mhz */
	/* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 270 Mhz */
	/* PLLLCDCLK = PLL3_VCO Output/PLL3R = 270/10 = 27Mhz */
	/* LTDC clock frequency = PLLLCDCLK = 27 Mhz */    
	periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	periph_clk_init_struct.PLL3.PLL3M = 25;    
	periph_clk_init_struct.PLL3.PLL3N = 270;
	periph_clk_init_struct.PLL3.PLL3P = 2;
	periph_clk_init_struct.PLL3.PLL3Q = 2;
	periph_clk_init_struct.PLL3.PLL3R = 30;  
	HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);  
}

void LCD_Init(void)
{ 
    /* 使能LTDC时钟 */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /* 使能DMA2D时钟 */
    __HAL_RCC_DMA2D_CLK_ENABLE();
	  /* 液晶屏时钟配置 */
		LCD_ClockConfig();
    /* 初始化LCD引脚 */
    LCD_GPIO_Config();
    /* 初始化SDRAM 用作LCD 显存*/
    SDRAM_Init();
    /* 配置LTDC参数 */
    Ltdc_Handler.Instance = LTDC;  
    /* 配置行同步信号宽度(HSW-1) */
    Ltdc_Handler.Init.HorizontalSync =HSW-1;
    /* 配置垂直同步信号宽度(VSW-1) */
    Ltdc_Handler.Init.VerticalSync = VSW-1;
    /* 配置(HSW+HBP-1) */
    Ltdc_Handler.Init.AccumulatedHBP = HSW+HBP-1;
    /* 配置(VSW+VBP-1) */
    Ltdc_Handler.Init.AccumulatedVBP = VSW+VBP-1;
    /* 配置(HSW+HBP+有效像素宽度-1) */
    Ltdc_Handler.Init.AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
    /* 配置(VSW+VBP+有效像素高度-1) */
    Ltdc_Handler.Init.AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
    /* 配置总宽度(HSW+HBP+有效像素宽度+HFP-1) */
    Ltdc_Handler.Init.TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 
    /* 配置总高度(VSW+VBP+有效像素高度+VFP-1) */
    Ltdc_Handler.Init.TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;

    /* 初始化LCD的像素宽度和高度 */
    Ltdc_Handler.LayerCfg->ImageWidth  = LCD_PIXEL_WIDTH;
    Ltdc_Handler.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;
    /* 设置LCD背景层的颜色，默认黑色 */
    Ltdc_Handler.Init.Backcolor.Red = 0;
    Ltdc_Handler.Init.Backcolor.Green = 0;
    Ltdc_Handler.Init.Backcolor.Blue = 0;
    /* 极性配置 */
    /* 初始化行同步极性，低电平有效 */
    Ltdc_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* 初始化场同步极性，低电平有效 */
    Ltdc_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* 初始化数据有效极性，低电平有效 */
    Ltdc_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* 初始化行像素时钟极性，同输入时钟 */
    Ltdc_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    HAL_LTDC_Init(&Ltdc_Handler);

    HAL_LTDC_ProgramLineEvent(&Ltdc_Handler, 0);
      
    /* Enable dithering */
    HAL_LTDC_EnableDither(&Ltdc_Handler);
    
    /* Set LTDC Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(LTDC_IRQn, 0, 0);  
    /* Enable LTDC Interrupt */
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
  
     /* Configure the DMA2D default mode */ 
    Dma2d_Handler.Init.Mode         = DMA2D_R2M;
    Dma2d_Handler.Init.ColorMode    = DMA2D_INPUT_ARGB8888;
    Dma2d_Handler.Init.OutputOffset = 0x0;     

    Dma2d_Handler.Instance          = DMA2D; 

    if(HAL_DMA2D_Init(&Dma2d_Handler) != HAL_OK)
    {
      while (1);
    }
}

///**
//  * @brief  初始化LCD层 
//  * @param  LayerIndex:  前景层(层1)或者背景层(层0)
//  * @param  FB_Address:  每一层显存的首地址
//  * @param  PixelFormat: 层的像素格式
//  * @retval 无
//  */
//void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address,uint32_t PixelFormat)
//{     
//  LTDC_LayerCfgTypeDef  layer_cfg;

//  /* 层初始化 */
//  layer_cfg.WindowX0 = 0;				//窗口起始位置X坐标
//  layer_cfg.WindowX1 = LCD_GetXSize();	//窗口结束位置X坐标
//  layer_cfg.WindowY0 = 0;				//窗口起始位置Y坐标
//  layer_cfg.WindowY1 = LCD_GetYSize();  //窗口结束位置Y坐标
//  layer_cfg.PixelFormat = PixelFormat;	//像素格式
//  layer_cfg.FBStartAdress = FB_Address; //层显存首地址
//  layer_cfg.Alpha = 255;				//用于混合的透明度常量，范围（0—255）0为完全透明
//  layer_cfg.Alpha0 = 0;					//默认透明度常量，范围（0—255）0为完全透明
//  layer_cfg.Backcolor.Blue = 0;			//层背景颜色蓝色分量
//  layer_cfg.Backcolor.Green = 0;		//层背景颜色绿色分量
//  layer_cfg.Backcolor.Red = 0;			//层背景颜色红色分量
//  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;//层混合系数1
//  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;//层混合系数2
//  layer_cfg.ImageWidth = LCD_GetXSize();//设置图像宽度
//  layer_cfg.ImageHeight = LCD_GetYSize();//设置图像高度
//  
//  HAL_LTDC_ConfigLayer(&Ltdc_Handler, &layer_cfg, LayerIndex); //设置选中的层参数

//  DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;//设置层的字体颜色
//  DrawProp[LayerIndex].pFont     = &LCD_DEFAULT_FONT;//设置层的字体类型
//  DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK; //设置层的字体背景颜色
//  
//  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);//重载层的配置参数
//}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
