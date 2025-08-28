#include <string.h>
#include <stdbool.h>
#include "bsp_fdcan.h"
#include "FreeRTOS.h"
#include "bsp_log.h"
#include "basic_math.h"

#ifdef USER_CAN1
static uint8_t idx1;
static CanInstance_s* fdcan1_instance[FDCAN_MAX_REGISTER_CNT]; // CAN1 实例数组,
static uint32_t USER_FDCAN1_FIFO_NUMBER;
#endif

#ifdef USER_CAN2
static uint8_t idx2;
static CanInstance_s* fdcan2_instance[FDCAN_MAX_REGISTER_CNT]; // CAN2
static uint32_t USER_FDCAN2_FIFO_NUMBER;
#endif

#ifdef USER_CAN3
static uint8_t idx3;
static CanInstance_s* fdcan3_instance[FDCAN_MAX_REGISTER_CNT]; // CAN3
static uint32_t USER_FDCAN3_FIFO_NUMBER;
#endif


/**
 * @file bsp_fdcan.c
 * @brief CAN 过滤器初始化函数
 * @return true-- 初始化成功   false-- 初始化失败
 * @date 2025-08-27
 * @todo 目前是全接收, 后续可以添加过滤功能; 判断 HAL 库函数的返回值
 */
static bool Can_Filter_Init(void) {
    // FDCAN1 过滤器配置
#ifdef USER_CAN1
    FDCAN_FilterTypeDef FDCAN1_FilterConfig;
    static uint32_t USER_FDCAN1_FrameFormat;
    // 判断 ID 类型
    if (hfdcan1.Init.FrameFormat == FDCAN_FRAME_CLASSIC){
        USER_FDCAN1_FrameFormat = FDCAN_STANDARD_ID;         // 使用经典 CAN，只有标准 ID
    }else{
        USER_FDCAN1_FrameFormat = FDCAN_EXTENDED_ID;         // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan1.Init.RxFifo0ElmtsNbr != 0){
        USER_FDCAN1_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN1 配置为 FIFO0 接收
    }else if (hfdcan1.Init.RxFifo1ElmtsNbr != 0){
        USER_FDCAN1_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN1 配置为 FIFO1 接收
    }else{
        return false; // 如果两个 FIFO 都没有元素，就返回错误
    }
    FDCAN1_FilterConfig.IdType =USER_FDCAN1_FrameFormat;
    FDCAN1_FilterConfig.FilterIndex = 0;                        // 过滤器编号，用几路 CAN 就以此类推 0、1、2
    FDCAN1_FilterConfig.FilterType = FDCAN_FILTER_MASK;         // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN1_FilterConfig.FilterConfig = USER_FDCAN1_FIFO_NUMBER;
    FDCAN1_FilterConfig.FilterID1 = 0x00000000;                 // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    FDCAN1_FilterConfig.FilterID2 = 0x00000000;                 // 过滤器 ID2

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &FDCAN1_FilterConfig) != HAL_OK){
        return false;
    } // 将上述配置应用到 FDCAN1
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
    {
        return false;
    }
    if (HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, USER_FDCAN1_FIFO_NUMBER, 0) != HAL_OK){
        return false;
    }
#endif
    // FDCAN2 过滤器配置
#ifdef USER_CAN2
    FDCAN_FilterTypeDef FDCAN2_FilterConfig;
    static uint32_t USER_FDCAN2_FrameFormat;
    // 判断 ID 类型
    if (hfdcan2.Init.FrameFormat == FDCAN_FRAME_CLASSIC){
        USER_FDCAN2_FrameFormat = FDCAN_STANDARD_ID;         // 使用标准 ID
    }else{
        USER_FDCAN2_FrameFormat = FDCAN_EXTENDED_ID;         // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan2.Init.RxFifo0ElmtsNbr != 0){
        USER_FDCAN2_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN2 配置为 FIFO0 接收
    }else if (hfdcan2.Init.RxFifo1ElmtsNbr != 0){
        USER_FDCAN2_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN2 配置为 FIFO1 接收
    }else{
        return false; // 如果两个 FIFO 都没有元素，就返回错误
    }
    FDCAN2_FilterConfig.IdType = USER_FDCAN2_FrameFormat;
    FDCAN2_FilterConfig.FilterIndex = 1;                        // 过滤器编号，用几路 CAN 就以此类推 0、1、2
    FDCAN2_FilterConfig.FilterType = FDCAN_FILTER_MASK;         // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN2_FilterConfig.FilterConfig = USER_FDCAN2_FIFO_NUMBER;
    FDCAN2_FilterConfig.FilterID1 = 0x00000000;                 // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    FDCAN2_FilterConfig.FilterID2 = 0x00000000;                 // 过滤器 ID2

    if (HAL_FDCAN_ConfigFilter(&hfdcan2, &FDCAN2_FilterConfig) != HAL_OK){
        return false;
    } // 将上述配置应用到 FDCAN2
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK){
        return false;
    }
    if (HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, USER_FDCAN2_FIFO_NUMBER, 0) != HAL_OK){
        return false;
    }
#endif
    // FDCAN3 过滤器配置
#ifdef USER_CAN3
    FDCAN_FilterTypeDef FDCAN3_FilterConfig;
    static uint32_t USER_FDCAN3_FrameFormat;
    // 判断 ID 类型
    if (hfdcan3.Init.FrameFormat == FDCAN_FRAME_CLASSIC){
        USER_FDCAN3_FrameFormat = FDCAN_STANDARD_ID;         // 使用经典 CAN，只有标准 ID
    }else{
        USER_FDCAN3_FrameFormat = FDCAN_EXTENDED_ID;         // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan3.Init.RxFifo0ElmtsNbr != 0){
        USER_FDCAN3_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN3 配置为 FIFO0 接收
        }else if (hfdcan3.Init.RxFifo1ElmtsNbr != 0){
        USER_FDCAN3_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN3 配置为 FIFO1 接收
        }else{
            return false; // 如果两个 FIFO 都没有元素，就返回错误
        }
    FDCAN3_FilterConfig.IdType = USER_FDCAN3_FrameFormat;
    FDCAN3_FilterConfig.FilterIndex = 2;                        // 过滤器编号
    FDCAN3_FilterConfig.FilterType = FDCAN_FILTER_MASK;         // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN3_FilterConfig.FilterConfig = USER_FDCAN3_FIFO_NUMBER;
    FDCAN3_FilterConfig.FilterID1 = 0x00000000;                // 过滤器 ID1，只要 ID2 配置为 0x00000000
    FDCAN3_FilterConfig.FilterID2 = 0x00000000;                // 过滤器 ID2
    if (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN3_FilterConfig) != HAL_OK){
        return false;
    } // 将上述配置应用到 FDCAN3
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK){
        return false;
    }
    if (HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, USER_FDCAN3_FIFO_NUMBER, 0) != HAL_OK){
        return false;
    }
#endif
    return true;
}


/**
 * @file bsp_fdcan.c
 * @brief CAN 使能函数
 * @return true-- 初始化成功   false-- 初始化失败
 * @date 2025-08-27
 * @todo 还要修
 */

uint8_t cnt_test = 0 ;
static bool Can_Service_Init(void){
#ifdef USER_CAN1
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK){
        Log_Error("FDCAN1 Starts Failed");
    }
#endif
#ifdef USER_CAN2
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK){
        Log_Error("FDCAN2 Starts Failed");
        return false;
    }
#endif
#ifdef USER_CAN3
    if (HAL_FDCAN_Start(&hfdcan3) != HAL_OK){
        Log_Error("FDCAN3 Starts Failed");
        return false;
    }
#endif
#ifdef USER_CAN1
    if (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0) != HAL_OK)
    {
        return false;
    }
#endif
#ifdef USER_CAN2
    Dwt_Delay_Ms(2);
    if (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0) != HAL_OK)
    {
        return false;
    }
#endif
#ifdef USER_CAN3
    Dwt_Delay_Ms(2);
    if (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0) != HAL_OK)
    {
        return false;
    }
#endif
    return true;
}

static bool Can_Init(void)
{
    if (!Can_Filter_Init()){
        return false;
    }
    if (!Can_Service_Init()){
        return false;
    }
    return true;
}

// 声明总和变量
// 统计所有 idx
static uint8_t Update_Total_Idx(void) {
    static uint8_t total_idx = 0;
#ifdef USER_CAN1
    total_idx += idx1;
#endif
#ifdef USER_CAN2
    total_idx += idx2;
#endif
#ifdef USER_CAN3
    total_idx += idx3;
#endif
    return total_idx;
}

/**
 * @file bsp_fdcan.h
 * @brief fdcan注册函数
 * @param config
 */
CanInstance_s* Can_Register(const CanInitConfig_s* config){
    if (config == NULL || config->can_handle == NULL){
        Log_Error("%s CanInitConfig Is Null",config->topic_name);
        return NULL; // 参数检查
    }
    if (!Update_Total_Idx()){
        if (Can_Init()){
            Log_Passing("Can Init Success");
        }else{
            Log_Error("Can Init Failed");
        }
    }
    CanInstance_s *instance = user_malloc(sizeof(CanInstance_s)); // 分配空间
    if (instance == NULL){
        Log_Error("%s CanInstance Malloc Failed",config->topic_name);
        return NULL; // 内存分配失败
    }
    memset(instance, 0, sizeof(CanInstance_s));
    instance->topic_name = config->topic_name;
    instance->tx_conf.Identifier = config->tx_id;
    instance->tx_conf.IdType = config->can_handle->Init.FrameFormat;
    instance->tx_conf.TxFrameType = FDCAN_DATA_FRAME;
    instance->tx_conf.DataLength = FDCAN_DLC_BYTES_8;
    instance->tx_conf.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // 传输节点 error active
    instance->tx_conf.BitRateSwitch = FDCAN_BRS_OFF;           // FDCAN 帧发送 / 接收不带波特率可变
    instance->tx_conf.FDFormat = FDCAN_CLASSIC_CAN;            // 设置为经典 CAN 帧格式
    instance->tx_conf.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // 不存储 Tx events 事件
    instance->tx_conf.MessageMarker = 0;

    instance->can_handle = config->can_handle;
    instance->tx_id = config->tx_id;
    instance->rx_id = config->rx_id;
    instance->can_module_callback = config->can_module_callback;
    instance->id = config->id;
    if (config->can_handle == &hfdcan1) {
        fdcan1_instance[idx1++] = instance;
    }
    Log_Passing("%s Register Successfully",instance->topic_name);
    return instance;
}

HAL_StatusTypeDef Can_Transmit(const CanInstance_s *instance)
{
    return HAL_FDCAN_AddMessageToTxFifoQ(instance->can_handle,&instance->tx_conf, instance->tx_buff);
}

void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs){
}

void HAL_FDCAN_RxFifo1Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    if(ErrorStatusITs & FDCAN_IR_BO)
    {
        CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
    }
    if(ErrorStatusITs & FDCAN_IR_EP)
    {
    }
}