#include <stdint.h>
#include <stdio.h>
#include <twi.h>

/*********** RDA5807m Hardware - register defines *************/

/* Fixed address of the RDA5807m chip*/
#define RDA5807M_I2C_ADR	    (uint8_t)(0x10)
#define RDA5807M_REG_SIZE       (uint8_t)(2u)

/* Only write marked register without real write possibility */
#define CHIP_NAME_ADR 		    (uint8_t)(0x00)

/**************************************************************/
#define REG_ADR_02		 	    (uint8_t)(0x02)

#define DHIZ				    (uint16_t)(15)
#define DMUTE				    (uint16_t)(14)
#define MONO				    (uint16_t)(13)
#define BASS				    (uint16_t)(12)
#define RCLK_N_CAL_MODE		    (uint16_t)(11)
#define RCLK_DIR_MODE		    (uint16_t)(10)
#define SEEKUP				    (uint16_t)(9)
#define SEEK				    (uint16_t)(8)
#define SKMODE				    (uint16_t)(7)
#define CLKMODE_2			    (uint16_t)(6)
#define CLKMODE_1			    (uint16_t)(5)
#define CLKMODE_0			    (uint16_t)(4)
#define RDS_EN				    (uint16_t)(3)
#define NEW_METHOD			    (uint16_t)(2)
#define SOFT_RESET			    (uint16_t)(1)
#define ENABLE				    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_03			    (uint8_t)0x03

#define CHAN_9				    (uint16_t)(15)
#define CHAN_8				    (uint16_t)(14)
#define CHAN_7				    (uint16_t)(13)
#define CHAN_6				    (uint16_t)(12)
#define CHAN_5				    (uint16_t)(11)
#define CHAN_4				    (uint16_t)(10)
#define CHAN_3				    (uint16_t)(9)
#define CHAN_2				    (uint16_t)(8)
#define CHAN_1				    (uint16_t)(7)
#define CHAN_0				    (uint16_t)(6)
#define DIRECT_MODE			    (uint16_t)(5)
#define TUNE				    (uint16_t)(4)
#define BAND_1				    (uint16_t)(3)
#define BAND_0				    (uint16_t)(2)
#define SPACE_1				    (uint16_t)(1)
#define SPACE_0				    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_04			    (uint8_t)(0x04)

#define RSVD_04_1			    (uint16_t)(15)
#define STCIEN				    (uint16_t)(14)
#define RBDS				    (uint16_t)(13)
#define RDS_FIFO_EN			    (uint16_t)(12)
#define DE					    (uint16_t)(11)
#define RDS_FIFO_CLR		    (uint16_t)(10)
#define SOFTMUTE_EN			    (uint16_t)(9)
#define AFCD				    (uint16_t)(8)
#define RSVD_04_2			    (uint16_t)(7)
#define I2S_ENABLE			    (uint16_t)(6)
#define GPIO3_1				    (uint16_t)(5)
#define GPIO3_0				    (uint16_t)(4)
#define GPIO2_1				    (uint16_t)(3)
#define GPIO2_0				    (uint16_t)(2)
#define GPIO1_1				    (uint16_t)(1)
#define GPIO1_0				    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_05			    (uint8_t)(0x05)

#define INT_MODE			    (uint16_t)(15)
#define SEEK_MODE_1			    (uint16_t)(14)
#define SEEK_MODE_0			    (uint16_t)(13)
#define RSVD_05_1			    (uint16_t)(12)
#define SEEKTH_3			    (uint16_t)(11)
#define SEEKTH_2			    (uint16_t)(10)
#define SEEKTH_1			    (uint16_t)(9)
#define SEEKTH_0			    (uint16_t)(8)
#define LNA_PORT_SEL_1		    (uint16_t)(7)
#define LNA_PORT_SEL_0		    (uint16_t)(6)
#define LNA_ICSEL_1			    (uint16_t)(5)
#define LNA_ICSEL_0			    (uint16_t)(4)
#define VOLUME_3			    (uint16_t)(3)
#define VOLUME_2			    (uint16_t)(2)
#define VOLUME_1			    (uint16_t)(1)
#define VOLUME_0			    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_06			    (uint8_t)(0x06)

#define RSVD_06_1			    (uint16_t)(15)
#define OPEN_MODE_1			    (uint16_t)(14)
#define OPEN_MODE_0			    (uint16_t)(13)
#define SLAVE_MASTER		    (uint16_t)(12)
#define WS_LR				    (uint16_t)(11)
#define SCLK_I_EDGE			    (uint16_t)(10)
#define DATA_SIGNED			    (uint16_t)(9)
#define WS_I_EDGE			    (uint16_t)(8)
#define I2S_SW_CNT_3		    (uint16_t)(7)
#define I2S_SW_CNT_2		    (uint16_t)(6)
#define I2S_SW_CNT_1		    (uint16_t)(5)
#define I2S_SW_CNT_0		    (uint16_t)(4)
#define SW_O_EDGE			    (uint16_t)(3)
#define SCLK_O_EDGE			    (uint16_t)(2)
#define L_DELY				    (uint16_t)(1)
#define R_DELY				    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_07			    (uint8_t)(0x07)

#define RSVD_07_1			    (uint16_t)(15)
#define TH_SOFRBLEND_4		    (uint16_t)(14)
#define TH_SOFRBLEND_3		    (uint16_t)(13)
#define TH_SOFRBLEND_2		    (uint16_t)(12)
#define TH_SOFRBLEND_1		    (uint16_t)(11)
#define TH_SOFRBLEND_0		    (uint16_t)(10)
#define MODE_65M_50M		    (uint16_t)(9)
#define RSVD_07_2			    (uint16_t)(8)
#define SEEK_TH_OLD_5		    (uint16_t)(7)
#define SEEK_TH_OLD_4		    (uint16_t)(6)
#define SEEK_TH_OLD_3		    (uint16_t)(5)
#define SEEK_TH_OLD_2		    (uint16_t)(4)
#define SEEK_TH_OLD_1		    (uint16_t)(3)
#define SEEK_TH_OLD_0		    (uint16_t)(2)
#define SOFTBLEND_EN		    (uint16_t)(1)
#define FREQ_MODE			    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_08			    (uint8_t)(0x08)

#define FREQ_DIRECT_15		    (uint16_t)(15)
#define FREQ_DIRECT_14		    (uint16_t)(14)
#define FREQ_DIRECT_13		    (uint16_t)(13)
#define FREQ_DIRECT_12		    (uint16_t)(12)
#define FREQ_DIRECT_11		    (uint16_t)(11)
#define FREQ_DIRECT_10		    (uint16_t)(10)
#define FREQ_DIRECT_9		    (uint16_t)(9)
#define FREQ_DIRECT_8		    (uint16_t)(8)
#define FREQ_DIRECT_7		    (uint16_t)(7)
#define FREQ_DIRECT_6		    (uint16_t)(6)
#define FREQ_DIRECT_5		    (uint16_t)(5)
#define FREQ_DIRECT_4		    (uint16_t)(4)
#define FREQ_DIRECT_3		    (uint16_t)(3)
#define FREQ_DIRECT_2		    (uint16_t)(2)
#define FREQ_DIRECT_1		    (uint16_t)(1)
#define FREQ_DIRECT_0		    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_0A			    (uint8_t)(0x0A)

#define RDSR				    (uint16_t)(15)
#define STC					    (uint16_t)(14)
#define SF					    (uint16_t)(13)
#define RDSS				    (uint16_t)(12)
#define BLK_E				    (uint16_t)(11)
#define ST					    (uint16_t)(10)
#define READCHAN_9			    (uint16_t)(9)
#define READCHAN_8			    (uint16_t)(8)
#define READCHAN_7			    (uint16_t)(7)
#define READCHAN_6			    (uint16_t)(6)
#define READCHAN_5			    (uint16_t)(5)
#define READCHAN_4			    (uint16_t)(4)
#define READCHAN_3			    (uint16_t)(3)
#define READCHAN_2			    (uint16_t)(2)
#define READCHAN_1			    (uint16_t)(1)
#define READCHAN_0			    (uint16_t)(0)
/**************************************************************/

#define REG_ADR_0B			    (uint8_t)(0x0B)

#define RSSI_6				    (uint16_t)(15)
#define RSSI_5				    (uint16_t)(14)
#define RSSI_4				    (uint16_t)(13)
#define RSSI_3				    (uint16_t)(12)
#define RSSI_2				    (uint16_t)(11)
#define RSSI_1				    (uint16_t)(10)
#define RSSI_0				    (uint16_t)(9)
#define FM_TRUE				    (uint16_t)(8)
#define FM_READY			    (uint16_t)(7)
#define RSVD_0B_1			    (uint16_t)(6)
#define RSVD_0B_2			    (uint16_t)(5)
#define ABCD_E				    (uint16_t)(4)
#define BLERA_1				    (uint16_t)(3)
#define BLERA_0				    (uint16_t)(2)
#define BLERB_1				    (uint16_t)(1)
#define BLERB_0				    (uint16_t)(0)
/**************************************************************/

#define REG_RDSA			    (uint8_t)(0x0C)
/**************************************************************/
#define REG_RDSB			    (uint8_t)(0x0D)
/**************************************************************/
#define REG_RDSC			    (uint8_t)(0x0E)
/**************************************************************/
#define REG_RDSD			    (uint8_t)(0x0F)
/**************************************************************/


/**********************Frequency bands*************************/
#define US_EUROPE_BW		    (uint16_t)(87000)
#define JAPAN_BW			    (uint16_t)(76000)
#define WORLDWIDE_BW		    (uint16_t)(76000)
#define EAST_EUROPE_BW		    (uint16_t)(65000)

/*******************User-input limits**************************/
#define RDA5807mWW_FREQ_MIN    ((uint16_t)(7600))
#define RDA5807mWW_FREQ_MAX    ((uint16_t)(10800))

#define RDA5807mVOLUME_MAX     ((uint8_t)(15))
#define RDA5807mVOLUME_MIN     ((uint8_t)(0))

/***********RDA5807m Module - memory allocation ***************/
#define RDA5807mWR_REG_LNG     ((uint8_t)(7))
#define RDA5807mRD_REG_LNG     ((uint8_t)(6))

/***********RDA5807m Module - function returns ****************/
#define RDA5807M_FN_OK		    ((uint16_t)(0x0))
#define RDA5807M_FN_ER		    ((uint16_t)(0xFFFF))
#define RDA5807mNO_DATA         ((uint16_t)(0x0))

/***************RDA5807m Module - public macros ***************/
#define rda5807mPackData(result, data) (uint32_t)(((uint32_t)(result) << 16) | (uint32_t)(data))
#define rda5807mUnPackReturn(result)   (uint16_t)(((result) & 0xFFFF0000) >> 16)
#define rda5807mUnPackData(result)     (uint16_t)((result) & 0x0000FFFF)

/**************** Public functions: Basic operation ***********/
void RDA5807mInit(void);
uint16_t RDA5807mSetFreq(uint16_t freq);
uint16_t RDA5807mSetVolm(uint8_t volume);
void RDA5807mReset(void);
uint16_t RDA5807mMute(uint8_t mute);
uint16_t RDA5807mGetRSSI(void);

/**************** Public functions: Basic operation ***********/

uint16_t RDA5807mGetRDSR(void);
uint16_t RDA5807mGetRDSS(void);
uint16_t RDA5807mIsChannelStereo(void);
uint16_t RDA5807mIsChannelStation(void);
uint16_t RDA5807mGetRDSBlockA(void);
uint16_t RDA5807mGetRDSBlockB(void);
uint16_t RDA5807mGetRDSBlockC(void);
uint16_t RDA5807mGetRDSBlockD(void);

uint16_t RDA5807mGetErrBlockA(void);
uint16_t RDA5807mGetErrBlockB(void);

void RDA5807mClearRDSFIFO(void);