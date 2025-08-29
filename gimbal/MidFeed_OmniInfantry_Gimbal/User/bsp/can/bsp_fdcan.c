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
 * @brief 初始化FDCAN过滤器配置。
 *
 * 该函数根据用户定义的宏（USER_CAN1, USER_CAN2, USER_CAN3）初始化相应的FDCAN实例的过滤器。对于每个启用的FDCAN实例，它首先确定ID类型（标准或扩展），然后选择接收FIFO（FIFO0或FIFO1）。如果两个FIFO都没有元素，则记录错误并返回false。接着，配置过滤器参数，包括ID类型、过滤器索引、过滤器类型和过滤器配置。此外，还配置了全局过滤器以拒绝不匹配的标准ID和扩展ID以及远程帧，并设置了FIFO水印中断。如果在配置过程中出现任何错误，将记录错误日志并重试直到成功。
 * 注意：此函数依赖于HAL库提供的FDCAN相关API。
 * @return 如果所有FDCAN实例的过滤器配置成功，返回true；否则返回false。
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
 * @brief 初始化CAN服务。
 *
 * 该函数根据用户配置的宏定义（USER_CAN1, USER_CAN2, USER_CAN3）来初始化相应的FDCAN硬件实例。对于每个启用的FDCAN实例，首先尝试启动它。如果启动失败，则记录错误日志并继续重试直到成功。随后，根据RxFifo0ElmtsNbr的值选择激活Rx FIFO 0或Rx FIFO 1的消息接收中断通知。如果激活中断通知失败，同样会记录错误日志，并且持续尝试直到成功为止。
 * 注意：此函数依赖于HAL库提供的FDCAN相关API。
 * @return 无返回值（void）。
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
 * @brief 初始化CAN模块。
 *
 * 该函数首先尝试初始化CAN过滤器，如果成功，则继续初始化CAN服务，并记录一条通过日志。如果CAN过滤器初始化失败，则直接记录一条错误日志。
 * 注意：无论初始化是否成功，都会打印一条关于CAN初始化的日志信息，
 * @return 无返回值（void）。
 */
static void Can_Init(void) {
    if (Can_Filter_Init()) {
        Can_Service_Init();
        Log_Passing("Can Init successfully");;
    }else {
        Log_Error("Can Init failed");
    }
}

/**
 * @brief 更新并返回累计索引值。
 *
 * 该函数根据预处理器宏`USER_CAN1`, `USER_CAN2`, 和 `USER_CAN3`是否定义，来决定是否累加对应的索引值`idx1`, `idx2`, 和 `idx3`到静态变量`total_idx`上。每次调用此函数时都会更新`total_idx`的值，并返回更新后的值。
 *
 * @return 返回更新后的累计索引值。
 */
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
 * @brief 注册一个新的CAN实例。
 *
 * 根据提供的配置信息创建并初始化一个新的CAN实例。如果配置信息为空或内存分配失败，则函数将返回NULL。成功注册后，新的CAN实例将被添加到相应的FDCAN实例列表中，并返回指向新实例的指针。
 *
 * @param config 指向包含CAN初始化配置信息（如句柄、发送ID等）的CanInitConfig_s结构的指针。
 *
 * @return 如果成功注册则返回指向新创建的CanInstance_s结构的指针；如果配置无效或内存分配失败则返回NULL。
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
 * @brief 通过指定的CAN实例发送消息。
 *
 * 此函数尝试将提供的数据缓冲区中的消息添加到指定FDCAN实例的传输FIFO中。如果传输FIFO没有空闲空间，函数将等待直到有空闲空间可用或超时。如果在指定时间内未能成功添加消息，则返回false。
 *
 * @param instance 指向包含CAN配置信息（如句柄、发送配置等）的CanInstance_s结构的指针。
 * @param tx_buff 指向要发送的数据缓冲区的指针。
 * @param time_out 等待传输FIFO空闲的最大时间（以毫秒为单位）。
 *
 * @return 如果消息成功添加到传输FIFO则返回true；如果超时或添加失败则返回false。
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

/**
 * @brief FDCAN接收FIFO中断的回调函数。
 *
 * 该函数处理接收FIFO中的消息。它检索消息，并在消息标识符与已注册的CAN实例之一匹配时调用相应的回调函数。
 *
 * @param hfdcan 指向包含指定FDCAN配置信息的FDCAN_HandleTypeDef结构的指针。
 * @param RxFifoITs 触发此回调的中断源。
 * @param idx 表示fdcan_instance数组中CanInstance_s元素的数量的索引。
 * @param fdcan_instance CanInstance_s结构数组，每个结构包含特定CAN实例的配置和回调。
 */
static void FDCAN_RxFifoCallback(FDCAN_HandleTypeDef *hfdcan, const uint32_t RxFifoITs, const uint8_t idx, CanInstance_s *fdcan_instance) {
    FDCAN_RxHeaderTypeDef rxconf;
    uint8_t rx_buff[8];
    while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, RxFifoITs)) {
        if (HAL_FDCAN_GetRxMessage(hfdcan, RxFifoITs, &rxconf, rx_buff)) {
            if (idx == 0)
                return;
            for (uint8_t i = 0; i < idx; i++) {
                if (rxconf.Identifier == fdcan_instance[i].rx_id) {
                    if (fdcan_instance[i].can_module_callback != NULL)
                        fdcan_instance[i].rx_len = rxconf.DataLength;
                    memcpy(fdcan_instance[i].rx_buff, rx_buff, rxconf.DataLength);
                    fdcan_instance[i].can_module_callback(&fdcan_instance[i]);
                }
                break;
            }
        }
    }
}

/**
 * @brief FDCAN接收FIFO0中断的回调函数。
 *
 * 该函数处理接收FIFO0中的消息。当检测到新的消息时，它会根据配置调用相应的用户定义的回调函数。
 *
 * @param hfdcan 指向包含指定FDCAN配置信息的FDCAN_HandleTypeDef结构的指针。
 * @param RxFifo0ITs 触发此回调的中断源。
 */
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
#ifdef USER_CAN1_FIFO_0
        if (hfdcan == &hfdcan1){
        FDCAN_RxFifoCallback(hfdcan,RxFifo0ITs,idx1,*fdcan1_instance);
            }
#endif
#ifdef USER_CAN2_FIFO_0
        if (hfdcan == &hfdcan2){
            FDCAN_RxFifoCallback(hfdcan,RxFifo0ITs,idx2,*fdcan2_instance);
        }
#endif
#ifdef USER_CAN3_FIFO_0
        if (hfdcan == &hfdcan3){
            FDCAN_RxFifoCallback(hfdcan,RxFifo0ITs,idx3,*fdcan3_instance);
        }
#endif
    }
}

/**
 * @brief FDCAN接收FIFO1中断的回调函数。
 *
 * 该函数处理来自FDCAN接收FIFO1中的消息。当接收到新消息时，它会根据配置调用相应的用户定义回调函数。
 *
 * @param hfdcan 指向包含指定FDCAN配置信息的FDCAN_HandleTypeDef结构的指针。
 * @param RxFifo1ITs 触发此回调的中断源。
 */
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
    if (RxFifo1ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
#ifdef USER_CAN1_FIFO_1
        if (hfdcan == &hfdcan1){
            FDCAN_RxFifoCallback(hfdcan,RxFifo1ITs,idx1,fdcan1_instance);
        }
#endif
#ifdef USER_CAN2_FIFO_1
        if (hfdcan == &hfdcan2){
            FDCAN_RxFifoCallback(hfdcan,RxFifo1ITs,idx2,fdcan2_instance);
        }
#endif
#ifdef USER_CAN3_FIFO_1
        if (hfdcan == &hfdcan3){
            FDCAN_RxFifoCallback(hfdcan,RxFifo1ITs,idx3,fdcan3_instance);
        }
#endif
    }
}

//对HAL_FDCAN_ErrorStatusCallback的重载
//@todo 待完善
// ReSharper disable once CppParameterMayBeConstPtrOrRef
// ReSharper disable once CppParameterMayBeConst
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs){
    if (ErrorStatusITs & FDCAN_IR_BO) {
        CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
    }
}
