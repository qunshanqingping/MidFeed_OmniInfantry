#include "bsp_can.h"
#ifdef USER_CAN_STANDARD
#include "bsp_fdcan.h"
#include "FreeRTOS.h"
#include "bsp_log.h"
#include "basic_math.h"
#include <string.h>
#include <stdbool.h>
#ifdef USER_CAN1
// ReSharper disable once CppDeclaratorNeverUsed
static uint8_t idx1;
// ReSharper disable once CppDeclaratorNeverUsed
static CanInstance_s *can1_instance[FDCAN_MAX_REGISTER_CNT]; // CAN1 实例数组,
#endif
#ifdef USER_CAN2
// ReSharper disable once CppDeclaratorNeverUsed
static uint8_t idx2;
// ReSharper disable once CppDeclaratorNeverUsed
static CanInstance_s *can2_instance[FDCAN_MAX_REGISTER_CNT]; // CAN2
#endif

/* 过滤器编号 */
// ReSharper disable once CppDeclaratorNeverUsed
static uint8_t can_filter_index = 0;

/* can初始化标志 */
static bool fdcan_init_flag = true;

/* 接收帧 */
static CAN_RxFrame_TypeDef *CAN_RxFIFO0Frame;
static CAN_RxFrame_TypeDef *CAN_RxFIFO1Frame;

#endif
