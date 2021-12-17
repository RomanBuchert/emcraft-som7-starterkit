/*
 * gt911.h
 *
 *  Created on: 06.12.2021
 *      Author: RBuchert
 */

#ifndef LCD_TEST_GT911_H_
#define LCD_TEST_GT911_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define GD911_I2C_7BIT_ADDR1	(0x5D)
#define GD911_I2C_7BIT_ADDR2	(0x14)

struct gt911_config_struct{
	uint8_t ConfigVersion;						//0x8047
	uint16_t XOutputMax;						//0x8048 (LB) / 0x8049 (HB)
	uint16_t YOutputMax;						//0x804A (LB) / 0x804B (HB)
	uint8_t TouchNumber;						//0x804C
	uint8_t ModuleSwitch1;						//0x804D
	uint8_t ModuleSwitch2;						//0x804E
	uint8_t Shake_Count;						//0x804F
	uint8_t Filter;								//0x8050
	uint8_t LargeTouch;							//0x8051
	uint8_t NoiseReduction;						//0x8052
	uint8_t ScreenTouchLevel;					//0x8053
	uint8_t ScreenLeaveLevel;					//0x8054
	uint8_t LowPowerControl;					//0x8055
	uint8_t RefreshRate;						//0x8056
	uint8_t XThreshold;							//0x8057
	uint8_t YThreshold;							//0x8058
	uint8_t XSpeedLimit;						//0x8059
	uint8_t YSpeedLimit;						//0x805A
	uint8_t SpaceTopBottom;						//0x805B
	uint8_t SpaceLeftRight;						//0x805C
	uint8_t MiniFilter;							//0x805D
	uint8_t StretchR0;							//0x805E
	uint8_t StretchR1;							//0x805F
	uint8_t StretchR2;							//0x8060
	uint8_t StretchRM;							//0x8061
	uint8_t DrvGroupANum;						//0x8062
	uint8_t DrvGroupBNum;						//0x8063;
	uint8_t SensorNum;							//0x8064;
	uint8_t FreqAFactor;						//0x8065
	uint8_t FreqBFactor;						//0x8066
	uint16_t PannelBitFreq;						//0x8067 (LB) / 0x8068 (HB)
	uint16_t PannelSensorTime;					//0x8069 (LB) / 0x806A (HB)
	uint8_t PannelTxGain;						//0x806B
	uint8_t PannelRxGain;						//0x806C
	uint8_t PannelDumpShift;					//0x806D
	uint8_t DrvFrameControl;					//0x806E
	uint8_t ChargingLevelUp;					//0x806F
	uint8_t ModuleSwitch3;						//0x8070
	uint8_t GestureDis;							//0x8071
	uint8_t GestureLongPressTime;				//0x8072
	uint8_t XYSlopeAdjust;						//0x8073
	uint8_t GestureControl;						//0x8074
	uint8_t GestureSwitch1;						//0x8075
	uint8_t GestureSwitch2;						//0x8076
	uint8_t GestureRefreshRate;					//0x8077
	uint8_t GestureTouchLevel;					//0x8078
	uint8_t NewGreenWakeUpLevel;				//0x8079
	uint8_t FreqHoppingStart;					//0x807A
	uint8_t FreqHoppingEnd;						//0x807B
	uint8_t NoiseDetectTime;					//0x807C
	uint8_t HoppingFlag;						//0x807D
	uint8_t HoppingThreshold;					//0x807E
	uint8_t NoiseThreshold;						//0x807F
	uint8_t NoiseMinThreshold;					//0x8080
	uint8_t Res0x8081;
	uint8_t HoppingSensorGroup;					//0x8082
	uint8_t HoppingSeg1Normalize;				//0x8083
	uint8_t HoppingSeg1Factor;					//0x8084
	uint8_t MainClockAdjust;					//0x8085
	uint8_t HoppingSeg2Normalize;				//0x8086
	uint8_t HoppingSeg2Factor;					//0x8087
	uint8_t Res0x8088;
	uint8_t HoppingSeg3Normalize;				//0x8089
	uint8_t HoppingSeg3Factor;					//0x808A
	uint8_t Res0x808B;
	uint8_t HoppingSeg4Normalize;				//0x808C
	uint8_t HoppingSeg4Factor;					//0x808D
	uint8_t Res0x808E;
	uint8_t HoppingSeg5Normalize;				//0x808F
	uint8_t HoppingSeg5Factor;					//0x8090
	uint8_t Res0x8091;
	uint8_t HoppingSeg6Normalize;				//0x8092
	uint8_t Key[4];								//0x8093...0x8096 / Key1...Key4
	uint8_t KeyArea;							//0x8097
	uint8_t KeyTouchLevel;						//0x8088
	uint8_t KeyLeaveLevel;						//0x8089
	uint16_t KeySens;							//0x809A (Key1/Key2) / 0x809B (Key3/Key4)
	uint8_t KeyRestrain;						//0x809C
	uint8_t KeyRestrainTime;					//0x809D
	uint8_t GestureLargeTouch;					//0x809E
	uint8_t Res0x809F;
	uint8_t Res0x80A0;
	uint8_t HotknotNoiseMap;					//0x80A1
	uint8_t LinkThreshold;						//0x80A2
	uint8_t PxyThreshold;						//0x80A3
	uint8_t GHotDumpShift;						//0x80A4
	uint8_t GHotRxGain;							//0x80A5
	uint8_t FreqGain[4];						//0x80A6...0x80A9 / FreqGain0...FreqGain3
	uint8_t Res0x80AA_0x80B2[9];
	uint8_t CombineDis;							//0x80B3
	uint8_t SplitSet;							//0x80B4
	uint8_t Res0x80B5;
	uint8_t Res0x80B6;
	uint8_t SensorCh[14];						//0x80B7...0x80C4 / Ch0...Ch13
	uint8_t Res0x80C5_0x80D4[16];
	uint8_t DriverCh[26];						//0x80D5...0x80EE / Ch0...Ch25
	uint8_t Res0x80EF_0x80FE[16];
	uint8_t ConfigChecksum;						//0x80FF
	uint8_t ConfigFresh;						//0x8100
} __attribute__((packed));

struct gt911_info_struct{
	uint32_t ProductID;
	uint16_t FwVersion;
	uint16_t XResolution;
	uint16_t YResolution;
	uint8_t VendorId;
} __attribute__((packed));

struct gt911_touchpoint_struct {
	uint16_t X;
	uint16_t Y;
	uint16_t Size;
	uint8_t Reserved;
	uint8_t TrackId;
} __attribute__((packed));

struct gt911_positions_struct{
	union {
		uint8_t Value;
		struct {
			uint8_t Touchpoints		:4;
			uint8_t HaveKey			:1;
			uint8_t ProximityValid	:1;
			uint8_t LargeDetect		:1;
			uint8_t BufferStatus	:1;
		};
	}Info ;
	uint8_t	TrackId;
	struct gt911_touchpoint_struct Point[5];
} __attribute__((packed));

void gt911_init();
void gt911_reset(bool state);
void gt911_change_addr(uint8_t new_addr);
int gt911_read(uint16_t reg, void* data, size_t size);
int gt911_write(uint16_t reg, void* data, size_t size);
uint8_t gt911_calc_config_checksum(struct gt911_config_struct* config);
void gt911_update_config(struct gt911_config_struct* config);
void gt911_debug();
#endif /* LCD_TEST_GT911_H_ */
