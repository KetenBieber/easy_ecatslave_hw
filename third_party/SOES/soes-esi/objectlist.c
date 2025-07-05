#include "esc_coe.h"
#include "utypes.h"
#include <stddef.h>


static const char acName1000[] = "Device Type";
static const char acName1008[] = "Device Name";
static const char acName1009[] = "Hardware Version";
static const char acName100A[] = "Software Version";
static const char acName1018[] = "Identity Object";
static const char acName1018_00[] = "Max SubIndex";
static const char acName1018_01[] = "Vendor ID";
static const char acName1018_02[] = "Product Code";
static const char acName1018_03[] = "Revision Number";
static const char acName1018_04[] = "Serial Number";
static const char acName1600[] = "led_ctrl";
static const char acName1600_00[] = "Max SubIndex";
static const char acName1600_01[] = "led1";
static const char acName1600_02[] = "Padding 1";
static const char acName1600_03[] = "led2";
static const char acName1600_04[] = "Padding 2";
static const char acName1600_05[] = "led3";
static const char acName1600_06[] = "Padding 3";
static const char acName1601[] = "can0_motor_commands";
static const char acName1601_00[] = "Max SubIndex";
static const char acName1601_01[] = "id1";
static const char acName1601_02[] = "id2";
static const char acName1A00[] = "can0_motor_positions";
static const char acName1A00_00[] = "Max SubIndex";
static const char acName1A00_01[] = "id1";
static const char acName1A00_02[] = "id2";
static const char acName1A01[] = "can0_motor_velocities";
static const char acName1A01_00[] = "Max SubIndex";
static const char acName1A01_01[] = "id1";
static const char acName1A01_02[] = "id2";
static const char acName1A02[] = "can0_motor_currents";
static const char acName1A02_00[] = "Max SubIndex";
static const char acName1A02_01[] = "id1";
static const char acName1A02_02[] = "id2";
static const char acName1A03[] = "can0_motor_temperatures";
static const char acName1A03_00[] = "Max SubIndex";
static const char acName1A03_01[] = "id1";
static const char acName1A03_02[] = "id2";
static const char acName1C00[] = "Sync Manager Communication Type";
static const char acName1C00_00[] = "Max SubIndex";
static const char acName1C00_01[] = "Communications Type SM0";
static const char acName1C00_02[] = "Communications Type SM1";
static const char acName1C00_03[] = "Communications Type SM2";
static const char acName1C00_04[] = "Communications Type SM3";
static const char acName1C12[] = "Sync Manager 2 PDO Assignment";
static const char acName1C12_00[] = "Max SubIndex";
static const char acName1C12_01[] = "PDO Mapping";
static const char acName1C12_02[] = "PDO Mapping";
static const char acName1C13[] = "Sync Manager 3 PDO Assignment";
static const char acName1C13_00[] = "Max SubIndex";
static const char acName1C13_01[] = "PDO Mapping";
static const char acName1C13_02[] = "PDO Mapping";
static const char acName1C13_03[] = "PDO Mapping";
static const char acName1C13_04[] = "PDO Mapping";
static const char acName2000[] = "max_motor_velocities";
static const char acName2000_00[] = "Max SubIndex";
static const char acName2000_01[] = "id1";
static const char acName2000_02[] = "id2";
static const char acName6001[] = "can0_motor_positions";
static const char acName6001_00[] = "Max SubIndex";
static const char acName6001_01[] = "id1";
static const char acName6001_02[] = "id2";
static const char acName6002[] = "can0_motor_velocities";
static const char acName6002_00[] = "Max SubIndex";
static const char acName6002_01[] = "id1";
static const char acName6002_02[] = "id2";
static const char acName6003[] = "can0_motor_currents";
static const char acName6003_00[] = "Max SubIndex";
static const char acName6003_01[] = "id1";
static const char acName6003_02[] = "id2";
static const char acName6004[] = "can0_motor_temperatures";
static const char acName6004_00[] = "Max SubIndex";
static const char acName6004_01[] = "id1";
static const char acName6004_02[] = "id2";
static const char acName7000[] = "led_ctrl";
static const char acName7000_00[] = "Max SubIndex";
static const char acName7000_01[] = "led1";
static const char acName7000_02[] = "led2";
static const char acName7000_03[] = "led3";
static const char acName7001[] = "can0_motor_commands";
static const char acName7001_00[] = "Max SubIndex";
static const char acName7001_01[] = "id1";
static const char acName7001_02[] = "id2";

const _objd SDO1000[] =
{
  {0x0, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1000, 5001, NULL},
};
const _objd SDO1008[] =
{
  {0x0, DTYPE_VISIBLE_STRING, 112, ATYPE_RO, acName1008, 0, "sync_ecat_test"},
};
const _objd SDO1009[] =
{
  {0x0, DTYPE_VISIBLE_STRING, 40, ATYPE_RO, acName1009, 0, "0.0.1"},
};
const _objd SDO100A[] =
{
  {0x0, DTYPE_VISIBLE_STRING, 40, ATYPE_RO, acName100A, 0, "0.0.1"},
};
const _objd SDO1018[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1018_00, 4, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1018_01, 0, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1018_02, 700707, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1018_03, 2, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1018_04, 1, &Obj.serial},
};
const _objd SDO1600[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1600_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_01, 0x70000101, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_02, 0x00000007, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_03, 0x70000201, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_04, 0x00000007, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_05, 0x70000301, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_06, 0x00000007, NULL},
};
const _objd SDO1601[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1601_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_01, 0x70010110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_02, 0x70010210, NULL},
};
const _objd SDO1A00[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A00_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_01, 0x60010110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_02, 0x60010210, NULL},
};
const _objd SDO1A01[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A01_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_01, 0x60020110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_02, 0x60020210, NULL},
};
const _objd SDO1A02[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A02_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_01, 0x60030110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_02, 0x60030210, NULL},
};
const _objd SDO1A03[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A03_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_01, 0x60040108, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_02, 0x60040208, NULL},
};
const _objd SDO1C00[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C00_00, 4, NULL},
  {0x01, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C00_01, 1, NULL},
  {0x02, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C00_02, 2, NULL},
  {0x03, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C00_03, 3, NULL},
  {0x04, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C00_04, 4, NULL},
};
const _objd SDO1C12[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C12_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_01, 0x1600, NULL},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_02, 0x1601, NULL},
};
const _objd SDO1C13[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C13_00, 4, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_01, 0x1A00, NULL},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_02, 0x1A01, NULL},
  {0x03, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_03, 0x1A02, NULL},
  {0x04, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_04, 0x1A03, NULL},
};
const _objd SDO2000[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName2000_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName2000_01, 0, &Obj.max_motor_velocities[0]},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName2000_02, 0, &Obj.max_motor_velocities[1]},
};
const _objd SDO6001[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6001_00, 2, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_01, 0, &Obj.can0_motor_positions[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_02, 0, &Obj.can0_motor_positions[1]},
};
const _objd SDO6002[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6002_00, 2, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_01, 0, &Obj.can0_motor_velocities[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_02, 0, &Obj.can0_motor_velocities[1]},
};
const _objd SDO6003[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6003_00, 2, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_01, 0, &Obj.can0_motor_currents[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_02, 0, &Obj.can0_motor_currents[1]},
};
const _objd SDO6004[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6004_00, 2, NULL},
  {0x01, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_01, 0, &Obj.can0_motor_temperatures[0]},
  {0x02, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_02, 0, &Obj.can0_motor_temperatures[1]},
};
const _objd SDO7000[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7000_00, 3, NULL},
  {0x01, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_01, 0, &Obj.led_ctrl.led1},
  {0x02, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_02, 0, &Obj.led_ctrl.led2},
  {0x03, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_03, 0, &Obj.led_ctrl.led3},
};
const _objd SDO7001[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7001_00, 2, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_01, 0, &Obj.can0_motor_commands[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_02, 0, &Obj.can0_motor_commands[1]},
};

const _objectlist SDOobjects[] =
{
  {0x1000, OTYPE_VAR, 0, 0, acName1000, SDO1000},
  {0x1008, OTYPE_VAR, 0, 0, acName1008, SDO1008},
  {0x1009, OTYPE_VAR, 0, 0, acName1009, SDO1009},
  {0x100A, OTYPE_VAR, 0, 0, acName100A, SDO100A},
  {0x1018, OTYPE_RECORD, 4, 0, acName1018, SDO1018},
  {0x1600, OTYPE_RECORD, 6, 0, acName1600, SDO1600},
  {0x1601, OTYPE_RECORD, 2, 0, acName1601, SDO1601},
  {0x1A00, OTYPE_RECORD, 2, 0, acName1A00, SDO1A00},
  {0x1A01, OTYPE_RECORD, 2, 0, acName1A01, SDO1A01},
  {0x1A02, OTYPE_RECORD, 2, 0, acName1A02, SDO1A02},
  {0x1A03, OTYPE_RECORD, 2, 0, acName1A03, SDO1A03},
  {0x1C00, OTYPE_ARRAY, 4, 0, acName1C00, SDO1C00},
  {0x1C12, OTYPE_ARRAY, 2, 0, acName1C12, SDO1C12},
  {0x1C13, OTYPE_ARRAY, 4, 0, acName1C13, SDO1C13},
  {0x2000, OTYPE_ARRAY, 2, 0, acName2000, SDO2000},
  {0x6001, OTYPE_ARRAY, 2, 0, acName6001, SDO6001},
  {0x6002, OTYPE_ARRAY, 2, 0, acName6002, SDO6002},
  {0x6003, OTYPE_ARRAY, 2, 0, acName6003, SDO6003},
  {0x6004, OTYPE_ARRAY, 2, 0, acName6004, SDO6004},
  {0x7000, OTYPE_RECORD, 3, 0, acName7000, SDO7000},
  {0x7001, OTYPE_ARRAY, 2, 0, acName7001, SDO7001},
  {0xffff, 0xff, 0xff, 0xff, NULL, NULL}
};
