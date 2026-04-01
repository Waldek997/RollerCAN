/*
 * RollerCAN.c
 *
 *  Created on: Mar 10, 2026
 *      Author: nicni
 */
#include "main.h"
#include "RollerCAN.h"
#include "RollerCAN_Def.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>



typedef union {
    struct {
        uint32_t can_id : 8;   // bity 0-7
        uint32_t option : 16;  // bity 8-23
        uint32_t cmd_id : 5;    // bity 24-28
    } __attribute__((packed));
    uint32_t raw;
} RollerCAN_ID_t;

typedef union {
    struct {
        uint16_t index;         // 0x7020, 0x7004, etc
        uint16_t reserved;
        uint8_t  param1;
        uint8_t  param2;
        uint8_t  param3;
        uint8_t  param4;
    } __attribute__((packed));
    uint8_t raw[8];
} RollerCAN_Data_t;


static void RollerCAN_SendingFrame(RollerCAN_t* R, uint8_t cmd_id, uint16_t option, uint16_t index, uint8_t* params)
{
    uint32_t start = HAL_GetTick();
    while (!R->tx_ready && (HAL_GetTick() - start) < 10)
    {

    }
    if (!R->tx_ready)
    {
        return;
    }
    R->tx_ready = false;

    RollerCAN_ID_t id;
    memset(&id, 0, sizeof(id));
    id.cmd_id = cmd_id;
    id.option = option;
    id.can_id = R->can_id;

    RollerCAN_Data_t data;
    memset(data.raw, 0, 8);
    data.index = index;

    if (params != NULL)
    {
        data.param1 = params[0];
        data.param2 = params[1];
        data.param3 = params[2];
        data.param4 = params[3];

    }
    // WYWOŁANIE CALLBACKA – main wykona właściwe wysłanie
    if (R->RollerCAN_SendCallback_t)
    {
        R->RollerCAN_SendCallback_t(id.raw, data.raw);
    }

}

static void RollerCAN_GetParameter(RollerCAN_t* R, uint16_t index, uint8_t* Data)
{
	RollerCAN_SendingFrame(R, ROLLERCAN_READ_CMD, 0,  index, Data);
}

static void RollerCAN_SetParameter(RollerCAN_t* R, uint16_t index, uint8_t* Data)
{
	RollerCAN_SendingFrame(R, ROLLERCAN_WRITE_CMD, 0,  index, Data);
}

static void FlushData(RollerCAN_t* R)
{
	memset(R->Data, 0, 4);
}

static void RollerCAN_SendingComend(RollerCAN_t* R, uint8_t cmd_id)
{
	RollerCAN_SendingFrame(R, cmd_id, 0, 0 , 0);
}

void RollerCAN_ReadFrame(RollerCAN_t* R, uint32_t identifier, uint8_t* data)
{
	if (R == NULL)
	{
	//        return ROLLERCAN_ERROR;
	}

    RollerCAN_ID_t id;
    id.raw = identifier;

    R->cmd_id = id.cmd_id;
    R->option = id.option;

    RollerCAN_Data_t d;
    memcpy(d.raw, data, 8);

    if (id.can_id == 0xFE && id.option == R->can_id) {
        R->init_flag = true;      // ← flaga inicjalizacji!
        R->data_ready = true;     // ← są nowe dane
        return;
    }

    if (id.can_id != R->can_id) return;

    switch (d.index)
    {
        case ROLLERCAN_RGB_MODE:
        	FlushData(R);
        	memcpy(R->Data, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_RGB_COLOR:
        	memcpy(R->Data, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_RGB_BRIGHTNESS:
        	memcpy(R->Data, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_TEMP:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_INPUT_VOLTAGE:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_ENCODER:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_ACTUAL_CURRENT:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_ACTUAL_POSITION:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_ACTUAL_SPEED:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_SPEED_KP:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_SPEED_KI:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_SPEED_KD:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_POSITION_KP:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_POSITION_KI:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_POSITION_KD:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
        case ROLLERCAN_CURRENT:
        	memcpy(&R->LastValue, &d.param1, 4);
            R->data_ready = true;
            break;
    }
}

void RollerCAN_TxComplete(RollerCAN_t* R)
{
    if (R) R->tx_ready = true;
}

RollerCAN_Status RollerCAN_Init(RollerCAN_t* R, uint32_t can_id, void *CallbackSend, void *CallbackFilter)
{
//	uint8_t tmp;
	R->init_flag = false;
	R->can_id = can_id;
	R->RollerCAN_SendCallback_t = CallbackSend;
	R->RollerCAN_UpdateFilter_t = CallbackFilter;
	R->init_flag = false;
    R->tx_ready = true;


    R->RollerCAN_UpdateFilter_t(0xFE);

    RollerCAN_SendingFrame(R, 0, 0, 0, 0);

    uint32_t start = HAL_GetTick();
    while (!R->init_flag && (HAL_GetTick() - start) < 10) {

    }

    if (R->init_flag)
    {
    	R->RollerCAN_UpdateFilter_t(R->can_id);
        return ROLLERCAN_OK;
    }

    return ROLLERCAN_ERROR;
}

void RollerCAN_OnOff(RollerCAN_t* R, bool OnOff)
{
	if(OnOff == true)
	{
		RollerCAN_SendingComend(R, ROLLERCAN_ON);
	}
	else if(OnOff == false)
	{
		RollerCAN_SendingComend(R, ROLLERCAN_OFF);
	}
}

void RollerCAN_SaveParametersToFlash(RollerCAN_t* R)
{
	RollerCAN_SendingComend(R, ROLLERCAN_FLASH);
}

void RollerCAN_UnProtection(RollerCAN_t* R)
{
	RollerCAN_SendingComend(R, ROLLERCAN_UNPROTECT);
}

void RollerCAN_OnOff_Protection(RollerCAN_t* R, bool OnOff)
{
	if(OnOff == true)
	{
		RollerCAN_SendingComend(R, ROLLERCAN_PROTECTION_ON);
	}
	else if(OnOff == false)
	{
		RollerCAN_SendingComend(R, ROLLERCAN_PROTECTION_OFF);
	}
}

///////////////// Current ///////////////////////////////////
uint32_t RollerCAN_ReadCurrent(RollerCAN_t* R)
{
	uint32_t current = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_CURRENT, NULL);
	if(R->data_ready == true)
	{
		current = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		current = 0;
	}
	return current;
}

void RollerCAN_SetCurrent(RollerCAN_t* R, uint32_t Current)
{
    uint32_t raw_current = Current;
    FlushData(R);
    memcpy(R->Data, &raw_current, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_CURRENT, R->Data);
}

//////////////// Position Mode /////////////////////////////////
uint32_t RollerCAN_ReadPosition_kP(RollerCAN_t* R)
{
	uint32_t kP = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_POSITION_KP, NULL);
	if(R->data_ready == true)
	{
		kP = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kP = 0;
	}
	return kP;

}

void RollerCAN_SetPosition_kP(RollerCAN_t* R, float Set_kP)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kP * 1000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_POSITION_KP, R->Data);
}

uint32_t RollerCAN_ReadPosition_kI(RollerCAN_t* R)
{
	uint32_t kI = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_POSITION_KI, NULL);
	if(R->data_ready == true)
	{
		kI = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kI = 0;
	}
	return kI;

}

void RollerCAN_SetPosition_kI(RollerCAN_t* R, float Set_kI)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kI * 100000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_POSITION_KI, R->Data);
}

uint32_t RollerCAN_ReadPosition_kD(RollerCAN_t* R)
{
	uint32_t kD = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_POSITION_KD, NULL);
	if(R->data_ready == true)
	{
		kD = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kD = 0;
	}
	return kD;

}

void RollerCAN_SetPosition_kD(RollerCAN_t* R, float Set_kD)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kD * 1000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_POSITION_KD, R->Data);
}

void RollerCAN_SetPositionCurrent(RollerCAN_t* R, uint32_t PositionCurrent)
{
    uint32_t raw_position = PositionCurrent * 100;
    FlushData(R);
    memcpy(R->Data, &raw_position, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_POSITION_CURRENT, R->Data);
}

uint32_t RollerCAN_ReadPositionMotor(RollerCAN_t* R)
{
	uint32_t position = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_POSITION, NULL);
	if(R->data_ready == true)
	{
		position = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		position = 0;
	}
	return position / 100;
}

void RollerCAN_SetPositionMotor(RollerCAN_t* R, uint32_t position)
{
    uint32_t raw_position = position * 100;
    FlushData(R);
    memcpy(R->Data, &raw_position, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_POSITION, R->Data);
}

/////////////// Speed Mode /////////////////////////////////////////////
uint32_t RollerCAN_ReadSpeed_kP(RollerCAN_t* R)
{
	uint32_t kP = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_SPEED_KP, NULL);
	if(R->data_ready == true)
	{
		kP = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kP = 0;
	}
	return kP;

}

void RollerCAN_SetSpeed_kP(RollerCAN_t* R, float Set_kP)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kP * 1000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_SPEED_KP, R->Data);
}

uint32_t RollerCAN_ReadSpeed_kI(RollerCAN_t* R)
{
	uint32_t kI = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_SPEED_KI, NULL);
	if(R->data_ready == true)
	{
		kI = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kI = 0;
	}
	return kI;

}

void RollerCAN_SetSpeed_kI(RollerCAN_t* R, float Set_kI)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kI * 100000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_SPEED_KI, R->Data);
}

uint32_t RollerCAN_ReadSpeed_kD(RollerCAN_t* R)
{
	uint32_t kD = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_SPEED_KD, NULL);
	if(R->data_ready == true)
	{
		kD = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		kD = 0;
	}
	return kD;

}

void RollerCAN_SetSpeed_kD(RollerCAN_t* R, float Set_kD)
{
	FlushData(R);
	uint32_t raw = (uint32_t)(Set_kD * 1000);
    memcpy(R->Data, &raw, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_SPEED_KD, R->Data);
}

void RollerCAN_SetSpeedCurrent(RollerCAN_t* R, uint32_t SpeedCurrent)
{
    uint32_t raw_speed = SpeedCurrent * 100;
    FlushData(R);
    memcpy(R->Data, &raw_speed, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_SPEED_CURRENT, R->Data);
}

uint32_t RollerCAN_ReadSpeedMotor(RollerCAN_t* R)
{
	uint32_t speed = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_ACTUAL_SPEED, NULL);
	if(R->data_ready == true)
	{
		speed = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		speed = 0;
	}
	return speed / 100;
}

void RollerCAN_SetSpeedMotor(RollerCAN_t* R, uint32_t Speed)
{
    uint32_t raw_speed = Speed * 100;
    FlushData(R);
    memcpy(R->Data, &raw_speed, 4);
	RollerCAN_SetParameter(R, ROLLERCAN_SPEED, R->Data);
}

void RollerCAN_StartStop(RollerCAN_t* R, bool OnOff)
{
	RollerCAN_SetMode(R, ROLLER_MODE_SPEED);
	FlushData(R);
	R->Data[0] = OnOff;
	RollerCAN_SetParameter(R, ROLLERCAN_SWITCH_ON_OFF, R->Data);
}

///////////// Read parameters ///////////////////////////////////////////

uint32_t RollerCAN_ReadActualPosition(RollerCAN_t* R)
{
	uint32_t position = 0;

	RollerCAN_GetParameter(R, ROLLERCAN_ACTUAL_POSITION, NULL);
	if(R->data_ready == true)
	{
		position = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		position = 0;
	}
	return position / 100;
}

uint32_t RollerCAN_ReadActualCurrent(RollerCAN_t* R)
{
	uint32_t current = 0;
	RollerCAN_GetParameter(R, ROLLERCAN_ACTUAL_CURRENT, NULL);
	if(R->data_ready == true)
	{
		current = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		current = 0;
	}
	return current / 100;
}

uint32_t RollerCAN_ReadActualEncoder(RollerCAN_t* R)
{
	uint32_t encoder = 0;
	RollerCAN_SetMode(R, ROLLER_MODE_ENCODER);
	RollerCAN_GetParameter(R, ROLLERCAN_ENCODER, NULL);
	if(R->data_ready == true)
	{
		encoder = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		encoder = 0;
	}
	return encoder;
}

uint32_t RollerCAN_ReadActualVoltageInput(RollerCAN_t* R)
{
	uint32_t voltage = 0;
	RollerCAN_GetParameter(R, ROLLERCAN_INPUT_VOLTAGE, NULL);
	if(R->data_ready == true)
	{
		voltage = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		voltage = 0;
	}
	return voltage / 100;
}

uint32_t RollerCAN_ReadTemp(RollerCAN_t* R)
{
	uint32_t temp = 0;
	RollerCAN_GetParameter(R, ROLLERCAN_TEMP, NULL);
	if(R->data_ready == true)
	{
		temp = R->LastValue;
		R->data_ready = false;
	}
	else
	{
		temp = 0;
	}
	return temp;
}

//////////////// LED ////////////////////////////////////////////////

void RollerCAN_SetBrightessLED(RollerCAN_t* R, uint8_t BrightessLED)
{
	RollerCAN_Set_RGB_Mode(R);
	FlushData(R);
	R->Data[0] = BrightessLED;
	RollerCAN_SetParameter(R, ROLLERCAN_RGB_BRIGHTNESS, R->Data);
}

uint8_t RollerCAN_ReadBrightessLED(RollerCAN_t* R)
{
	uint8_t Value;
	RollerCAN_GetParameter(R, ROLLERCAN_RGB_BRIGHTNESS, NULL);
	if(R->data_ready == true)
	{
		Value = R->Data[0];
		R->data_ready = false;
	}
	else
	{
		Value = 0;
	}
	return Value;
}

void RollerCAN_SetColorLED(RollerCAN_t* R, uint8_t Red, uint8_t Green, uint8_t Blue)
{
	RollerCAN_Set_RGB_Mode(R);
	FlushData(R);
	R->Data[0] = Red;
	R->Data[1] = Green;
	R->Data[2] = Blue;
	RollerCAN_SetParameter(R, ROLLERCAN_RGB_COLOR, R->Data);
}

void RollerCAN_Read_ColorLED(RollerCAN_t* R, uint32_t *rgbValues)
{
	RollerCAN_GetParameter(R, ROLLERCAN_RGB_COLOR, NULL);
	if(R->data_ready == true)
	{
		rgbValues[0] = R->Data[0];
		rgbValues[1] = R->Data[1];
		rgbValues[2] = R->Data[2];
		R->data_ready = false;
	}
	else
	{
		FlushData(R);
	}
}

void RollerCAN_Set_RGB_Mode(RollerCAN_t* R)
{
	FlushData(R);
	R->Data[0] = 1;
	RollerCAN_SetParameter(R, ROLLERCAN_RGB_MODE, R->Data);
}

uint8_t RollerCAN_Read_RGB_Mode(RollerCAN_t* R)
{
	uint8_t Value;
	RollerCAN_GetParameter(R, ROLLERCAN_RGB_MODE, NULL);
	if(R->data_ready == true)
	{
		Value = R->Data[0];
		R->data_ready = false;
	}
	else
	{
		Value = 0;
	}
	return Value;
}

//////////////// Function //////////////////////////////////////

void RollerCAN_SetMode(RollerCAN_t* R, roller_mode_t mode)
{
	FlushData(R);
	R->Data[0] = mode;
	RollerCAN_SetParameter(R, ROLLERCAN_MODE, R->Data);
}

