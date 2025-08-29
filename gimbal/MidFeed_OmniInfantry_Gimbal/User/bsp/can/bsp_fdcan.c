#include <string.h>
#include <stdbool.h>
#include "bsp_fdcan.h"
#include "FreeRTOS.h"
#include "bsp_log.h"
#include "basic_math.h"

#ifdef USER_CAN1
static uint8_t idx1;
// ReSharper disable once CppDeclaratorNeverUsed
static CanInstance_s *fdcan1_instance[FDCAN_MAX_REGISTER_CNT]; // CAN1 实例数组,
static uint32_t USER_FDCAN1_FIFO_NUMBER;
#endif

#ifdef USER_CAN2
static uint8_t idx2;
// ReSharper disable once CppDeclaratorNeverUsed
static CanInstance_s *fdcan2_instance[FDCAN_MAX_REGISTER_CNT]; // CAN2
static uint32_t USER_FDCAN2_FIFO_NUMBER;
#endif

#ifdef USER_CAN3
static uint8_t idx3;
// ReSharper disable once CppDeclaratorNeverUsed
static CanInstance_s *fdcan3_instance[FDCAN_MAX_REGISTER_CNT]; // CAN3
static uint32_t USER_FDCAN3_FIFO_NUMBER;
#endif

/* 私有函数 ---------------------------------------------------------------------*/
/**
 * @file bsp_fdcan.c
 * @brief CAN 过滤器初始化函数
 * @return true-- 初始化成功   false-- 初始化失败
 * @date 2025-08-27
 * @todo 目前是全接收, 后续添加过滤功能; 判断 HAL 库函数的返回值
 */
static bool Can_Filter_Init(void) {
    // FDCAN1 过滤器配置
#ifdef USER_CAN1
    FDCAN_FilterTypeDef FDCAN1_FilterConfig;
    static uint32_t USER_FDCAN1_FrameFormat;
    // 判断 ID 类型
    if (hfdcan1.Init.FrameFormat == FDCAN_FRAME_CLASSIC) {
        USER_FDCAN1_FrameFormat = FDCAN_STANDARD_ID; // 使用经典 CAN，只有标准 ID
    } else {
        USER_FDCAN1_FrameFormat = FDCAN_EXTENDED_ID; // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan1.Init.RxFifo0ElmtsNbr != 0) {
        USER_FDCAN1_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN1 配置为 FIFO0 接收
    } else if (hfdcan1.Init.RxFifo1ElmtsNbr != 0) {
        USER_FDCAN1_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN1 配置为 FIFO1 接收
    } else {
        Log_Error("FDCAN1 has none fifo for rx");
        return false; // 如果两个 FIFO 都没有元素，就返回错误
    }
    FDCAN1_FilterConfig.IdType = USER_FDCAN1_FrameFormat;
    FDCAN1_FilterConfig.FilterIndex = 0; // 过滤器编号，用几路 CAN 就以此类推 0、1、2
    FDCAN1_FilterConfig.FilterType = FDCAN_FILTER_MASK; // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN1_FilterConfig.FilterConfig = USER_FDCAN1_FIFO_NUMBER;
    FDCAN1_FilterConfig.FilterID1 = 0x00000000; // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    FDCAN1_FilterConfig.FilterID2 = 0x00000000; // 过滤器 ID2

    while (HAL_FDCAN_ConfigFilter(&hfdcan1, &FDCAN1_FilterConfig) != HAL_OK) {
        Log_Error("FDCAN1 configs filter failed");
    } //拒绝接收匹配不成功的标准ID和扩展ID,不接受远程帧
    while (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=
        HAL_OK) {
        Log_Error("FDCAN1 configs global filter failed");
    }
    // 水印中断，接受1条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, USER_FDCAN1_FIFO_NUMBER, 1) != HAL_OK) {
        Log_Error("FDCAN1 configs fifo watermark failed");
    }
#endif
    // FDCAN2 过滤器配置
#ifdef USER_CAN2
    FDCAN_FilterTypeDef FDCAN2_FilterConfig;
    static uint32_t USER_FDCAN2_FrameFormat;
    // 判断 ID 类型
    if (hfdcan2.Init.FrameFormat == FDCAN_FRAME_CLASSIC) {
        USER_FDCAN2_FrameFormat = FDCAN_STANDARD_ID; // 使用标准 ID
    } else {
        USER_FDCAN2_FrameFormat = FDCAN_EXTENDED_ID; // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan2.Init.RxFifo0ElmtsNbr != 0) {
        USER_FDCAN2_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN2 配置为 FIFO0 接收
    } else if (hfdcan2.Init.RxFifo1ElmtsNbr != 0) {
        USER_FDCAN2_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN2 配置为 FIFO1 接收
    } else {
        Log_Error("FDCAN2 has none fifo for rx");
        return false; // 如果两个 FIFO 都没有元素，就返回错误
    }
    FDCAN2_FilterConfig.IdType = USER_FDCAN2_FrameFormat;
    FDCAN2_FilterConfig.FilterIndex = 1; // 过滤器编号，用几路 CAN 就以此类推 0、1、2
    FDCAN2_FilterConfig.FilterType = FDCAN_FILTER_MASK; // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN2_FilterConfig.FilterConfig = USER_FDCAN2_FIFO_NUMBER;
    FDCAN2_FilterConfig.FilterID1 = 0x00000000; // 过滤器 ID1，只要 ID2 配置为 0x00000000，就不会过滤任何 ID
    FDCAN2_FilterConfig.FilterID2 = 0x00000000; // 过滤器 ID2

    while (HAL_FDCAN_ConfigFilter(&hfdcan2, &FDCAN2_FilterConfig) != HAL_OK) {
        Log_Error("FDCAN2 configs filter failed");
    } ///拒绝接收匹配不成功的标准ID和扩展ID,不接受远程帧
    while (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=
        HAL_OK) {
        Log_Error("FDCAN2 configs global filter failed");
    }
    // 水印中断，接受1条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, USER_FDCAN2_FIFO_NUMBER, 1) != HAL_OK) {
        Log_Error("FDCAN2 configs fifo watermark failed");
    }
#endif
    // FDCAN3 过滤器配置
#ifdef USER_CAN3
    FDCAN_FilterTypeDef FDCAN3_FilterConfig;
    static uint32_t USER_FDCAN3_FrameFormat;
    // 判断 ID 类型
    if (hfdcan3.Init.FrameFormat == FDCAN_FRAME_CLASSIC) {
        USER_FDCAN3_FrameFormat = FDCAN_STANDARD_ID; // 使用经典 CAN，只有标准 ID
    } else {
        USER_FDCAN3_FrameFormat = FDCAN_EXTENDED_ID; // 使用扩展 ID
    }
    // 判断使用哪个 FIFO 接收
    if (hfdcan3.Init.RxFifo0ElmtsNbr != 0) {
        USER_FDCAN3_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO0; // FDCAN3 配置为 FIFO0 接收
    } else if (hfdcan3.Init.RxFifo1ElmtsNbr != 0) {
        USER_FDCAN3_FIFO_NUMBER = FDCAN_FILTER_TO_RXFIFO1; // FDCAN3 配置为 FIFO1 接收
    } else {
        Log_Error("FDCAN3 has none fifo for rx");
        return false; // 如果两个 FIFO 都没有元素，就返回错误
    }
    FDCAN3_FilterConfig.IdType = USER_FDCAN3_FrameFormat;
    FDCAN3_FilterConfig.FilterIndex = 2; // 过滤器编号
    FDCAN3_FilterConfig.FilterType = FDCAN_FILTER_MASK; // 过滤器 Mask 模式 关乎到 ID1ID2 的配置
    FDCAN3_FilterConfig.FilterConfig = USER_FDCAN3_FIFO_NUMBER;
    FDCAN3_FilterConfig.FilterID1 = 0x00000000; // 过滤器 ID1，只要 ID2 配置为 0x00000000
    FDCAN3_FilterConfig.FilterID2 = 0x00000000; // 过滤器 ID2
    while (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN3_FilterConfig) != HAL_OK) {
        Log_Error("FDCAN3 configs filter failed");
    } //拒绝接收匹配不成功的标准ID和扩展ID,不接受远程帧
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) !=
        HAL_OK) {
        Log_Error("FDCAN3 configs global filter failed");
    }
    // 水印中断，接受1条消息触发中断
    while (HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, USER_FDCAN3_FIFO_NUMBER, 1) != HAL_OK) {
        Log_Error("FDCAN3 configs fifo watermark failed");
    }
#endif
    return true;
}


/**
 * @file bsp_fdcan.c
 * @brief CAN 使能函数
 * @return true-- 初始化成功   false-- 初始化失败
 * @date 2025-08-27
 * @note 目前只能选择一种中断方式
 * @todo 可以选择多种中断方式
 */
static void Can_Service_Init(void) {
#ifdef USER_CAN1
    while (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        Log_Error("FDCAN1 Starts Failed");
    }
#endif
#ifdef USER_CAN2
    while (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        Log_Error("FDCAN2 Starts Failed");
    }
#endif
#ifdef USER_CAN3
    while (HAL_FDCAN_Start(&hfdcan3) != HAL_OK) {
        Log_Error("FDCAN3 Starts Failed");
    }
#endif
#ifdef USER_CAN1
    if (hfdcan1.Init.RxFifo0ElmtsNbr != 0) {
        while (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN1 configs interruption failed");
        }
    } else {
        while (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN1 configs interruption failed");
        }
    }
#endif
#ifdef USER_CAN2
    if (hfdcan2.Init.RxFifo0ElmtsNbr != 0) {
        while (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN2 configs interruption failed");
        }
    } else {
        while (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN2 configs interruption failed");
        }
    }
#endif
#ifdef USER_CAN3
    if (hfdcan3.Init.RxFifo0ElmtsNbr != 0) {
        while (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN3 configs interruption failed");
        }
    } else {
        while (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) {
            Log_Error("FDCAN3 configs interruption failed");
        }
    }
#endif
}
/**
 * @file bsp_fdcan.c
 * @brief CAN 滤波器初始化函数
 * @date 2025-08-27
 * @note 目前只能选择一种滤波器模式--掩码，且不能过滤ID
 * @todo 可以选择多种滤波器模式
 */
static void Can_Init(void) {
    if (Can_Filter_Init()) {
        Can_Service_Init();
        Log_Passing("Can Init successfully");;
    }
    Log_Error("Can Init successfully");
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

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @file bsp_fdcan.c
 * @brief fdcan注册函数
 * @param config CAN 初始化配置结构体
 * @return 注册的 CAN 实例指针
 */
CanInstance_s *Can_Register(const CanInitConfig_s *config) {
    if (config == NULL || config->can_handle == NULL) {
        Log_Error("%s CanInitConfig Is Null", config->topic_name);
        return NULL; // 参数检查
    }
    if (!Update_Total_Idx()) {
        Can_Init();
    }
    CanInstance_s *instance = user_malloc(sizeof(CanInstance_s)); // 分配空间
    if (instance == NULL) {
        Log_Error("%s CanInstance Malloc Failed", config->topic_name);
        return NULL; // 内存分配失败
    }
    memset(instance, 0, sizeof(CanInstance_s));
    instance->topic_name = config->topic_name;
    instance->tx_conf.Identifier = config->tx_id;
    instance->tx_conf.IdType = config->can_handle->Init.FrameFormat;
    instance->tx_conf.TxFrameType = FDCAN_DATA_FRAME;
    instance->tx_conf.DataLength = FDCAN_DLC_BYTES_8;
    instance->tx_conf.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // 传输节点 error active
    instance->tx_conf.BitRateSwitch = FDCAN_BRS_OFF; // FDCAN 帧发送 / 接收不带波特率可变
    instance->tx_conf.FDFormat = FDCAN_CLASSIC_CAN; // 设置为经典 CAN 帧格式
    instance->tx_conf.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // 不存储 Tx events 事件
    instance->tx_conf.MessageMarker = 0;

    instance->can_handle = config->can_handle;
    instance->tx_id = config->tx_id;
    instance->rx_id = config->rx_id;
    instance->can_module_callback = config->can_module_callback;
    instance->parent_pointer = config->parent_pointer;
    if (config->can_handle == &hfdcan1) {
        fdcan1_instance[idx1++] = instance;
    }
    Log_Passing("%s Register Successfully", instance->topic_name);
    return instance;
}
/**
 * @file bsp_fdcan.c
 * @brief FDCAN 发送函数
 * @param instance FDCAN 实例
 * @param tx_buff 发送缓冲区
 * @param time_out 超时时间
 * @return true-- 发送成功   false-- 发送失败
 */
bool Can_Transmit(const CanInstance_s *instance, const uint8_t *tx_buff, const float time_out) {
    static uint8_t busy_count;
    static volatile float wait_time;
    const float dwt_start = Dwt_Get_Time_Line_Ms();
    while (HAL_FDCAN_GetTxFifoFreeLevel(instance->can_handle) == 0) {
        if (Dwt_Get_Time_Line_Ms() - dwt_start > time_out) {
            Log_Warning("%s fdcan transition is time out", instance->topic_name);
            return false;
        }
        busy_count++;
        Log_Warning("%s fdcan transition bus busy cnt is %d", instance->topic_name, busy_count);
    }
    //测试上一段代码耗时
    wait_time = Dwt_Get_Time_Line_Ms() - dwt_start;
    if (wait_time > 0.01) {
        Log_Warning("%s fdcan transition wait time is %f ms", instance->topic_name, wait_time);
    }
    if (HAL_FDCAN_AddMessageToTxFifoQ(instance->can_handle, &instance->tx_conf, tx_buff) == HAL_OK) {
        return true;
    }
    busy_count++;
    Log_Warning("%s fdcan transition is busy %d", instance->topic_name, busy_count);
    return false;
}


void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs) {
    if (ErrorStatusITs & FDCAN_IR_BO) {
        CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
    }
    if (ErrorStatusITs & FDCAN_IR_EP) {
    }
}
