#include "stm32f10x.h"      // Device header
#include "myiic.h"
#include "mpu6050_Reg.h"

#define MPU6050_ADDRESS  0xD0

/*指定地址写寄存器*/
void mpu6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	myiic_start ();
	myiic_send (MPU6050_ADDRESS);
	myiic_receiveack ();
	myiic_send (RegAddress );
	myiic_receiveack ();
	myiic_send (Data);
	myiic_receiveack ();
	myiic_stop ();
}
/*指定地址读寄存器*/
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t data;
	myiic_start ();
	myiic_send (MPU6050_ADDRESS);
	myiic_receiveack ();
	myiic_send (RegAddress );
	myiic_receiveack ();
	
	myiic_start ();
	myiic_send (MPU6050_ADDRESS |0x01);
	myiic_receiveack ();
	data=myiic_receive ();
	myiic_sendack (1);
	myiic_stop();
	
	return data;
}
/*配置寄存器 存储数据*/
void mpu6050_init(void)
{
	myiic_init ();
	mpu6050_WriteReg(MPU6050_PWR_MGMT_1,0x01); //解除睡眠
	mpu6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	mpu6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
	mpu6050_WriteReg(MPU6050_CONFIG,0x06);
	mpu6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
	mpu6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);
}
/*读取数据*/
void mpu6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t High;
	uint8_t Low;
	
	High=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	Low=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX=(High<<8)|Low;
	High=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	Low=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY=(High<<8)|Low;
	High=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	Low=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ=(High<<8)|Low;
	
	High=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	Low=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX =(High<<8)|Low;
	High=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	Low=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY =(High<<8)|Low;
	High=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	Low=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ =(High<<8)|Low;
}
