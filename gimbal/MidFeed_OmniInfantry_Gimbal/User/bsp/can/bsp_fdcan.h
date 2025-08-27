#pragma once
#include "user_configuration.h"

#ifdef USER_CAN_FD
#ifndef BSP_FDCAN_H
#define BSP_FDCAN_H

#include <stdint.h>
#include "fdcan.h"

#define FDCAN_MAX_REGISTER_CNT 8   // 一条 FDCAN 总线最多支持 8 个模块, 即 8 个 FDCANInstance 实例
#define USER_CAN_CNT ((defined(USER_CAN1) ? 1 : 0) + \
                      (defined(USER_CAN2) ? 1 : 0) + \
                      (defined(USER_CAN3) ? 1 : 0))
#define MX_FDCAN_FILTER_CNT (USER_CAN_CNT *14) // FDCAN 每个 CAN 有 14 个过滤器, 每个过滤器可以配置 3 个 ID, 所以总共可以配置 42 个 ID

#pragma pack(1)
typedef struct _CanInstance_s
{
    FDCAN_HandleTypeDef *can_handle;                      // FDCAN 句柄
    FDCAN_TxHeaderTypeDef tx_conf;                        // FDCAN 报文发送配置
    uint16_t tx_id;                                       // 发送 id, 即发送的 FDCAN 报文 id
    uint8_t tx_buff[8];                                   // 发送缓存, 发送消息长度可以通过 Can_SetDLC() 设定
    uint16_t rx_id;                                       // 接收 id, 即接收的 FDCAN 报文 id
    uint8_t rx_buff[8];                                   // 接收缓存, 最大消息长度为 8
    uint8_t rx_len;                                       // 接收长度, 可能为 0-8
    void (*can_module_callback)(struct _CanInstance_s *); // 接收的回调函数, 用于解析接收到的数据
    void *id;                                             // 使用 can 外设的模块指针 (即 id 指向的模块拥有此 can 实例, 是父子关系)
}CanInstance_s;

typedef struct
{
    FDCAN_HandleTypeDef *can_handle;
    uint16_t tx_id;
    uint16_t rx_id;
    void (*can_module_callback)(struct _CanInstance_s *);
    void *id;
}CanInitConfig_s;
CanInstance_s* Can_Register(const CanInitConfig_s* config);
HAL_StatusTypeDef Can_Transmit(const CanInstance_s *instance);
#endif
#endif
