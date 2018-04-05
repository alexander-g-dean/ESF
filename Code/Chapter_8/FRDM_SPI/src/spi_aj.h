#include <MKL25Z4.H>
#include "user_defs.h"


#define SPI_CS_POS		4
#define SPI_SCK_POS		2
#define SPI_MOSI_POS	1
#define SPI_MISO_POS	3

extern uint8_t SPI1_SendingData;
extern uint8_t SPI1_ReceivingData;
extern uint8_t alpha;

// Define which SPI peripherals are enabled
#define LCD1_EN  1
#define SD_EN    0
#define PMOD1_EN 0
#define PMOD2_EN 0
#define WIFI_EN  1

typedef enum SPI_perif {
   SPI_LCD = 0,
   SPI_SD,
   SPI_PMOD1,
   SPI_PMOD2,
   SPI_WIFI,
} SPI_perif_t;

#define LCD_CS      0

#define WIFI_CS     4

extern void Init_SPI1(void);
extern void SPI1_Send(SPI_perif_t aDevice, uint8_t *aData, uint32_t aLength);
extern void SPI1_SendReceive(SPI_perif_t aDevice, uint8_t *aTXData, uint32_t aTXLength, uint8_t *aRXData);
extern void SPI1_callback_receiveend(void);
extern void SPI1_callback_sendend(void);
extern void SPI1_callback_error(uint8_t err_type);
extern uint8_t test_SPIsend(uint8_t);
extern void SPI1_SendReceive(SPI_perif_t aDevice, uint8_t *aTXData, uint32_t aLength,uint8_t *aRXData);
extern void SPI_CS_Start(SPI_perif_t aDevice);
extern void SPI_CS_End(SPI_perif_t aDevice);



