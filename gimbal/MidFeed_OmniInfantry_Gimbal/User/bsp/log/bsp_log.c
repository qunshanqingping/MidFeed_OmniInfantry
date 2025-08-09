#include <cmsis_os.h>
  #include "bsp_log.h"

/**
 * @brief 获取当前系统时间戳（毫秒）
 * @return 当前系统时间戳（毫秒）
 */
  uint32_t GetCurrentTime(void)
  {
      TickType_t ticks = xTaskGetTickCount();
      uint32_t milliseconds = ticks * portTICK_PERIOD_MS;
      return milliseconds;
  }

  /**
   * @brief 日志系统初始化
   */
  void log_init(void)
  {
      SEGGER_RTT_Init();
  }