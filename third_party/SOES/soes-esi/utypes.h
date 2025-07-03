#ifndef __UTYPES_H__
#define __UTYPES_H__

#include "cc.h"

/* Object dictionary storage */

typedef struct
{
   /* Identity */

   uint32_t serial;

   /* Inputs */

   struct
   {
      uint8_t io1;
      uint8_t io2;
      uint8_t io3;
      uint8_t io4;
      uint8_t io5;
      uint8_t io6;
      uint8_t io7;
      uint8_t io8;
   } input_io;
   int16_t can0_motor_positions[8];
   int16_t can0_motor_velocities[8];
   int16_t can0_motor_currents[8];
   uint8_t can0_motor_temperatures[8];
   int16_t can1_motor_positions[8];
   int16_t can1_motor_velocities[8];
   uint16_t can1_motor_currents[8];
   uint8_t can1_motor_temperatures[8];
   int32_t vesc_can0_motor_velocities[6];
   int32_t vesc_can1_motor_velocities[6];
   int32_t vesc_can0_motor_currents[6];
   int32_t vesc_can1_motor_currents[6];

   /* Outputs */

   struct
   {
      uint8_t io1;
      uint8_t io2;
      uint8_t io3;
      uint8_t io4;
      uint8_t io5;
      uint8_t io6;
      uint8_t io7;
      uint8_t io8;
   } output_io;
   int16_t can0_motor_commands[8];
   int16_t can1_motor_commands[8];
   int32_t vesc_can0_commands[6];
   int32_t vesc_can1_commands[6];

} _Objects;

extern _Objects Obj;

#endif /* __UTYPES_H__ */
