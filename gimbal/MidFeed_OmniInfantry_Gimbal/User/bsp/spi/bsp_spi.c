/**
 * @file bsp_spi.c
 * @author Adonis Jin
 * @brief STM32cubemx生成的SPI文件的进一步抽象
 * @date 2025-08-08
 * @version 1.0.0
 * @note 本模块提供了SPI通信的高级接口，使得与各种SPI设备的集成更加简单，
 *       无需直接与HAL库交互。
 */

/* 包含文件 ------------------------------------------------------------------*/
#include "bsp_spi.h"
#include "memory.h"
#include "stdlib.h"

/* 私有变量 -----------------------------------------------------------------*/

/**
 * @brief 用于存储SPI实例指针的数组同时用于在执行回调操作时确定中断的来源
 */
static SpiInstance_s *spi_instances[SPI_DEVICE_CNT*MX_SPI_BUS_SLAVE_CNT]={NULL};

/**
 * 配合中断以及初始化
 */
static uint8_t idx = 0;

/**
 * @brief 用于判断当前spi是否正在传输,防止多个模块同时使用一个spi总线
 *        0--正在传输; 1--未传输
 */
uint8_t SPIDeviceOnGoing[SPI_DEVICE_CNT] = {1};