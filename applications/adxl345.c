#include <rtdevice.h>
#include "ADXL345.h"
#include <math.h>
 
void adxl345_init(void)
{
	write_reg(ADXL345_DATA_FORMAT, 0x00);//测量范围,正负16g，13位模式
	write_reg(ADXL345_BW_RATE, 0x0A);    //速率设定为100 参考pdf13页
	write_reg(ADXL345_POWER_CTL, 0x08);  //选择电源模式   参考pdf24页
	write_reg(ADXL345_INT_ENABLE, 0x80); //使能 DATA_READY 中断
	write_reg(ADXL345_OFSX, 0x00);       //X 偏移量 根据测试传感器的状态写入pdf29页
	write_reg(ADXL345_OFSY, 0x00);       //Y 偏移量 根据测试传感器的状态写入pdf29页
	write_reg(ADXL345_OFSZ, 0x00);       //Z 偏移量 根据测试传感器的状态写入pdf29页
}
 
// Writes a register
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[3];
    struct rt_i2c_msg msgs;

    buf[0] = reg; //cmd
    buf[1] = data[0];
    buf[2] = data[1];

    msgs.addr = ADXL345_SLA_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 3;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
 
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = ADXL345_SLA_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
 
 
 
 
 
// Reads the 3 channels and stores them in vector g
void ADXL345_read()
{
	Wire.beginTransmission(ADXL345_SLA_ADDR);
	// assert the MSB of the address to get the gyro 
	// to do slave-transmit subaddress updating.
	Wire.write(ADXL345_DATAX0 | (1 << 7)); 
	Wire.endTransmission();
	Wire.requestFrom(ADXL345_SLA_ADDR, 6);
 
	while (Wire.available() < 6);
 
	uint8_t xla = Wire.read();
	uint8_t xha = Wire.read();
	uint8_t yla = Wire.read();
	uint8_t yha = Wire.read();
	uint8_t zla = Wire.read();
	uint8_t zha = Wire.read();
 
	a.x = xha << 8 | xla;
	a.y = yha << 8 | yla;
	a.z = zha << 8 | zla;
}
