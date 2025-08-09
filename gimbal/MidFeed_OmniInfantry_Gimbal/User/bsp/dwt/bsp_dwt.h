
/**
 * @file bsp_dwt.h
 * @brief DWT (Data Watchpoint and Trace) 模块 BSP 层头文件
 * @author Adonis Jin
 * @date 2025-7-27
 */

#ifndef __BSP_DWT_H
#define __BSP_DWT_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief 系统时间结构体定义
     */
    typedef struct
    {
        uint32_t s;   // 秒
        uint16_t ms;  // 毫秒
        uint16_t us;  // 微秒
    } DwtTime;

    /**
     * @brief 初始化DWT（Data Watchpoint and Trace）模块
     *
     * 使能DWT CYCCNT计数器，并根据CPU频率设置相关变量。
     *
     * @param cpu_freq_mhz CPU频率（MHz）
     */
    void dwt_init(uint32_t cpu_freq_mhz);

    /**
     * @brief 获取两个时间点之间的时间差(单精度浮点数)
     *
     * 计算从上次记录的时间点到当前时间点经过的时间，单位为秒
     *
     * @param cnt_last 指向记录上次计数值的指针
     * @return float 时间差，单位为秒
     */
    float get_time_delta(uint32_t *cnt_last);

    /**
     * @brief 获取两个时间点之间的时间差(双精度浮点数)
     *
     * 计算从上次记录的时间点到当前时间点经过的时间，单位为秒
     *
     * @param cnt_last 指向记录上次计数值的指针
     * @return double 时间差，单位为秒
     */
    double get_time_delta64(uint32_t *cnt_last);

    /**
     * @brief 更新系统时间
     *
     * 根据DWT计数器的值更新系统时间结构体，包括秒、毫秒和微秒
     */
    void dwt_sys_time_update(void);

    /**
     * @brief 获取系统时间线(秒)
     *
     * 返回系统运行的总时间，单位为秒，精度到微秒
     *
     * @return float 系统运行时间，单位为秒
     */
    float dwt_get_time_line_s(void);

    /**
     * @brief 获取系统时间线(毫秒)
     *
     * 返回系统运行的总时间，单位为毫秒，精度到微秒
     *
     * @return float 系统运行时间，单位为毫秒
     */
    float dwt_get_time_line_ms(void);

    /**
     * @brief 获取系统时间线(微秒)
     *
     * 返回系统运行的总时间，单位为微秒
     *
     * @return uint32_t 系统运行时间，单位为微秒
     */
    uint32_t dwt_get_time_line_us(void);

    /**
     * @brief 延时函数(秒)
     *
     * 使用DWT计数器实现精确延时，单位为秒
     *
     * @param delay_time 延时时间，单位为秒
     */
    void dwt_delay_s(uint32_t delay_time);

    /**
     * @brief 延时函数(毫秒)
     *
     * 使用DWT计数器实现精确延时，单位为毫秒
     *
     * @param delay_time 延时时间，单位为毫秒
     */
    void dwt_delay_ms(uint32_t delay_time);

    /**
     * @brief 延时函数(微秒)
     *
     * 使用DWT计数器实现精确延时，单位为微秒
     *
     * @param delay_time 延时时间，单位为微秒
     */
    void dwt_delay_us(uint32_t delay_time);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DWT_H */

