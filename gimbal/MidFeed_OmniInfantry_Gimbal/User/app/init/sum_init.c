/*
 * @file sum_init.c
 * @brief Sum initialization includes application initialization,
 * module initialization,bsp initialization
 * @author Adonis Jin
 * @date 2021-09-07
 * @version 1.0.0
 */

#include "sum_init.h"

#include <usart.h>

#include "basic_math.h"

#include "bsp_dwt.h"
#include "bsp_log.h"
#include "bsp_fdcan.h"
#include "dbus.h"
#include "cmsis_os.h"
DbusInstance_s* dbus_instance;
static DbusInstance_s* Dbus_Init(void);
static void test_can(void);
/**
 * @brief Initializes the Board Support Package (BSP)
 * @todo repair the log initialization function
 */
static void Bsp_Init(void)
{
    /* Initialize the BSP */
    Dwt_Init();
    Log_Init();
}

/**
 * @brief Initializes the module library.
 * @details This function initializes the DBus communication. If the initialization fails, it logs an error message.
 */
static void Module_Init(void) {
    dbus_instance = Dbus_Init();
    if (dbus_instance == NULL) {
        Log_Error("DBUS initialization failed");
    }
    test_can();
}

/**
 * @brief Initializes the application
 *
 * This function is responsible for performing any necessary initialization steps for the application.
 * It is part of the overall initialization process, which includes initializing the Board Support Package (BSP),
 * the module library, and finally the application itself.
 */
static void App_Init(void)
{
    /* Initialize the application */
}

/**
 * @brief Initializes the system by sequentially initializing the Board Support Package (BSP), module library, and application.
 * This function serves as a starting point for the initialization process, ensuring that all necessary components are set up before the main application logic begins.
 */
void Sum_Init(void)
{
    /* Initialize the BSP */
    Bsp_Init();

    /* Initialize the module library */
    Module_Init();

    /* Initialize the application */
    App_Init();
}

/**
 * @brief Dbus initialization function
 * @details This function initializes the DBUS module by allocating memory for the configuration
 * @return Pointer to the initialized DbusInstance_s structure, or NULL if initialization failed
 * @note It sets up the UART configuration for DBUS communication, registers the instance,
 */
static DbusInstance_s* Dbus_Init(void)
{
    /* Initialize the DBUS */
    DbusConfig_s *dbus_config = user_malloc(sizeof(DbusConfig_s));
    if (dbus_config == NULL)
    {
        Log_Error("DBUS config allocation failed");
        return NULL;
    }
    memset(dbus_config, 0, sizeof(DbusConfig_s));
    dbus_config->uart_config.uart_handle = &huart5; // Use UART5 for DBUS
    dbus_config->uart_config.transfer_mode = DMA_MODE; // Set transfer mode
    dbus_config->uart_config.direction_mode = RX_MODE; // Set direction mode
    dbus_config->uart_config.buffer_mode = DOUBLE_BUFFER_MODE; // Use double buffer mode
    dbus_config->uart_config.rx_len = 18; // Set receive length
    dbus_config->uart_config.uart_module_callback = Dbus_RxCallback; // Set the callback function for UART reception
    DbusInstance_s *dbus_instance = (DbusInstance_s*)user_malloc(sizeof(DbusInstance_s));
    if (dbus_instance == NULL)
    {
        Log_Error("DBUS instance registration failed");
        user_free(dbus_config);
        return NULL;
    }
    memset(dbus_instance, 0, sizeof(DbusInstance_s));
    dbus_instance = Dbus_Register(dbus_config);
    user_free(dbus_config);
    return dbus_instance;
}

CanInstance_s *test;
CanInitConfig_s test_config = {
    .topic_name = "test",
    .can_handle = &hfdcan1,
    .tx_id = 0x200,
    .rx_id = 0x1FF,
    .can_module_callback = NULL,
    .id = NULL
};

static void test_can(void)
{
    test = Can_Register(&test_config);
    if (test == NULL)
    {
        Log_Error("CAN Register Failed");
    }
    else
    {
        Log_Passing("CAN Register Success");
    }
}
HAL_StatusTypeDef test_status;
void CAN_Task(void const * argument)
{
    /* USER CODE BEGIN CAN_Task */
    /* Infinite loop */
    for(;;)
    {
        test_status = Can_Transmit(test);
        osDelay(2000);
    }
    /* USER CODE END CAN_Task */
}