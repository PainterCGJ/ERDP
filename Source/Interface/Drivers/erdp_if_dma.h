#ifndef __ERDP_IF_DMA_H__
#define __ERDP_IF_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif    // __cplusplus
#include "erdp_if_spi.h"
#include "erdp_interface.h"

    typedef enum {
        ERDP_DMA0 = 0,
        ERDP_DMA1,
        ERDP_DMA2,
        ERDP_DMA_NUM,
    } ERDP_Dma_t;
    typedef enum {
        ERDP_DMA_STREAM0 = 0,
        ERDP_DMA_STREAM1,
        ERDP_DMA_STREAM2,
        ERDP_DMA_STREAM3,
        ERDP_DMA_STREAM4,
        ERDP_DMA_STREAM5,
        ERDP_DMA_STREAM6,
        ERDP_DMA_STREAM7,
        ERDP_DMA_STREAM_NUM,
    } ERDP_DmaStream_t;

    typedef enum {
        ERDP_DMA_CHANNEL0 = 0,
        ERDP_DMA_CHANNEL1,
        ERDP_DMA_CHANNEL2,
        ERDP_DMA_CHANNEL3,
        ERDP_DMA_CHANNEL4,
        ERDP_DMA_CHANNEL5,
        ERDP_DMA_CHANNEL6,
        ERDP_DMA_CHANNEL7,
        ERDP_DMA_CHANNEL_NUM,
    } ERDP_DmaChannel_t;

    typedef enum {
        ERDP_DMA_PRIORITY_LOW = 0,
        ERDP_DMA_PRIORITY_MEDIUM,
        ERDP_DMA_PRIORITY_HIGH,
        ERDP_DMA_PRIORITY_MAX,
    } ERDP_StreamPriority_t;

    typedef enum {
        ERDP_DMA_DIR_M2P = 0,    // Memory to Peripheral
        ERDP_DMA_DIR_P2M,        // Peripheral to Memory
        ERDP_DMA_DIR_M2M,        // Memory to Memory
    } ERDP_DmaDir_t;

    typedef struct {
        ERDP_Dma_t dma;
        ERDP_DmaStream_t stream;
        ERDP_DmaChannel_t channel;
        ERDP_DmaDir_t dir;
        uint32_t dst_addr;
        ERDP_StreamPriority_t stream_priority;
        bool enable_tc_irq;
        uint8_t irq_priority;
    } ERDP_DmaCfg_t;

    /**
     * @brief Initialize DMA
     * @param[in]  cfg DMA configuration structure
     */
    void erdp_if_dma_init(ERDP_DmaCfg_t cfg);

    /**
     * @brief DMA transfer data to SPI
     * @param[in]  dma DMA number
     * @param[in]  stream DMA stream
     * @param[in]  spi SPI number
     * @param[in]  data data to transfer
     * @param[in]  len data length
     */
    void erdp_if_dma_spi_transfer(ERDP_Dma_t dma, ERDP_DmaStream_t stream, ERDP_Spi_t spi, const uint8_t *data,
                                  uint32_t len);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif
