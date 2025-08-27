/**
 *@file user_configuration.h
 *@brief 初始化配置文件
 *@version 1.0
 *@date 2025-08-27
 *@note 该文件用于存放用户的配置选项，如宏定义、常量；避免对bsp层文件的直接修改
 *@author Adonis_Jin
 */

#ifndef USER_CONFIGURATION_H
#define USER_CONFIGURATION_H

/* CAN初始化配置选项 */

//选择CAN类型
#define USER_CAN_FD
// #define USER_CAN_STANDARD


//选择CAN总线
#define USER_CAN1
#define USER_CAN2
#define USER_CAN3

//检查是否出现定义冲突,只允许一个CAN类型定义存在,否则编译会自动报错
#if defined(USER_CAN_FD) && defined(USER_CAN_STANDARD)
#error "USER_CAN_FD 和 USER_CAN_STANDARD 不能同时定义！"
#endif
//检查是否出现定义冲突,标准can只支持CAN1和CAN2
#if defined(USER_CAN_STANDARD) &&defined(USER_CAN3)
#error "USER_CAN_STANDARD 不支持CAN3！"
#endif

#endif //USER_CONFIGURATION_H
