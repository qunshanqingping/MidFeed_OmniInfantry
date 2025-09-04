#pragma once
#include "user_configuration.h"
#ifndef BSP_CAN_H
#define BSP_CAN_H
#ifdef USER_CAN_STANDARD
#include <stdint.h>
#include "can.h"
/**
 *@brief 1路CAN最大注册实例数，1M Baud rate下最多建议8个实例
 */
#define FDCAN_MAX_REGISTER_CNT 8

#pragma pack(1)
typedef struct _CanInstance_s
{
    char* topic_name;                                     //实例名称
    CAN_HandleTypeDef *can_handle;                        // CAN 句柄
    CAN_TxHeaderTypeDef tx_conf;                          // CAN 报文发送配置
    uint16_t tx_id;                                       // 发送 id, 即发送的 CAN 报文 id
    uint8_t tx_buff[8];                                   // 发送缓存, 可以不用，但建议保留，方便调试
    uint16_t rx_id;                                       // 接收 id, 即接收的 CAN 报文 id
    uint8_t rx_buff[8];                                   // 接收缓存, 目前保留，增加了一次 memcpy 操作，方便监视所以保留
    uint8_t rx_len;                                       // 接收长度, 可能为 0-8
    void (*can_module_callback)(struct _CanInstance_s *); // 接收的回调函数, 用于解析接收到的数据，如果增加了 uint8_t *rx_buff 成员，前面的rx_buff[8] 可以删去
    void *id;                                             // 使用 can 外设的模块指针 (即 id 指向的模块拥有此 can 实例, 是父子关系)
}CanInstance_s;

typedef struct{
    char* topic_name;                                     //实例名称
    uint8_t can_channel;                                  //can通道号 1,2 分别对应 CAN1, CAN2，为了抽象接口向module层隐藏HAL库
    uint16_t tx_id;                                       //发送id
    uint16_t rx_id;                                       //接收id
    void (*can_module_callback)(struct _CanInstance_s *); //接收的回调函数, 用于解析接收到的数据
    void *id;                                             //使用 can 外设的父指针 (即 id 指向的模块拥有此 can 实例, 是父子关系)
}CanInitConfig_s;
#pragma pack()
/**
 * @brief CAN接收帧结构体。
 * 该结构体用于存储从CAN接收的消息，包括消息头和数据缓冲区。
 */
typedef struct {
    CAN_RxHeaderTypeDef RxHeader;                         // CAN 消息头
    uint8_t 			rx_buff[8];                       // 数据缓冲区
}CAN_RxFrame_TypeDef;
};
#endif //BSP_CAN_H
#endif
