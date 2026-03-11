#ifndef __MYIIC_H
#define __MYIIC_H

void myiic_W_SCL(uint8_t BitValue);
void myiic_W_SDA(uint8_t BitValue);
uint8_t myiic_R_SDA(void );
void myiic_init(void);
void myiic_start(void);
void myiic_stop(void);
void myiic_send(uint8_t Byte);
uint8_t myiic_receive(void);
void myiic_sendack(uint8_t Ackbit);
uint8_t myiic_receiveack(void);

#endif
