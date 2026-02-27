/**
  ******************************************************************************
  * @file      startup_gd32f450_470.s
  * @author    Generated based on ARM startup file
  * @brief     GCC startup file for GD32F450/470
  ******************************************************************************
  */

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.global g_pfnVectors
.global Default_Handler

/* Initialization values */
.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

/**
  ******************************************************************************
  * @brief  This is the code that gets called when the processor first
  *         starts execution following a reset event.
  * @param  None
  * @retval : None
  */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
  ldr sp, =_estack

  /* Call the clock system initialization function */
  bl SystemInit

  /* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

  /* Zero fill the bss segment */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

  /* Call static constructors */
  bl __libc_init_array

  /* Call the application's entry point */
  bl main
  bx lr

.size Reset_Handler, .-Reset_Handler

/**
  ******************************************************************************
  * @brief  This is the code that gets called when the processor receives an
  *         unexpected interrupt.
  ******************************************************************************
  */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
.size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex M4.
*
*******************************************************************************/
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  /* External Interrupts */
  .word WWDGT_IRQHandler
  .word LVD_IRQHandler
  .word TAMPER_STAMP_IRQHandler
  .word RTC_WKUP_IRQHandler
  .word FMC_IRQHandler
  .word RCU_CTC_IRQHandler
  .word EXTI0_IRQHandler
  .word EXTI1_IRQHandler
  .word EXTI2_IRQHandler
  .word EXTI3_IRQHandler
  .word EXTI4_IRQHandler
  .word DMA0_Channel0_IRQHandler
  .word DMA0_Channel1_IRQHandler
  .word DMA0_Channel2_IRQHandler
  .word DMA0_Channel3_IRQHandler
  .word DMA0_Channel4_IRQHandler
  .word DMA0_Channel5_IRQHandler
  .word DMA0_Channel6_IRQHandler
  .word ADC_IRQHandler
  .word CAN0_TX_IRQHandler
  .word CAN0_RX0_IRQHandler
  .word CAN0_RX1_IRQHandler
  .word CAN0_EWMC_IRQHandler
  .word EXTI5_9_IRQHandler
  .word TIMER0_BRK_TIMER8_IRQHandler
  .word TIMER0_UP_TIMER9_IRQHandler
  .word TIMER0_TRG_CMT_TIMER10_IRQHandler
  .word TIMER0_Channel_IRQHandler
  .word TIMER1_IRQHandler
  .word TIMER2_IRQHandler
  .word TIMER3_IRQHandler
  .word I2C0_EV_IRQHandler
  .word I2C0_ER_IRQHandler
  .word I2C1_EV_IRQHandler
  .word I2C1_ER_IRQHandler
  .word SPI0_IRQHandler
  .word SPI1_IRQHandler
  .word USART0_IRQHandler
  .word USART1_IRQHandler
  .word USART2_IRQHandler
  .word EXTI10_15_IRQHandler
  .word RTC_Alarm_IRQHandler
  .word USBFS_WKUP_IRQHandler
  .word TIMER7_BRK_TIMER11_IRQHandler
  .word TIMER7_UP_TIMER12_IRQHandler
  .word TIMER7_TRG_CMT_TIMER13_IRQHandler
  .word TIMER7_Channel_IRQHandler
  .word DMA0_Channel7_IRQHandler
  .word EXMC_IRQHandler
  .word SDIO_IRQHandler
  .word TIMER4_IRQHandler
  .word SPI2_IRQHandler
  .word UART3_IRQHandler
  .word UART4_IRQHandler
  .word TIMER5_DAC_IRQHandler
  .word TIMER6_IRQHandler
  .word DMA1_Channel0_IRQHandler
  .word DMA1_Channel1_IRQHandler
  .word DMA1_Channel2_IRQHandler
  .word DMA1_Channel3_IRQHandler
  .word DMA1_Channel4_IRQHandler
  .word ENET_IRQHandler
  .word ENET_WKUP_IRQHandler
  .word CAN1_TX_IRQHandler
  .word CAN1_RX0_IRQHandler
  .word CAN1_RX1_IRQHandler
  .word CAN1_EWMC_IRQHandler
  .word USBFS_IRQHandler
  .word DMA1_Channel5_IRQHandler
  .word DMA1_Channel6_IRQHandler
  .word DMA1_Channel7_IRQHandler
  .word USART5_IRQHandler
  .word I2C2_EV_IRQHandler
  .word I2C2_ER_IRQHandler
  .word USBHS_EP1_Out_IRQHandler
  .word USBHS_EP1_In_IRQHandler
  .word USBHS_WKUP_IRQHandler
  .word USBHS_IRQHandler
  .word DCI_IRQHandler
  .word 0
  .word TRNG_IRQHandler
  .word FPU_IRQHandler
  .word UART6_IRQHandler
  .word UART7_IRQHandler
  .word SPI3_IRQHandler
  .word SPI4_IRQHandler
  .word SPI5_IRQHandler
  .word 0
  .word TLI_IRQHandler
  .word TLI_ER_IRQHandler
  .word IPA_IRQHandler

/* Exception Handlers */
.macro WEAK_HANDLER name
.weak \name
.set \name, Default_Handler
.endm

WEAK_HANDLER NMI_Handler
WEAK_HANDLER HardFault_Handler
WEAK_HANDLER MemManage_Handler
WEAK_HANDLER BusFault_Handler
WEAK_HANDLER UsageFault_Handler
WEAK_HANDLER SVC_Handler
WEAK_HANDLER DebugMon_Handler
WEAK_HANDLER PendSV_Handler
WEAK_HANDLER SysTick_Handler

WEAK_HANDLER WWDGT_IRQHandler
WEAK_HANDLER LVD_IRQHandler
WEAK_HANDLER TAMPER_STAMP_IRQHandler
WEAK_HANDLER RTC_WKUP_IRQHandler
WEAK_HANDLER FMC_IRQHandler
WEAK_HANDLER RCU_CTC_IRQHandler
WEAK_HANDLER EXTI0_IRQHandler
WEAK_HANDLER EXTI1_IRQHandler
WEAK_HANDLER EXTI2_IRQHandler
WEAK_HANDLER EXTI3_IRQHandler
WEAK_HANDLER EXTI4_IRQHandler
WEAK_HANDLER DMA0_Channel0_IRQHandler
WEAK_HANDLER DMA0_Channel1_IRQHandler
WEAK_HANDLER DMA0_Channel2_IRQHandler
WEAK_HANDLER DMA0_Channel3_IRQHandler
WEAK_HANDLER DMA0_Channel4_IRQHandler
WEAK_HANDLER DMA0_Channel5_IRQHandler
WEAK_HANDLER DMA0_Channel6_IRQHandler
WEAK_HANDLER ADC_IRQHandler
WEAK_HANDLER CAN0_TX_IRQHandler
WEAK_HANDLER CAN0_RX0_IRQHandler
WEAK_HANDLER CAN0_RX1_IRQHandler
WEAK_HANDLER CAN0_EWMC_IRQHandler
WEAK_HANDLER EXTI5_9_IRQHandler
WEAK_HANDLER TIMER0_BRK_TIMER8_IRQHandler
WEAK_HANDLER TIMER0_UP_TIMER9_IRQHandler
WEAK_HANDLER TIMER0_TRG_CMT_TIMER10_IRQHandler
WEAK_HANDLER TIMER0_Channel_IRQHandler
WEAK_HANDLER TIMER1_IRQHandler
WEAK_HANDLER TIMER2_IRQHandler
WEAK_HANDLER TIMER3_IRQHandler
WEAK_HANDLER I2C0_EV_IRQHandler
WEAK_HANDLER I2C0_ER_IRQHandler
WEAK_HANDLER I2C1_EV_IRQHandler
WEAK_HANDLER I2C1_ER_IRQHandler
WEAK_HANDLER SPI0_IRQHandler
WEAK_HANDLER SPI1_IRQHandler
WEAK_HANDLER USART0_IRQHandler
WEAK_HANDLER USART1_IRQHandler
WEAK_HANDLER USART2_IRQHandler
WEAK_HANDLER EXTI10_15_IRQHandler
WEAK_HANDLER RTC_Alarm_IRQHandler
WEAK_HANDLER USBFS_WKUP_IRQHandler
WEAK_HANDLER TIMER7_BRK_TIMER11_IRQHandler
WEAK_HANDLER TIMER7_UP_TIMER12_IRQHandler
WEAK_HANDLER TIMER7_TRG_CMT_TIMER13_IRQHandler
WEAK_HANDLER TIMER7_Channel_IRQHandler
WEAK_HANDLER DMA0_Channel7_IRQHandler
WEAK_HANDLER EXMC_IRQHandler
WEAK_HANDLER SDIO_IRQHandler
WEAK_HANDLER TIMER4_IRQHandler
WEAK_HANDLER SPI2_IRQHandler
WEAK_HANDLER UART3_IRQHandler
WEAK_HANDLER UART4_IRQHandler
WEAK_HANDLER TIMER5_DAC_IRQHandler
WEAK_HANDLER TIMER6_IRQHandler
WEAK_HANDLER DMA1_Channel0_IRQHandler
WEAK_HANDLER DMA1_Channel1_IRQHandler
WEAK_HANDLER DMA1_Channel2_IRQHandler
WEAK_HANDLER DMA1_Channel3_IRQHandler
WEAK_HANDLER DMA1_Channel4_IRQHandler
WEAK_HANDLER ENET_IRQHandler
WEAK_HANDLER ENET_WKUP_IRQHandler
WEAK_HANDLER CAN1_TX_IRQHandler
WEAK_HANDLER CAN1_RX0_IRQHandler
WEAK_HANDLER CAN1_RX1_IRQHandler
WEAK_HANDLER CAN1_EWMC_IRQHandler
WEAK_HANDLER USBFS_IRQHandler
WEAK_HANDLER DMA1_Channel5_IRQHandler
WEAK_HANDLER DMA1_Channel6_IRQHandler
WEAK_HANDLER DMA1_Channel7_IRQHandler
WEAK_HANDLER USART5_IRQHandler
WEAK_HANDLER I2C2_EV_IRQHandler
WEAK_HANDLER I2C2_ER_IRQHandler
WEAK_HANDLER USBHS_EP1_Out_IRQHandler
WEAK_HANDLER USBHS_EP1_In_IRQHandler
WEAK_HANDLER USBHS_WKUP_IRQHandler
WEAK_HANDLER USBHS_IRQHandler
WEAK_HANDLER DCI_IRQHandler
WEAK_HANDLER TRNG_IRQHandler
WEAK_HANDLER FPU_IRQHandler
WEAK_HANDLER UART6_IRQHandler
WEAK_HANDLER UART7_IRQHandler
WEAK_HANDLER SPI3_IRQHandler
WEAK_HANDLER SPI4_IRQHandler
WEAK_HANDLER SPI5_IRQHandler
WEAK_HANDLER TLI_IRQHandler
WEAK_HANDLER TLI_ER_IRQHandler
WEAK_HANDLER IPA_IRQHandler
