#ifndef __SIM800C_H
#define __SIM800C_H	 
#include "sys.h" 

#define sim800_power PDout(13)	// PD13

void init_sim800c(void);
u8 check_sim800c_ready(void);

#endif














