//
// Created by honor on 25-7-27.
//

#include "bsp_dwt.h"
#include "cmsis_os.h"
#include "main.h"
// 定义系统时间结构体变量
static DwtTime sys_time;
// CPU频率相关变量：Hz、kHz、MHz单位下的频率值
static uint32_t cpu_freq_hz, cpu_freq_hz_ms, cpu_freq_hz_us;
// DWT计数器相关变量：
// cyccnt_rount_count: 记录计数器溢出次数
// cyccnt_last: 上次读取的计数值
static uint32_t cyccnt_rount_count, cyccnt_last;
// cyccnt64: 64位高精度计数器变量
static uint64_t cyccnt64;

/**
 * @brief 更新DWT计数器值
 *
 * 该函数用于更新CYCCNT计数器的状态，检测计数器是否溢出，
 * 并更新相应的计数器变量。使用bit_blocker防止函数重入。
 */
static void dwt_cnt_update(void)
{
    // 使用静态变量作为互斥标志，防止函数重入
    static volatile uint8_t bit_blocker = 0;
    if (!bit_blocker)
    {
        bit_blocker = 1;
        // 读取当前CYCCNT寄存器的值
        volatile uint32_t cnt_now = DWT->CYCCNT;
        // 如果当前值小于上次记录的值，说明计数器已溢出，增加溢出计数
        if (cnt_now < cyccnt_last)
        {
            cyccnt_rount_count++;
        }
        // 更新上次记录的计数值
        cyccnt_last = DWT->CYCCNT;
        bit_blocker = 0;
    }
}

/**
 * @brief 初始化DWT（Data Watchpoint and Trace）模块
 *
 * 使能DWT CYCCNT计数器，并根据CPU频率设置相关变量。
 *
 * @param cpu_freq_mhz CPU频率（MHz）
 */
void dwt_init(uint32_t cpu_freq_mhz)
{
    // 使能调试跟踪功能
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // 清零CYCCNT计数器
    DWT->CYCCNT = 0;
    // 使能CYCCNT计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    // 根据输入的MHz频率计算不同单位下的频率值
    cpu_freq_hz = cpu_freq_mhz * 1000000;     // Hz
    cpu_freq_hz_ms = cpu_freq_mhz * 1000;     // 每毫秒计数
    cpu_freq_hz_us = cpu_freq_mhz;            // 每微秒计数

    // 初始化计数器溢出计数为0
    cyccnt_rount_count = 0;
    // 更新计数器状态
    dwt_cnt_update();
}

/**
 * @brief 获取两个时间点之间的时间差(单精度浮点数)
 *
 * 计算从上次记录的时间点到当前时间点经过的时间，单位为秒
 *
 * @param cnt_last 指向记录上次计数值的指针
 * @return float 时间差，单位为秒
 */
float get_time_delta(uint32_t *cnt_last)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    float dt = ((uint32_t)(cnt_now-*cnt_last))/((float)(cpu_freq_hz));
    *cnt_last = cnt_now;
    dwt_cnt_update();
    return dt;
}

/**
 * @brief 获取两个时间点之间的时间差(双精度浮点数)
 *
 * 计算从上次记录的时间点到当前时间点经过的时间，单位为秒
 *
 * @param cnt_last 指向记录上次计数值的指针
 * @return double 时间差，单位为秒
 */
double get_time_delta64(uint32_t *cnt_last)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    double dt = ((uint32_t)(cnt_now-*cnt_last))/((double)(cpu_freq_hz));
    *cnt_last = cnt_now;
    dwt_cnt_update();
    return dt;
}

/**
 * @brief 更新系统时间
 *
 * 根据DWT计数器的值更新系统时间结构体，包括秒、毫秒和微秒
 */
void dwt_sys_time_update(void)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    static uint64_t cnt_temp1, cnt_temp2, cnt_temp3;
    dwt_cnt_update();
    // 构造64位计数值：溢出次数*最大值 + 当前计数值
    cyccnt64 = (uint64_t)cyccnt_rount_count * (uint64_t)UINT32_MAX + (uint64_t)cnt_now;
    // 计算秒数
    cnt_temp1 = cyccnt64 / cpu_freq_hz;
    cnt_temp2 = cyccnt64 - cnt_temp1 * cpu_freq_hz;
    sys_time.s = cnt_temp1;
    // 计算毫秒数
    sys_time.ms = cnt_temp2 / cpu_freq_hz_ms;
    cnt_temp3 = cnt_temp2 - cnt_temp3 * cpu_freq_hz_ms;
    // 计算微秒数
    sys_time.us = cnt_temp3 / cpu_freq_hz_us;
}

/**
 * @brief 获取系统时间线(秒)
 *
 * 返回系统运行的总时间，单位为秒，精度到微秒
 *
 * @return float 系统运行时间，单位为秒
 */
float dwt_get_time_line_s(void)
{
    dwt_sys_time_update();
    float dwt_time_line_f32 = sys_time.s + (float)sys_time.ms / 1000.0f + (float)sys_time.us / 1000000.0f;
    return dwt_time_line_f32;
}

/**
 * @brief 获取系统时间线(毫秒)
 *
 * 返回系统运行的总时间，单位为毫秒，精度到微秒
 *
 * @return float 系统运行时间，单位为毫秒
 */
float dwt_get_time_line_ms(void)
{
    dwt_sys_time_update();
    float dwt_time_line_f32 = sys_time.s * 1000.0f + sys_time.ms + (float)sys_time.us / 1000.0f;
    return dwt_time_line_f32;
}

/**
 * @brief 获取系统时间线(微秒)
 *
 * 返回系统运行的总时间，单位为微秒
 *
 * @return uint32_t 系统运行时间，单位为微秒
 */
uint32_t dwt_get_time_line_us(void)
{
    dwt_sys_time_update();
    uint64_t dwt_time_line_f64 = sys_time.s * 1000000 + sys_time.ms * 1000 + sys_time.us;
    return dwt_time_line_f64;
}

/**
 * @brief 延时函数(秒)
 *
 * 使用DWT计数器实现精确延时，单位为秒
 *
 * @param delay_time 延时时间，单位为秒
 */
void dwt_delay_s(uint32_t delay_time)
{
    uint32_t tick_start = DWT->CYCCNT;
    float wait_time = delay_time;
    while ((DWT->CYCCNT - tick_start) < wait_time * cpu_freq_hz);
}

/**
 * @brief 延时函数(毫秒)
 *
 * 使用DWT计数器实现精确延时，单位为毫秒
 *
 * @param delay_time 延时时间，单位为毫秒
 */
void dwt_delay_ms(uint32_t delay_time)
{
    uint32_t tick_start = DWT->CYCCNT;
    float wait_time = delay_time ;
    while ((DWT->CYCCNT - tick_start) < wait_time * cpu_freq_hz_ms);
}

/**
 * @brief 延时函数(微秒)
 *
 * 使用DWT计数器实现精确延时，单位为微秒
 *
 * @param delay_time 延时时间，单位为微秒
 */
void dwt_delay_us(uint32_t delay_time)
{
    uint32_t tick_start = DWT->CYCCNT;
    float wait_time = delay_time ;
    while ((DWT->CYCCNT - tick_start) < wait_time * cpu_freq_hz_us);
}
