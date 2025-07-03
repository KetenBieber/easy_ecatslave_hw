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
static const char acName1600[] = "output_io";
static const char acName1600_00[] = "Max SubIndex";
static const char acName1600_01[] = "io1";
static const char acName1600_02[] = "Padding 9";
static const char acName1600_03[] = "io2";
static const char acName1600_04[] = "Padding 10";
static const char acName1600_05[] = "io3";
static const char acName1600_06[] = "Padding 11";
static const char acName1600_07[] = "io4";
static const char acName1600_08[] = "Padding 12";
static const char acName1600_09[] = "io5";
static const char acName1600_10[] = "Padding 13";
static const char acName1600_11[] = "io6";
static const char acName1600_12[] = "Padding 14";
static const char acName1600_13[] = "io7";
static const char acName1600_14[] = "Padding 15";
static const char acName1600_15[] = "io8";
static const char acName1600_16[] = "Padding 16";
static const char acName1601[] = "can0_motor_commands";
static const char acName1601_00[] = "Max SubIndex";
static const char acName1601_01[] = "id1";
static const char acName1601_02[] = "id2";
static const char acName1601_03[] = "id3";
static const char acName1601_04[] = "id4";
static const char acName1601_05[] = "id5";
static const char acName1601_06[] = "id6";
static const char acName1601_07[] = "id7";
static const char acName1601_08[] = "id8";
static const char acName1602[] = "can1_motor_commands";
static const char acName1602_00[] = "Max SubIndex";
static const char acName1602_01[] = "id1";
static const char acName1602_02[] = "id2";
static const char acName1602_03[] = "id3";
static const char acName1602_04[] = "id4";
static const char acName1602_05[] = "id5";
static const char acName1602_06[] = "id6";
static const char acName1602_07[] = "id7";
static const char acName1602_08[] = "id8";
static const char acName1603[] = "vesc_can0_commands";
static const char acName1603_00[] = "Max SubIndex";
static const char acName1603_01[] = "id1";
static const char acName1603_02[] = "id2";
static const char acName1603_03[] = "id3";
static const char acName1603_04[] = "id4";
static const char acName1603_05[] = "id5";
static const char acName1603_06[] = "id6";
static const char acName1604[] = "vesc_can1_commands";
static const char acName1604_00[] = "Max SubIndex";
static const char acName1604_01[] = "id1";
static const char acName1604_02[] = "id2";
static const char acName1604_03[] = "id3";
static const char acName1604_04[] = "id4";
static const char acName1604_05[] = "id5";
static const char acName1604_06[] = "id6";
static const char acName1A00[] = "input_io";
static const char acName1A00_00[] = "Max SubIndex";
static const char acName1A00_01[] = "io1";
static const char acName1A00_02[] = "Padding 1";
static const char acName1A00_03[] = "io2";
static const char acName1A00_04[] = "Padding 2";
static const char acName1A00_05[] = "io3";
static const char acName1A00_06[] = "Padding 3";
static const char acName1A00_07[] = "io4";
static const char acName1A00_08[] = "Padding 4";
static const char acName1A00_09[] = "io5";
static const char acName1A00_10[] = "Padding 5";
static const char acName1A00_11[] = "io6";
static const char acName1A00_12[] = "Padding 6";
static const char acName1A00_13[] = "io7";
static const char acName1A00_14[] = "Padding 7";
static const char acName1A00_15[] = "io8";
static const char acName1A00_16[] = "Padding 8";
static const char acName1A01[] = "can0_motor_positions";
static const char acName1A01_00[] = "Max SubIndex";
static const char acName1A01_01[] = "id1";
static const char acName1A01_02[] = "id2";
static const char acName1A01_03[] = "id3";
static const char acName1A01_04[] = "id4";
static const char acName1A01_05[] = "id5";
static const char acName1A01_06[] = "id6";
static const char acName1A01_07[] = "id7";
static const char acName1A01_08[] = "id8";
static const char acName1A02[] = "can0_motor_velocities";
static const char acName1A02_00[] = "Max SubIndex";
static const char acName1A02_01[] = "id1";
static const char acName1A02_02[] = "id2";
static const char acName1A02_03[] = "id3";
static const char acName1A02_04[] = "id4";
static const char acName1A02_05[] = "id5";
static const char acName1A02_06[] = "id6";
static const char acName1A02_07[] = "id7";
static const char acName1A02_08[] = "id8";
static const char acName1A03[] = "can0_motor_currents";
static const char acName1A03_00[] = "Max SubIndex";
static const char acName1A03_01[] = "id1";
static const char acName1A03_02[] = "id2";
static const char acName1A03_03[] = "id3";
static const char acName1A03_04[] = "id4";
static const char acName1A03_05[] = "id5";
static const char acName1A03_06[] = "id6";
static const char acName1A03_07[] = "id7";
static const char acName1A03_08[] = "id8";
static const char acName1A04[] = "can0_motor_temperatures";
static const char acName1A04_00[] = "Max SubIndex";
static const char acName1A04_01[] = "id1";
static const char acName1A04_02[] = "id2";
static const char acName1A04_03[] = "id3";
static const char acName1A04_04[] = "id4";
static const char acName1A04_05[] = "id5";
static const char acName1A04_06[] = "id6";
static const char acName1A04_07[] = "id7";
static const char acName1A04_08[] = "id8";
static const char acName1A05[] = "can1_motor_positions";
static const char acName1A05_00[] = "Max SubIndex";
static const char acName1A05_01[] = "id1";
static const char acName1A05_02[] = "id2";
static const char acName1A05_03[] = "id3";
static const char acName1A05_04[] = "id4";
static const char acName1A05_05[] = "id5";
static const char acName1A05_06[] = "id6";
static const char acName1A05_07[] = "id7";
static const char acName1A05_08[] = "id8";
static const char acName1A06[] = "can1_motor_velocities";
static const char acName1A06_00[] = "Max SubIndex";
static const char acName1A06_01[] = "id1";
static const char acName1A06_02[] = "id2";
static const char acName1A06_03[] = "id3";
static const char acName1A06_04[] = "id4";
static const char acName1A06_05[] = "id5";
static const char acName1A06_06[] = "id6";
static const char acName1A06_07[] = "id7";
static const char acName1A06_08[] = "id8";
static const char acName1A07[] = "can1_motor_currents";
static const char acName1A07_00[] = "Max SubIndex";
static const char acName1A07_01[] = "id1";
static const char acName1A07_02[] = "id2";
static const char acName1A07_03[] = "id3";
static const char acName1A07_04[] = "id4";
static const char acName1A07_05[] = "id5";
static const char acName1A07_06[] = "id6";
static const char acName1A07_07[] = "id7";
static const char acName1A07_08[] = "id8";
static const char acName1A08[] = "can1_motor_temperatures";
static const char acName1A08_00[] = "Max SubIndex";
static const char acName1A08_01[] = "id1";
static const char acName1A08_02[] = "id2";
static const char acName1A08_03[] = "id3";
static const char acName1A08_04[] = "id4";
static const char acName1A08_05[] = "id5";
static const char acName1A08_06[] = "id6";
static const char acName1A08_07[] = "id7";
static const char acName1A08_08[] = "id8";
static const char acName1A09[] = "vesc_can0_motor_velocities";
static const char acName1A09_00[] = "Max SubIndex";
static const char acName1A09_01[] = "id1";
static const char acName1A09_02[] = "id2";
static const char acName1A09_03[] = "id3";
static const char acName1A09_04[] = "id4";
static const char acName1A09_05[] = "id5";
static const char acName1A09_06[] = "id6";
static const char acName1A0A[] = "vesc_can1_motor_velocities";
static const char acName1A0A_00[] = "Max SubIndex";
static const char acName1A0A_01[] = "id1";
static const char acName1A0A_02[] = "id2";
static const char acName1A0A_03[] = "id3";
static const char acName1A0A_04[] = "id4";
static const char acName1A0A_05[] = "id5";
static const char acName1A0A_06[] = "id6";
static const char acName1A0B[] = "vesc_can0_motor_currents";
static const char acName1A0B_00[] = "Max SubIndex";
static const char acName1A0B_01[] = "id1";
static const char acName1A0B_02[] = "id2";
static const char acName1A0B_03[] = "id3";
static const char acName1A0B_04[] = "id4";
static const char acName1A0B_05[] = "id5";
static const char acName1A0B_06[] = "id6";
static const char acName1A0C[] = "vesc_can1_motor_currents";
static const char acName1A0C_00[] = "Max SubIndex";
static const char acName1A0C_01[] = "id1";
static const char acName1A0C_02[] = "id2";
static const char acName1A0C_03[] = "id3";
static const char acName1A0C_04[] = "id4";
static const char acName1A0C_05[] = "id5";
static const char acName1A0C_06[] = "id6";
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
static const char acName1C12_03[] = "PDO Mapping";
static const char acName1C12_04[] = "PDO Mapping";
static const char acName1C12_05[] = "PDO Mapping";
static const char acName1C13[] = "Sync Manager 3 PDO Assignment";
static const char acName1C13_00[] = "Max SubIndex";
static const char acName1C13_01[] = "PDO Mapping";
static const char acName1C13_02[] = "PDO Mapping";
static const char acName1C13_03[] = "PDO Mapping";
static const char acName1C13_04[] = "PDO Mapping";
static const char acName1C13_05[] = "PDO Mapping";
static const char acName1C13_06[] = "PDO Mapping";
static const char acName1C13_07[] = "PDO Mapping";
static const char acName1C13_08[] = "PDO Mapping";
static const char acName1C13_09[] = "PDO Mapping";
static const char acName1C13_10[] = "PDO Mapping";
static const char acName1C13_11[] = "PDO Mapping";
static const char acName1C13_12[] = "PDO Mapping";
static const char acName1C13_13[] = "PDO Mapping";
static const char acName6000[] = "input_io";
static const char acName6000_00[] = "Max SubIndex";
static const char acName6000_01[] = "io1";
static const char acName6000_02[] = "io2";
static const char acName6000_03[] = "io3";
static const char acName6000_04[] = "io4";
static const char acName6000_05[] = "io5";
static const char acName6000_06[] = "io6";
static const char acName6000_07[] = "io7";
static const char acName6000_08[] = "io8";
static const char acName6001[] = "can0_motor_positions";
static const char acName6001_00[] = "Max SubIndex";
static const char acName6001_01[] = "id1";
static const char acName6001_02[] = "id2";
static const char acName6001_03[] = "id3";
static const char acName6001_04[] = "id4";
static const char acName6001_05[] = "id5";
static const char acName6001_06[] = "id6";
static const char acName6001_07[] = "id7";
static const char acName6001_08[] = "id8";
static const char acName6002[] = "can0_motor_velocities";
static const char acName6002_00[] = "Max SubIndex";
static const char acName6002_01[] = "id1";
static const char acName6002_02[] = "id2";
static const char acName6002_03[] = "id3";
static const char acName6002_04[] = "id4";
static const char acName6002_05[] = "id5";
static const char acName6002_06[] = "id6";
static const char acName6002_07[] = "id7";
static const char acName6002_08[] = "id8";
static const char acName6003[] = "can0_motor_currents";
static const char acName6003_00[] = "Max SubIndex";
static const char acName6003_01[] = "id1";
static const char acName6003_02[] = "id2";
static const char acName6003_03[] = "id3";
static const char acName6003_04[] = "id4";
static const char acName6003_05[] = "id5";
static const char acName6003_06[] = "id6";
static const char acName6003_07[] = "id7";
static const char acName6003_08[] = "id8";
static const char acName6004[] = "can0_motor_temperatures";
static const char acName6004_00[] = "Max SubIndex";
static const char acName6004_01[] = "id1";
static const char acName6004_02[] = "id2";
static const char acName6004_03[] = "id3";
static const char acName6004_04[] = "id4";
static const char acName6004_05[] = "id5";
static const char acName6004_06[] = "id6";
static const char acName6004_07[] = "id7";
static const char acName6004_08[] = "id8";
static const char acName6005[] = "can1_motor_positions";
static const char acName6005_00[] = "Max SubIndex";
static const char acName6005_01[] = "id1";
static const char acName6005_02[] = "id2";
static const char acName6005_03[] = "id3";
static const char acName6005_04[] = "id4";
static const char acName6005_05[] = "id5";
static const char acName6005_06[] = "id6";
static const char acName6005_07[] = "id7";
static const char acName6005_08[] = "id8";
static const char acName6006[] = "can1_motor_velocities";
static const char acName6006_00[] = "Max SubIndex";
static const char acName6006_01[] = "id1";
static const char acName6006_02[] = "id2";
static const char acName6006_03[] = "id3";
static const char acName6006_04[] = "id4";
static const char acName6006_05[] = "id5";
static const char acName6006_06[] = "id6";
static const char acName6006_07[] = "id7";
static const char acName6006_08[] = "id8";
static const char acName6007[] = "can1_motor_currents";
static const char acName6007_00[] = "Max SubIndex";
static const char acName6007_01[] = "id1";
static const char acName6007_02[] = "id2";
static const char acName6007_03[] = "id3";
static const char acName6007_04[] = "id4";
static const char acName6007_05[] = "id5";
static const char acName6007_06[] = "id6";
static const char acName6007_07[] = "id7";
static const char acName6007_08[] = "id8";
static const char acName6008[] = "can1_motor_temperatures";
static const char acName6008_00[] = "Max SubIndex";
static const char acName6008_01[] = "id1";
static const char acName6008_02[] = "id2";
static const char acName6008_03[] = "id3";
static const char acName6008_04[] = "id4";
static const char acName6008_05[] = "id5";
static const char acName6008_06[] = "id6";
static const char acName6008_07[] = "id7";
static const char acName6008_08[] = "id8";
static const char acName6009[] = "vesc_can0_motor_velocities";
static const char acName6009_00[] = "Max SubIndex";
static const char acName6009_01[] = "id1";
static const char acName6009_02[] = "id2";
static const char acName6009_03[] = "id3";
static const char acName6009_04[] = "id4";
static const char acName6009_05[] = "id5";
static const char acName6009_06[] = "id6";
static const char acName600A[] = "vesc_can1_motor_velocities";
static const char acName600A_00[] = "Max SubIndex";
static const char acName600A_01[] = "id1";
static const char acName600A_02[] = "id2";
static const char acName600A_03[] = "id3";
static const char acName600A_04[] = "id4";
static const char acName600A_05[] = "id5";
static const char acName600A_06[] = "id6";
static const char acName600B[] = "vesc_can0_motor_currents";
static const char acName600B_00[] = "Max SubIndex";
static const char acName600B_01[] = "id1";
static const char acName600B_02[] = "id2";
static const char acName600B_03[] = "id3";
static const char acName600B_04[] = "id4";
static const char acName600B_05[] = "id5";
static const char acName600B_06[] = "id6";
static const char acName600C[] = "vesc_can1_motor_currents";
static const char acName600C_00[] = "Max SubIndex";
static const char acName600C_01[] = "id1";
static const char acName600C_02[] = "id2";
static const char acName600C_03[] = "id3";
static const char acName600C_04[] = "id4";
static const char acName600C_05[] = "id5";
static const char acName600C_06[] = "id6";
static const char acName7000[] = "output_io";
static const char acName7000_00[] = "Max SubIndex";
static const char acName7000_01[] = "io1";
static const char acName7000_02[] = "io2";
static const char acName7000_03[] = "io3";
static const char acName7000_04[] = "io4";
static const char acName7000_05[] = "io5";
static const char acName7000_06[] = "io6";
static const char acName7000_07[] = "io7";
static const char acName7000_08[] = "io8";
static const char acName7001[] = "can0_motor_commands";
static const char acName7001_00[] = "Max SubIndex";
static const char acName7001_01[] = "id1";
static const char acName7001_02[] = "id2";
static const char acName7001_03[] = "id3";
static const char acName7001_04[] = "id4";
static const char acName7001_05[] = "id5";
static const char acName7001_06[] = "id6";
static const char acName7001_07[] = "id7";
static const char acName7001_08[] = "id8";
static const char acName7002[] = "can1_motor_commands";
static const char acName7002_00[] = "Max SubIndex";
static const char acName7002_01[] = "id1";
static const char acName7002_02[] = "id2";
static const char acName7002_03[] = "id3";
static const char acName7002_04[] = "id4";
static const char acName7002_05[] = "id5";
static const char acName7002_06[] = "id6";
static const char acName7002_07[] = "id7";
static const char acName7002_08[] = "id8";
static const char acName7003[] = "vesc_can0_commands";
static const char acName7003_00[] = "Max SubIndex";
static const char acName7003_01[] = "id1";
static const char acName7003_02[] = "id2";
static const char acName7003_03[] = "id3";
static const char acName7003_04[] = "id4";
static const char acName7003_05[] = "id5";
static const char acName7003_06[] = "id6";
static const char acName7004[] = "vesc_can1_commands";
static const char acName7004_00[] = "Max SubIndex";
static const char acName7004_01[] = "id1";
static const char acName7004_02[] = "id2";
static const char acName7004_03[] = "id3";
static const char acName7004_04[] = "id4";
static const char acName7004_05[] = "id5";
static const char acName7004_06[] = "id6";

const _objd SDO1000[] =
{
  {0x0, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1000, 5001, NULL},
};
const _objd SDO1008[] =
{
  {0x0, DTYPE_VISIBLE_STRING, 56, ATYPE_RO, acName1008, 0, "rc_ecat"},
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
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1600_00, 16, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_01, 0x70000101, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_02, 0x00000007, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_03, 0x70000201, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_04, 0x00000007, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_05, 0x70000301, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_06, 0x00000007, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_07, 0x70000401, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_08, 0x00000007, NULL},
  {0x09, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_09, 0x70000501, NULL},
  {0x10, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_10, 0x00000007, NULL},
  {0x11, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_11, 0x70000601, NULL},
  {0x12, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_12, 0x00000007, NULL},
  {0x13, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_13, 0x70000701, NULL},
  {0x14, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_14, 0x00000007, NULL},
  {0x15, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_15, 0x70000801, NULL},
  {0x16, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1600_16, 0x00000007, NULL},
};
const _objd SDO1601[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1601_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_01, 0x70010110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_02, 0x70010210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_03, 0x70010310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_04, 0x70010410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_05, 0x70010510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_06, 0x70010610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_07, 0x70010710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1601_08, 0x70010810, NULL},
};
const _objd SDO1602[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1602_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_01, 0x70020110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_02, 0x70020210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_03, 0x70020310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_04, 0x70020410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_05, 0x70020510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_06, 0x70020610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_07, 0x70020710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1602_08, 0x70020810, NULL},
};
const _objd SDO1603[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1603_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_01, 0x70030120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_02, 0x70030220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_03, 0x70030320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_04, 0x70030420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_05, 0x70030520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1603_06, 0x70030620, NULL},
};
const _objd SDO1604[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1604_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_01, 0x70040120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_02, 0x70040220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_03, 0x70040320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_04, 0x70040420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_05, 0x70040520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1604_06, 0x70040620, NULL},
};
const _objd SDO1A00[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A00_00, 16, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_01, 0x60000101, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_02, 0x00000007, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_03, 0x60000201, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_04, 0x00000007, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_05, 0x60000301, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_06, 0x00000007, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_07, 0x60000401, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_08, 0x00000007, NULL},
  {0x09, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_09, 0x60000501, NULL},
  {0x10, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_10, 0x00000007, NULL},
  {0x11, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_11, 0x60000601, NULL},
  {0x12, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_12, 0x00000007, NULL},
  {0x13, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_13, 0x60000701, NULL},
  {0x14, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_14, 0x00000007, NULL},
  {0x15, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_15, 0x60000801, NULL},
  {0x16, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A00_16, 0x00000007, NULL},
};
const _objd SDO1A01[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A01_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_01, 0x60010110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_02, 0x60010210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_03, 0x60010310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_04, 0x60010410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_05, 0x60010510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_06, 0x60010610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_07, 0x60010710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A01_08, 0x60010810, NULL},
};
const _objd SDO1A02[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A02_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_01, 0x60020110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_02, 0x60020210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_03, 0x60020310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_04, 0x60020410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_05, 0x60020510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_06, 0x60020610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_07, 0x60020710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A02_08, 0x60020810, NULL},
};
const _objd SDO1A03[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A03_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_01, 0x60030110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_02, 0x60030210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_03, 0x60030310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_04, 0x60030410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_05, 0x60030510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_06, 0x60030610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_07, 0x60030710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A03_08, 0x60030810, NULL},
};
const _objd SDO1A04[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A04_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_01, 0x60040108, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_02, 0x60040208, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_03, 0x60040308, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_04, 0x60040408, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_05, 0x60040508, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_06, 0x60040608, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_07, 0x60040708, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A04_08, 0x60040808, NULL},
};
const _objd SDO1A05[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A05_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_01, 0x60050110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_02, 0x60050210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_03, 0x60050310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_04, 0x60050410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_05, 0x60050510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_06, 0x60050610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_07, 0x60050710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A05_08, 0x60050810, NULL},
};
const _objd SDO1A06[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A06_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_01, 0x60060110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_02, 0x60060210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_03, 0x60060310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_04, 0x60060410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_05, 0x60060510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_06, 0x60060610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_07, 0x60060710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A06_08, 0x60060810, NULL},
};
const _objd SDO1A07[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A07_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_01, 0x60070110, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_02, 0x60070210, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_03, 0x60070310, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_04, 0x60070410, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_05, 0x60070510, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_06, 0x60070610, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_07, 0x60070710, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A07_08, 0x60070810, NULL},
};
const _objd SDO1A08[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A08_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_01, 0x60080108, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_02, 0x60080208, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_03, 0x60080308, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_04, 0x60080408, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_05, 0x60080508, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_06, 0x60080608, NULL},
  {0x07, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_07, 0x60080708, NULL},
  {0x08, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A08_08, 0x60080808, NULL},
};
const _objd SDO1A09[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A09_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_01, 0x60090120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_02, 0x60090220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_03, 0x60090320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_04, 0x60090420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_05, 0x60090520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A09_06, 0x60090620, NULL},
};
const _objd SDO1A0A[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A0A_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_01, 0x600A0120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_02, 0x600A0220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_03, 0x600A0320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_04, 0x600A0420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_05, 0x600A0520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0A_06, 0x600A0620, NULL},
};
const _objd SDO1A0B[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A0B_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_01, 0x600B0120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_02, 0x600B0220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_03, 0x600B0320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_04, 0x600B0420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_05, 0x600B0520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0B_06, 0x600B0620, NULL},
};
const _objd SDO1A0C[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1A0C_00, 6, NULL},
  {0x01, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_01, 0x600C0120, NULL},
  {0x02, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_02, 0x600C0220, NULL},
  {0x03, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_03, 0x600C0320, NULL},
  {0x04, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_04, 0x600C0420, NULL},
  {0x05, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_05, 0x600C0520, NULL},
  {0x06, DTYPE_UNSIGNED32, 32, ATYPE_RO, acName1A0C_06, 0x600C0620, NULL},
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
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C12_00, 5, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_01, 0x1600, NULL},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_02, 0x1601, NULL},
  {0x03, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_03, 0x1602, NULL},
  {0x04, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_04, 0x1603, NULL},
  {0x05, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C12_05, 0x1604, NULL},
};
const _objd SDO1C13[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName1C13_00, 13, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_01, 0x1A00, NULL},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_02, 0x1A01, NULL},
  {0x03, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_03, 0x1A02, NULL},
  {0x04, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_04, 0x1A03, NULL},
  {0x05, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_05, 0x1A04, NULL},
  {0x06, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_06, 0x1A05, NULL},
  {0x07, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_07, 0x1A06, NULL},
  {0x08, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_08, 0x1A07, NULL},
  {0x09, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_09, 0x1A08, NULL},
  {0x10, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_10, 0x1A09, NULL},
  {0x11, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_11, 0x1A0A, NULL},
  {0x12, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_12, 0x1A0B, NULL},
  {0x13, DTYPE_UNSIGNED16, 16, ATYPE_RO, acName1C13_13, 0x1A0C, NULL},
};
const _objd SDO6000[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6000_00, 8, NULL},
  {0x01, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_01, 0, &Obj.input_io.io1},
  {0x02, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_02, 0, &Obj.input_io.io2},
  {0x03, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_03, 0, &Obj.input_io.io3},
  {0x04, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_04, 0, &Obj.input_io.io4},
  {0x05, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_05, 0, &Obj.input_io.io5},
  {0x06, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_06, 0, &Obj.input_io.io6},
  {0x07, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_07, 0, &Obj.input_io.io7},
  {0x08, DTYPE_BOOLEAN, 1, ATYPE_RO, acName6000_08, 0, &Obj.input_io.io8},
};
const _objd SDO6001[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6001_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_01, 0, &Obj.can0_motor_positions[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_02, 0, &Obj.can0_motor_positions[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_03, 0, &Obj.can0_motor_positions[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_04, 0, &Obj.can0_motor_positions[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_05, 0, &Obj.can0_motor_positions[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_06, 0, &Obj.can0_motor_positions[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_07, 0, &Obj.can0_motor_positions[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6001_08, 0, &Obj.can0_motor_positions[7]},
};
const _objd SDO6002[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6002_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_01, 0, &Obj.can0_motor_velocities[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_02, 0, &Obj.can0_motor_velocities[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_03, 0, &Obj.can0_motor_velocities[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_04, 0, &Obj.can0_motor_velocities[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_05, 0, &Obj.can0_motor_velocities[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_06, 0, &Obj.can0_motor_velocities[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_07, 0, &Obj.can0_motor_velocities[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6002_08, 0, &Obj.can0_motor_velocities[7]},
};
const _objd SDO6003[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6003_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_01, 0, &Obj.can0_motor_currents[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_02, 0, &Obj.can0_motor_currents[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_03, 0, &Obj.can0_motor_currents[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_04, 0, &Obj.can0_motor_currents[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_05, 0, &Obj.can0_motor_currents[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_06, 0, &Obj.can0_motor_currents[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_07, 0, &Obj.can0_motor_currents[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6003_08, 0, &Obj.can0_motor_currents[7]},
};
const _objd SDO6004[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6004_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_01, 0, &Obj.can0_motor_temperatures[0]},
  {0x02, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_02, 0, &Obj.can0_motor_temperatures[1]},
  {0x03, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_03, 0, &Obj.can0_motor_temperatures[2]},
  {0x04, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_04, 0, &Obj.can0_motor_temperatures[3]},
  {0x05, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_05, 0, &Obj.can0_motor_temperatures[4]},
  {0x06, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_06, 0, &Obj.can0_motor_temperatures[5]},
  {0x07, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_07, 0, &Obj.can0_motor_temperatures[6]},
  {0x08, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6004_08, 0, &Obj.can0_motor_temperatures[7]},
};
const _objd SDO6005[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6005_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_01, 0, &Obj.can1_motor_positions[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_02, 0, &Obj.can1_motor_positions[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_03, 0, &Obj.can1_motor_positions[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_04, 0, &Obj.can1_motor_positions[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_05, 0, &Obj.can1_motor_positions[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_06, 0, &Obj.can1_motor_positions[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_07, 0, &Obj.can1_motor_positions[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6005_08, 0, &Obj.can1_motor_positions[7]},
};
const _objd SDO6006[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6006_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_01, 0, &Obj.can1_motor_velocities[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_02, 0, &Obj.can1_motor_velocities[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_03, 0, &Obj.can1_motor_velocities[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_04, 0, &Obj.can1_motor_velocities[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_05, 0, &Obj.can1_motor_velocities[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_06, 0, &Obj.can1_motor_velocities[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_07, 0, &Obj.can1_motor_velocities[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_TXPDO, acName6006_08, 0, &Obj.can1_motor_velocities[7]},
};
const _objd SDO6007[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6007_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_01, 0, &Obj.can1_motor_currents[0]},
  {0x02, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_02, 0, &Obj.can1_motor_currents[1]},
  {0x03, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_03, 0, &Obj.can1_motor_currents[2]},
  {0x04, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_04, 0, &Obj.can1_motor_currents[3]},
  {0x05, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_05, 0, &Obj.can1_motor_currents[4]},
  {0x06, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_06, 0, &Obj.can1_motor_currents[5]},
  {0x07, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_07, 0, &Obj.can1_motor_currents[6]},
  {0x08, DTYPE_UNSIGNED16, 16, ATYPE_RO | ATYPE_TXPDO, acName6007_08, 0, &Obj.can1_motor_currents[7]},
};
const _objd SDO6008[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6008_00, 8, NULL},
  {0x01, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_01, 0, &Obj.can1_motor_temperatures[0]},
  {0x02, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_02, 0, &Obj.can1_motor_temperatures[1]},
  {0x03, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_03, 0, &Obj.can1_motor_temperatures[2]},
  {0x04, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_04, 0, &Obj.can1_motor_temperatures[3]},
  {0x05, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_05, 0, &Obj.can1_motor_temperatures[4]},
  {0x06, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_06, 0, &Obj.can1_motor_temperatures[5]},
  {0x07, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_07, 0, &Obj.can1_motor_temperatures[6]},
  {0x08, DTYPE_UNSIGNED8, 8, ATYPE_RO | ATYPE_TXPDO, acName6008_08, 0, &Obj.can1_motor_temperatures[7]},
};
const _objd SDO6009[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName6009_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_01, 0, &Obj.vesc_can0_motor_velocities[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_02, 0, &Obj.vesc_can0_motor_velocities[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_03, 0, &Obj.vesc_can0_motor_velocities[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_04, 0, &Obj.vesc_can0_motor_velocities[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_05, 0, &Obj.vesc_can0_motor_velocities[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName6009_06, 0, &Obj.vesc_can0_motor_velocities[5]},
};
const _objd SDO600A[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName600A_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_01, 0, &Obj.vesc_can1_motor_velocities[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_02, 0, &Obj.vesc_can1_motor_velocities[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_03, 0, &Obj.vesc_can1_motor_velocities[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_04, 0, &Obj.vesc_can1_motor_velocities[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_05, 0, &Obj.vesc_can1_motor_velocities[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600A_06, 0, &Obj.vesc_can1_motor_velocities[5]},
};
const _objd SDO600B[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName600B_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_01, 0, &Obj.vesc_can0_motor_currents[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_02, 0, &Obj.vesc_can0_motor_currents[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_03, 0, &Obj.vesc_can0_motor_currents[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_04, 0, &Obj.vesc_can0_motor_currents[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_05, 0, &Obj.vesc_can0_motor_currents[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600B_06, 0, &Obj.vesc_can0_motor_currents[5]},
};
const _objd SDO600C[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName600C_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_01, 0, &Obj.vesc_can1_motor_currents[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_02, 0, &Obj.vesc_can1_motor_currents[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_03, 0, &Obj.vesc_can1_motor_currents[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_04, 0, &Obj.vesc_can1_motor_currents[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_05, 0, &Obj.vesc_can1_motor_currents[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_TXPDO, acName600C_06, 0, &Obj.vesc_can1_motor_currents[5]},
};
const _objd SDO7000[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7000_00, 8, NULL},
  {0x01, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_01, 0, &Obj.output_io.io1},
  {0x02, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_02, 0, &Obj.output_io.io2},
  {0x03, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_03, 0, &Obj.output_io.io3},
  {0x04, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_04, 0, &Obj.output_io.io4},
  {0x05, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_05, 0, &Obj.output_io.io5},
  {0x06, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_06, 0, &Obj.output_io.io6},
  {0x07, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_07, 0, &Obj.output_io.io7},
  {0x08, DTYPE_BOOLEAN, 1, ATYPE_RO, acName7000_08, 0, &Obj.output_io.io8},
};
const _objd SDO7001[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7001_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_01, 0, &Obj.can0_motor_commands[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_02, 0, &Obj.can0_motor_commands[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_03, 0, &Obj.can0_motor_commands[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_04, 0, &Obj.can0_motor_commands[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_05, 0, &Obj.can0_motor_commands[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_06, 0, &Obj.can0_motor_commands[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_07, 0, &Obj.can0_motor_commands[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7001_08, 0, &Obj.can0_motor_commands[7]},
};
const _objd SDO7002[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7002_00, 8, NULL},
  {0x01, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_01, 0, &Obj.can1_motor_commands[0]},
  {0x02, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_02, 0, &Obj.can1_motor_commands[1]},
  {0x03, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_03, 0, &Obj.can1_motor_commands[2]},
  {0x04, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_04, 0, &Obj.can1_motor_commands[3]},
  {0x05, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_05, 0, &Obj.can1_motor_commands[4]},
  {0x06, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_06, 0, &Obj.can1_motor_commands[5]},
  {0x07, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_07, 0, &Obj.can1_motor_commands[6]},
  {0x08, DTYPE_INTEGER16, 16, ATYPE_RO | ATYPE_RXPDO, acName7002_08, 0, &Obj.can1_motor_commands[7]},
};
const _objd SDO7003[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7003_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_01, 0, &Obj.vesc_can0_commands[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_02, 0, &Obj.vesc_can0_commands[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_03, 0, &Obj.vesc_can0_commands[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_04, 0, &Obj.vesc_can0_commands[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_05, 0, &Obj.vesc_can0_commands[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7003_06, 0, &Obj.vesc_can0_commands[5]},
};
const _objd SDO7004[] =
{
  {0x00, DTYPE_UNSIGNED8, 8, ATYPE_RO, acName7004_00, 6, NULL},
  {0x01, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_01, 0, &Obj.vesc_can1_commands[0]},
  {0x02, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_02, 0, &Obj.vesc_can1_commands[1]},
  {0x03, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_03, 0, &Obj.vesc_can1_commands[2]},
  {0x04, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_04, 0, &Obj.vesc_can1_commands[3]},
  {0x05, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_05, 0, &Obj.vesc_can1_commands[4]},
  {0x06, DTYPE_INTEGER32, 32, ATYPE_RO | ATYPE_RXPDO, acName7004_06, 0, &Obj.vesc_can1_commands[5]},
};

const _objectlist SDOobjects[] =
{
  {0x1000, OTYPE_VAR, 0, 0, acName1000, SDO1000},
  {0x1008, OTYPE_VAR, 0, 0, acName1008, SDO1008},
  {0x1009, OTYPE_VAR, 0, 0, acName1009, SDO1009},
  {0x100A, OTYPE_VAR, 0, 0, acName100A, SDO100A},
  {0x1018, OTYPE_RECORD, 4, 0, acName1018, SDO1018},
  {0x1600, OTYPE_RECORD, 16, 0, acName1600, SDO1600},
  {0x1601, OTYPE_RECORD, 8, 0, acName1601, SDO1601},
  {0x1602, OTYPE_RECORD, 8, 0, acName1602, SDO1602},
  {0x1603, OTYPE_RECORD, 6, 0, acName1603, SDO1603},
  {0x1604, OTYPE_RECORD, 6, 0, acName1604, SDO1604},
  {0x1A00, OTYPE_RECORD, 16, 0, acName1A00, SDO1A00},
  {0x1A01, OTYPE_RECORD, 8, 0, acName1A01, SDO1A01},
  {0x1A02, OTYPE_RECORD, 8, 0, acName1A02, SDO1A02},
  {0x1A03, OTYPE_RECORD, 8, 0, acName1A03, SDO1A03},
  {0x1A04, OTYPE_RECORD, 8, 0, acName1A04, SDO1A04},
  {0x1A05, OTYPE_RECORD, 8, 0, acName1A05, SDO1A05},
  {0x1A06, OTYPE_RECORD, 8, 0, acName1A06, SDO1A06},
  {0x1A07, OTYPE_RECORD, 8, 0, acName1A07, SDO1A07},
  {0x1A08, OTYPE_RECORD, 8, 0, acName1A08, SDO1A08},
  {0x1A09, OTYPE_RECORD, 6, 0, acName1A09, SDO1A09},
  {0x1A0A, OTYPE_RECORD, 6, 0, acName1A0A, SDO1A0A},
  {0x1A0B, OTYPE_RECORD, 6, 0, acName1A0B, SDO1A0B},
  {0x1A0C, OTYPE_RECORD, 6, 0, acName1A0C, SDO1A0C},
  {0x1C00, OTYPE_ARRAY, 4, 0, acName1C00, SDO1C00},
  {0x1C12, OTYPE_ARRAY, 5, 0, acName1C12, SDO1C12},
  {0x1C13, OTYPE_ARRAY, 13, 0, acName1C13, SDO1C13},
  {0x6000, OTYPE_RECORD, 8, 0, acName6000, SDO6000},
  {0x6001, OTYPE_ARRAY, 8, 0, acName6001, SDO6001},
  {0x6002, OTYPE_ARRAY, 8, 0, acName6002, SDO6002},
  {0x6003, OTYPE_ARRAY, 8, 0, acName6003, SDO6003},
  {0x6004, OTYPE_ARRAY, 8, 0, acName6004, SDO6004},
  {0x6005, OTYPE_ARRAY, 8, 0, acName6005, SDO6005},
  {0x6006, OTYPE_ARRAY, 8, 0, acName6006, SDO6006},
  {0x6007, OTYPE_ARRAY, 8, 0, acName6007, SDO6007},
  {0x6008, OTYPE_ARRAY, 8, 0, acName6008, SDO6008},
  {0x6009, OTYPE_ARRAY, 6, 0, acName6009, SDO6009},
  {0x600A, OTYPE_ARRAY, 6, 0, acName600A, SDO600A},
  {0x600B, OTYPE_ARRAY, 6, 0, acName600B, SDO600B},
  {0x600C, OTYPE_ARRAY, 6, 0, acName600C, SDO600C},
  {0x7000, OTYPE_RECORD, 8, 0, acName7000, SDO7000},
  {0x7001, OTYPE_ARRAY, 8, 0, acName7001, SDO7001},
  {0x7002, OTYPE_ARRAY, 8, 0, acName7002, SDO7002},
  {0x7003, OTYPE_ARRAY, 6, 0, acName7003, SDO7003},
  {0x7004, OTYPE_ARRAY, 6, 0, acName7004, SDO7004},
  {0xffff, 0xff, 0xff, 0xff, NULL, NULL}
};
