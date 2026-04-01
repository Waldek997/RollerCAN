/*
 * RollerCAN.h
 *
 *  Created on: Mar 10, 2026
 *      Author: nicni
 */

#ifndef INC_ROLLERCAN_H_
#define INC_ROLLERCAN_H_

#include <stdbool.h>

typedef struct
{
	uint32_t can_id;
	uint32_t cmd_id;
	uint32_t option;
	uint8_t Data[4];
	uint32_t LastValue;

    bool init_flag;
    bool data_ready;
    bool tx_ready;

    void (*RollerCAN_SendCallback_t)(uint32_t id, uint8_t* data);
    void (*RollerCAN_UpdateFilter_t)(uint32_t address);

}RollerCAN_t;

typedef enum
{
    ROLLERCAN_OK = 0,
    ROLLERCAN_ERROR = 1,

} RollerCAN_Status;

typedef enum
{
    ROLLER_MODE_SPEED = 1,  // Speed mode
    ROLLER_MODE_POSITION,   // Position mode(2)
    ROLLER_MODE_CURRENT,    // Current mode (3)
    ROLLER_MODE_ENCODER     // Encoder mode (4)

} roller_mode_t;

RollerCAN_Status RollerCAN_Init(RollerCAN_t* R, uint32_t can_id, void *CallbackSend, void *CallbackFilter);
void RollerCAN_ReadFrame(RollerCAN_t* R, uint32_t identifier, uint8_t* data);
void RollerCAN_TxComplete(RollerCAN_t* R);
void RollerCAN_UnProtection(RollerCAN_t* R);
void RollerCAN_OnOff_Protection(RollerCAN_t* R, bool OnOff);
void RollerCAN_SaveParametersToFlash(RollerCAN_t* R);
void RollerCAN_OnOff(RollerCAN_t* R, bool OnOff);

void RollerCAN_Set_RGB_Mode(RollerCAN_t* R);
void RollerCAN_SetColorLED(RollerCAN_t* R, uint8_t Red, uint8_t Green, uint8_t Blue);
void RollerCAN_SetBrightessLED(RollerCAN_t* R, uint8_t BrightessLED);

void RollerCAN_SetMode(RollerCAN_t* R, roller_mode_t mode);
void RollerCAN_StartStop(RollerCAN_t* R, bool OnOff);

void RollerCAN_SetSpeedMotor(RollerCAN_t* R, uint32_t Speed);
void RollerCAN_SetSpeedCurrent(RollerCAN_t* R, uint32_t SpeedCurrent);
void RollerCAN_SetSpeed_kP(RollerCAN_t* R, float Set_kP);
void RollerCAN_SetSpeed_kI(RollerCAN_t* R, float Set_kI);
void RollerCAN_SetSpeed_kD(RollerCAN_t* R, float Set_kD);

void RollerCAN_SetPositionMotor(RollerCAN_t* R, uint32_t Position);
void RollerCAN_SetPositionCurrent(RollerCAN_t* R, uint32_t PositionCurrent);
void RollerCAN_SetPosition_kP(RollerCAN_t* R, float Set_kP);
void RollerCAN_SetPosition_kI(RollerCAN_t* R, float Set_kI);
void RollerCAN_SetPosition_kD(RollerCAN_t* R, float Set_kD);

void RollerCAN_SetCurrent(RollerCAN_t* R, uint32_t Current);

void RollerCAN_OnOff_Protection(RollerCAN_t* R, bool OnOff);

uint8_t RollerCAN_Read_RGB_Mode(RollerCAN_t* R);
void RollerCAN_Read_ColorLED(RollerCAN_t* R, uint32_t *rgbValues);
uint8_t RollerCAN_ReadBrightessLED(RollerCAN_t* R);

uint32_t RollerCAN_ReadTemp(RollerCAN_t* R);
uint32_t RollerCAN_ReadActualVoltageInput(RollerCAN_t* R);
uint32_t RollerCAN_ReadActualEncoder(RollerCAN_t* R);
uint32_t RollerCAN_ReadActualCurrent(RollerCAN_t* R);
uint32_t RollerCAN_ReadActualPosition(RollerCAN_t* R);

uint32_t RollerCAN_ReadSpeedMotor(RollerCAN_t* R);
uint32_t RollerCAN_ReadSpeed_kP(RollerCAN_t* R);
uint32_t RollerCAN_ReadSpeed_kI(RollerCAN_t* R);
uint32_t RollerCAN_ReadSpeed_kD(RollerCAN_t* R);

uint32_t RollerCAN_ReadPositionMotor(RollerCAN_t* R);
uint32_t RollerCAN_ReadPosition_kP(RollerCAN_t* R);
uint32_t RollerCAN_ReadPosition_kI(RollerCAN_t* R);
uint32_t RollerCAN_ReadPosition_kD(RollerCAN_t* R);

uint32_t RollerCAN_ReadCurrent(RollerCAN_t* R);

#endif /* INC_ROLLERCAN_H_ */
