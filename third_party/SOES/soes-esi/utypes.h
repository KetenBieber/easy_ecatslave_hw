#ifndef __UTYPES_H__
#define __UTYPES_H__

#include "cc.h"

/* Object dictionary storage */

typedef struct
{
   /* Identity */

   uint32_t serial;

   /* Inputs */

   int16_t can0_motor_positions[2];
   int16_t can0_motor_velocities[2];
   int16_t can0_motor_currents[2];
   uint8_t can0_motor_temperatures[2];

   /* Outputs */

   struct
   {
      uint8_t led1;
      uint8_t led2;
      uint8_t led3;
   } led_ctrl;
   int16_t can0_motor_commands[2];

   /* Parameters */

   uint16_t max_motor_velocities[2];
} _Objects;

extern _Objects Obj;

#endif /* __UTYPES_H__ */
