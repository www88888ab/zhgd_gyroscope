#include <rtdevice.h>
#include "ADXL345.h"
#include <math.h>
 
#define adxl345_I2C_BUS_NAME     "i2c1"  /* 传感器连接的I2C总线设备名称 */

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C总线设备句柄 */
static rt_uint8_t initialized = RT_FALSE;                /* 传感器初始化状态 */
 


static rt_err_t adxl345_tfr(struct rt_i2c_bus_device *bus,rt_uint8_t wr, rt_uint8_t reg,rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs[2];

    msgs[0].addr = ADXL345_SLA_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &reg;
    msgs[0].len = 1;

    msgs[1].addr = ADXL345_SLA_ADDR;
	
		if(wr)// write
			msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START; 
		else
			msgs[1].flags = RT_I2C_RD;
	   
    msgs[1].buf = buf;
    msgs[1].len = len;

    if (rt_i2c_transfer(bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
				if(wr)
					rt_kprintf("Writing command error\n");
				else
					rt_kprintf("Reading command error\n");
        return -RT_ERROR;
    }
}


 
 /*
// Reads the 3 channels and stores them in vector g
void ADXL345_read(rt_uint8_t )
{


//a.x = xha << 8 | xla;
//	a.y = yha << 8 | yla;
//	a.z = zha << 8 | zla;
}*/


void adxl345_init(const char *name)
{
		rt_uint8_t buf[1];
	
	  i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(name);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", name);
    }
    else
    {
			buf[0] = 0x00;
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_DATA_FORMAT, 1,buf);//测量范围,正负16g，13位模式
			
			buf[0] = 0x0a;
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_BW_RATE,1,buf);    //速率设定为100 参考pdf13页
			
			buf[0] = 0x08;
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_POWER_CTL, 1,buf);  //选择电源模式   参考pdf24页
			
			
			//adxl345_tfr(ADXL345_INT_ENABLE, 0x80); // DATA_READY 中断
			buf[0] = 0;
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_INT_ENABLE,1,buf); // DATA_READY 中断
			
			buf[0] = 0;
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_OFSX,1,buf);       //X 偏移量 根据测试传感器的状态写入pdf29页
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_OFSY,1,buf);       //Y 偏移量 根据测试传感器的状态写入pdf29页
			adxl345_tfr(i2c_bus,ADXL345_WRITE,ADXL345_OFSZ,1,buf);       //Z 偏移量 根据测试传感器的状态写入pdf29页
			
			rt_kprintf(" init reg ok\n\r");
		}
}
 


static void i2c_sensor_sample(int argc, char *argv[])
{
 //   float humidity, temperature;
    char name[RT_NAME_MAX];
	rt_uint8_t buf[6];

  //  humidity = 0.0;
  //  temperature = 0.0;

    if (argc == 2)
    {
        rt_strncpy(name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(name, adxl345_I2C_BUS_NAME, RT_NAME_MAX);
    }

    if (!initialized)
    {
        /* 传感器初始化 */
			
        adxl345_init(name);
			initialized = RT_TRUE;
    }
    if (initialized)
    {
			
			adxl345_tfr(i2c_bus,ADXL345_READ,ADXL345_DATAX0, 6,buf);
			rt_kprintf(" data is %x %x %x %x %x %x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
			
        /* 读取温湿度数据 */
     //   ADXL345_read(&temperature, &humidity);

 //       rt_kprintf("read aht10 sensor humidity   : %d.%d %%\n", (int)humidity, (int)(humidity * 10) % 10);
   //     rt_kprintf("read aht10 sensor temperature: %d.%d \n", (int)temperature, (int)(temperature * 10) % 10);
    }
    else
    {
        rt_kprintf("initialize sensor failed!\n");
    }
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(i2c_sensor_sample, i2c sensor sample);
