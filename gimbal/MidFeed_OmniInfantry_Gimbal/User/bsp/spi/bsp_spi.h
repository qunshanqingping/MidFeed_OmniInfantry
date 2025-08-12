/**
 * @file bsp_spi.h
 * @brief STM32cubemx生成的SPI文件的进一步抽象
 * @author Adonis Jin
 * @date 2025-08-08
 * @version 1.0.0
 */
#ifndef BSP_SPI_H
#define BSP_SPI_H

/* 包含文件 ------------------------------------------------------------------*/

#include "spi.h"
#include "stdbool.h"

/* 宏定义 -------------------------------------------------------------------*/

/**
 * @brief SPI调试模式，用于解决配置与cubemx配置之间的冲突
 * @param 0 发布模式
 * @note 发布模式将继续按照cubemx的配置进行配置
 * @param 1 调试模式将停止配置并终止程序
 */
#define SPI_DEBUG_MODE 0 // 0: 发布模式, 1: 调试模式

/**
 * @brief 根据不同开发板的SPI设备数量
 * @todo 不再局限于DJI C型开发板和达妙MC02开发板，而是根据实际工程需求
 */
#ifdef __STM32F407xx_HAL_H
/**
 * @brief DJI C型开发板配备了两个SPI接口，
 * 分别连接到BMI088和扩展IO，并从8针插座引出
 */
#define SPI_DEVICE_CNT 2
#endif
#ifdef STM32H7xx_HAL_H
/**
 * @brief 达妙MC02开发板配备了三个SPI接口，
 * 连接到BMI088、WS2812B和扩展IO，并从16针插座引出
 */
#define SPI_DEVICE_CNT 3
#endif

/**
 * @brief 支持的SPI从设备最大数量
 * @note 此值可根据实际项目需求调整
 */
#define MX_SPI_BUS_SLAVE_CNT 4

/* 类型定义 ------------------------------------------------------------------*/

/**
 * @brief SPI模式枚举
 * @details 此枚举定义了SPI通信的不同操作模式
 */
typedef enum
{
    SPI_BLOCKING_MODE = 0, //!< 阻塞模式，程序将等待直到SPI传输完成
    SPI_IT_MODE = 1,       //!< 中断模式，程序不会等待SPI传输完成，而是使用中断来通知完成
    SPI_DMA_MODE = 2,      //!< DMA模式，程序不会等待SPI传输完成，而是使用DMA来传输数据
}SpiMode_e;

/**
 * @brief SPI片选模式枚举
 * @details 此枚举定义了SPI片选信号的使能和禁用状态
 * @note 该枚举定义是为了兼容点对点，不存在cs引脚的设备
 */
typedef enum
{
    SPI_CS_ENABLE = 0,  //!< 使能SPI片选
    SPI_CS_DISABLE = 1, //!< 禁用SPI片选
}SpiCsMode_e;

#pragma pack(1)
/**
 * @brief SPI实例化结构体
 */
typedef struct _SpiInstance_s
{
    SPI_HandleTypeDef spi_handle;                         //!< SPI实例的句柄
    SpiMode_e mode;                                       //!< SPI操作模式（阻塞、中断或DMA）
    SpiCsMode_e cs_mode;                                   //!< SPI片选模式（使能或禁用）

    uint8_t slave_num;                                    //!< SPI从设备ID号,范围0~(MX_SPI_BUS_SLAVE_CNT-1),需要用户手动写入
    void* (*spi_module_callback)(struct _SpiInstance_s*); //!< SPI数据接收回调函数，用于处理接收到的数据
    void* id;                                             //!< 使用此SPI实例的父模块指针

}SpiInstance_s;

/**
 * @brief SPI配置结构体
 */
typedef struct
{
    SPI_HandleTypeDef spi_handle;                          //!< SPI实例的句柄
    SpiMode_e mode;                                        //!< SPI操作模式（阻塞、中断或DMA）
    SpiCsMode_e cs_mode;                                   //!< SPI片选模式（使能或禁用）

    uint8_t slave_num;                                    //!< SPI从设备ID号,范围0~(MX_SPI_BUS_SLAVE_CNT-1),需要用户手动写入
    void* (*spi_module_callback)(struct _SpiInstance_s*);  //!< SPI数据接收回调函数，用于处理接收到的数据
    void* id;                                              //!< 使用此SPI实例的父模块指针
}SpiInitConfig_s;
#pragma pack()

/* 函数定义 ---------------------------------------------------------------*/

/**@file bsp_spi.h
 * @brief SPI实例注册函数
 * @param config 指向SPI配置结构体的指针
 * @return SpiInstance_s结构体指针 --注册成功
 *         NULL --注册失败
 * @note 确保有足够的内存用于动态分配
 * @date 2025-08-08
 */
SpiInstance_s Spi_Register(SpiInitConfig_s config);

bool Spi_Transmit(SpiInstance_s* spi_instance, uint8_t* data, uint16_t len);

bool Spi_Receive(SpiInstance_s* spi_instance, uint8_t* data, uint16_t len);

#endif //BSP_SPI_H