

#include <rtthread.h>
#include <rthw.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <dfs_posix.h>
#include <dfs_select.h>
//#include <termios.h>
#include "drv_gpio.h"
#include "drv_heap.h"
#include "hal_base.h"
#include "drv_gpio.h"  
#include "rtdef.h"
#include "drv_keyctrl.h"
#include "rk_audio.h"
#include "blsx.h"
#include "rtdef.h"
#include "iomux.h"
#include "hal_base.h"
#include <drivers/fuel_gauge.h>

#define LOG_TAG "blxs"
#define LOG_LVL LOG_LVL_DBG

#include <ulog.h>
  
    
#define RT_TIMER_THREAD_STACK_SIZE 1024*2

static rt_device_t wdg_dev;    /* 看门狗设备句柄 */
//static rt_mq_t msg_queue = {0};	

static rt_uint8_t rt_thread_stack[RT_TIMER_THREAD_STACK_SIZE];
static rt_uint8_t rt_thread_stack2[512];

static struct rt_event event;

extern int bt_get_hfp_status();
extern int bt_a2dp_play_status();
extern int bt_connect_status();
extern int tinyplay_audio();	//开始录音的函数(文件的路径暂时被写死了)
extern int tinycap_audio();		//开始播放的函数(文件的路径在那时被写死了)
//extern void capture_state_set(record_type_e data); //录音时的控制函数
//extern void player_state_set(player_type_e data);	//播放时的控制函数
extern void application_start();
extern int rk_snd_card_init(void);
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
extern int vol_sub();		
extern int vol_add();
extern int vol_add_record();
extern int vol_sub_record();
#endif
//功放(静音)控制函数，eg: falg为真，静音；反之不静音。
void app_mute_control(mute_e flag)
{
	int ret = 0;
	ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_B1);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, 1&(~ret));
	
	/*if(flag == MUTE_TYPE)
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, GPIO_LOW);
	else 
		HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B1, GPIO_HIGH);*/
}

//3个LED灯的控制
void app_led_control(led_state_e led_state, led_ctr_e led_ctr)
{
	int ret = 0;

	switch(led_state)
	{
		case LED_RECORD:
			HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_B0, PIN_CONFIG_MUX_FUNC0);
			HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_B0, GPIO_OUT);
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_B0); //red 1
			if(led_ctr == LED_NEGAT)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B0, 1&(~ret));
			else if(led_ctr == LED_CLOSE)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B0, 1);
			else if(led_ctr == LED_OPEN)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B0, 0);
			break;
			
		case LED_MUTE:
			HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_D0, PIN_CONFIG_MUX_FUNC0);
			HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_D0, GPIO_OUT);
			ret = HAL_GPIO_GetPinData(GPIO1, GPIO_PIN_D0); //red
			if(led_ctr == LED_NEGAT)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_D0, 1&(~ret));
			else if(led_ctr == LED_CLOSE)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_D0, 1);
			else if(led_ctr == LED_OPEN)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_D0, 0);
			break;
			
		case LED_BATTERY:
			HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_A7, PIN_CONFIG_MUX_FUNC0);
			HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A7, GPIO_OUT);
			ret = HAL_GPIO_GetPinData(GPIO1, GPIO_PIN_A7); //red 1
			if(led_ctr == LED_NEGAT)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A7, 1&(~ret));
			else if(led_ctr == LED_CLOSE)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A7, 0);
			else if(led_ctr == LED_OPEN)
				HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A7, 1);
			break;

		case LED_BT_PAIR:
			HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D7, PIN_CONFIG_MUX_FUNC0);
			HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D7, GPIO_OUT);
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_D7); //blue 1
			if(led_ctr == LED_NEGAT)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D7, 1&(~ret));
			else if(led_ctr == LED_CLOSE)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D7, 1);
			else if(led_ctr == LED_OPEN)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D7, 0);
			break;

		/*case 5: //暂时没用
			HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_C3, PIN_CONFIG_MUX_FUNC0);
			HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_C3, GPIO_OUT);
			ret = HAL_GPIO_GetPinData(GPIO0, GPIO_PIN_C3); //blue
			HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C3, 1&(~ret));
			if(led_ctr == LED_NEGAT)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C3, 1&(~ret));
			else if(led_ctr == LED_CLOSE)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C3, 1);
			else if(led_ctr == LED_OPEN)
				HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C3, 0);
			break;*/
			
		default:
			LOG_E("app_led_control error");
			break;
	}
	//rt_printf("ret is %d\n", ret);
}


//发送消息
static void app_key_doing(char num)
{
	cmd_type data = 0;
	
	rt_printf("key is %d\n", num);

	//短按是1-4，长按是8-11
	if(num == 0x1)
	{		
		data = TYPE_VOL_ADD;			 
	}
	else if(num == 0x2)
	{
		data = TYPE_VOL_SUB;
	}
	else  if(num == 0x3)
	{
		data = TYPE_MUTE;
	}
	else  if(num == 0x4 || num == 0x5)
	{
		data = TYPE_RECORD;
	}
	else  if(num == 0x8)
	{
		data = 8;
	}
	else  if(num == 0x9)
	{
		data = 9;
	}
	else  if(num == 0xa)
	{
		data = 10;
	}
	else  if(num == 0xb)
	{
		data = 11;
	}
	else
	{
		data = YTPE_MAX;
	}
	
	rt_event_send(&event, data);	
}


//按键触发时的回调函数
static rt_err_t app_key_read_data_cb(rt_device_t dev, rt_size_t size)
{
    char ch;
    rt_size_t i;
    rt_err_t ret = RT_EOK;
	//rt_printf("size is %d\n", size);
		
    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            //rt_printf("dev : %s read 0x%x\n", dev->parent.name, ch);
			app_key_doing(ch);						
        }
        else
        {
            ret = rt_get_errno();
            LOG_E("dev : %s read error %d", dev->parent.name, ret);
        }
    }

    return ret;
}

rt_device_t keyctrl_dev = RT_NULL;

//打开按键的字符设备，用于检测按键的值
static int app_key_open_device(char * pcName)
{
	rt_err_t result;
    struct rt_keyctrl_info info;
	

	//通过名字查找list_device命令里面的设备
	keyctrl_dev = rt_device_find(pcName);
	
	RT_ASSERT(keyctrl_dev != RT_NULL);
	
	//获取设备信息
	result = rt_device_control(keyctrl_dev, RT_KEYCTRL_CTRL_GET_INFO, &info);
    if (result != RT_EOK)
    {
        LOG_E("device : %s cmd RT_KEYCTRL_CTRL_GET_INFO failed\n",
                   keyctrl_dev->parent.name);
        return result;
    }

	result = rt_device_init(keyctrl_dev);
    if (result != RT_EOK)
    {
        LOG_E("To initialize device:%s failed. The error code is %d\n",
                   keyctrl_dev->parent.name, result);
        return result;
    }

    result = rt_device_open(keyctrl_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (result != RT_EOK)
    {
        LOG_E("To open device:%s failed. The error code is %d\n",
                   keyctrl_dev->parent.name, result);
        return result;
    }

	//设置按键的回调函数
    rt_device_set_rx_indicate(keyctrl_dev, app_key_read_data_cb);

    return RT_EOK;
	
}

//（默认是点亮的，设置低电平就是点亮，高电平就是熄灭）
static void app_gpio_init()
{
	//红灯 录音状态：录音中（灯亮），录音停止（灯灭）
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_B0, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_B0, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_B0, 1);
    //红灯 充电灯，充电中亮灯，冲完灭灯 (这个高电平亮灯，低电平灭灯)
	HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_A7, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A7, GPIO_OUT);
	//HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A7, 1);
	
	//红灯 Mute状态：Mute禁音（灯亮），Mute取消（灯灭）	
	HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_D0, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_D0, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_D0, 1);
	
	//功放引脚 
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_B1, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_B1, GPIO_OUT);

	//蓝灯 蓝牙配对状态灯：配对中（闪烁200mS），配对成功(灭灯）
	HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D7, PIN_CONFIG_MUX_FUNC0); 
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D7, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D7, 1);
	

	//蓝灯 开机与USB连接状态：开机中与USB连接（灯亮），关机及断开USB连接（灯灭）
	/*HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_C3, PIN_CONFIG_MUX_FUNC0);
	HAL_GPIO_SetPinDirection(GPIO_BANK0, GPIO_PIN_C3, GPIO_OUT);
	HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C3, 0);*/
}

static void wakeup_isr(void *args)
{
    rt_interrupt_enter();
	
	//if(rt_current_pm_state() != 1)
	{
		rt_pm_request(1);
	}
    rt_interrupt_leave();
}

void gpio_interrupt_init(void)
{
    rt_err_t ret;
    struct GPIO_REG *gpio = GPIO0;

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_D2, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D2, GPIO_IN);
    ret = rt_pin_attach_irq(BANK_PIN(GPIO_BANK0, 26), PIN_IRQ_MODE_RISING_FALLING, wakeup_isr, "wakeup_isr");
    RT_ASSERT(ret == RT_EOK);
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANK0, 26),  PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);

    gpio->INT_EN_L = 0x02000200;
}


void wdg_keepalive(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}

//看门狗的初始化
int app_wdt_init()
{
	
	rt_uint32_t timeout = 1;       /* 溢出时间，单位：秒*/
	int type  = 0;
	
	/* 根据设备名称查找看门狗设备，获取设备句柄 */
	wdg_dev = rt_device_find("dw_wdt");
	if(wdg_dev == NULL)
	{
		LOG_E("no find wdt device, %s", __func__);
		return RT_ERROR;
	}
	/* 初始化设备 */
	rt_device_init(wdg_dev);

	/* 设置看门狗溢出时间 */
	rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, (void *)timeout);

	rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, &type);
	
	/* 设置空闲线程回调函数 */
	rt_thread_idle_sethook(wdg_keepalive);
	
	return RT_EOK;
}

struct TIMER_REG *pTimer1 = TIMER0;
void pmic_power_off(void);
static int power_down_flag = 0;
void timer_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    pTimer1->INTSTATUS = 0x01;
    rt_kprintf("suspend count: %d\n", rt_tick_get());
	rt_pm_request(1);
	power_down_flag = 1;
	
	rt_interrupt_leave();    
}

static void timer_wakeup()
{
    int sec_to_count = 0;
	
    sec_to_count = 20 * 1000*1000*24;

    HAL_NVIC_SetIRQHandler(TIMER0_IRQn, (NVIC_IRQHandler) & timer_IRQHandler);
    HAL_NVIC_EnableIRQ(TIMER0_IRQn);

    rt_kprintf("timer0 auto wake up:\n");
    pTimer1->INTSTATUS = 0x01;
    pTimer1->CONTROLREG = 0x00;
    pTimer1->LOAD_COUNT[0] = sec_to_count;
    pTimer1->LOAD_COUNT[1] = 0x0000;
    pTimer1->CONTROLREG = 0x05;

    return;
}

int charger_vol_get()
{	
	rt_device_t device;
	struct rt_fg_status st;

	device = rt_device_find("charger");

	device = rt_device_find("fuel_gauge");
	if (device == RT_NULL)
	{
		return -RT_ERROR;
	}

	rt_device_control(device, RT_DEVICE_CTRL_FG_GET, &st);
	//rt_kprintf("soc %d, fcc %d, current %d, voltage %d, present %d\n", 
	//	st.soc, st.fcc, st.current, st.voltage, st.present);

	rt_device_close(device);

	return st.voltage;
}


static void app_thread_entry(void *parameter)
{
	
	uint32_t ret = -1;

	rt_thread_mdelay(200);

	//打开按键检测
	app_key_open_device("keyctrl");	
	
	//打开看门狗
	app_wdt_init();
		
	//if (access(RT_SDCARD_MOUNT_POINT, F_OK) < 0)
	//	mkdir(RT_SDCARD_MOUNT_POINT, 0);
	
    /* 初始化事件对象 */
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);

	/*for(i = 0; i < 10; i++)
	{
		rt_thread_mdelay(1000);
		if((ret = rt_hw_codec_es8311_init()) == RT_EOK)
		{
			LOG_I("es8311 init suc, count is %d\n", i+1);
			break;
		}
		else if(ret == -1)
		{
			LOG_I("es8311 already init\n");
			break;
		}
			
	}*/

	//rt_thread_mdelay(10);
	//rk_snd_card_init();

	//蓝牙初始化并打开
	application_start();

	app_gpio_init();

	LOG_I("hello world!, %s\n", __func__);
		
    while (1)
    {
    	
    	//rt_thread_mdelay(1000);
    	
    	#if 1
    	//等待消息
		rt_event_recv(&event, TYPE_POWER|TYPE_RECORD|TYPE_MUTE|TYPE_VOL_ADD|TYPE_VOL_SUB | 8,
						RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      	RT_WAITING_FOREVER, &ret);
		//rt_kprintf("ret is %d\n", ret);
		switch(ret)
		{
			case TYPE_POWER:
				LOG_D("power control");
				break;

			case TYPE_RECORD:
				LOG_D("record control");
				tinycap_audio();
				app_led_control(LED_RECORD, LED_NEGAT);
				break;

			case TYPE_MUTE:
				LOG_D("mute control play");
				tinyplay_audio();
				app_led_control(LED_MUTE, LED_NEGAT);
				break;

			case TYPE_VOL_ADD:
				LOG_D("add vol control");
				
				#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL	
				vol_add();
				#endif
				//vol_add_record();
				break;
			
			case TYPE_VOL_SUB:
				LOG_D("sub vol control");
				#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
				vol_sub();
				#endif
				//vol_sub_record();
				break;
			
			case 8:
				//rt_hw_cpu_reset();
				//system("reboot");
				break;
				
			case 9:
				//app_mute_control(0);
				break;
				
			case 10:
				break;
				
			case 11:
				break;
				
			default:
				LOG_E("have not this select");
				break;
		}		
		
		#endif				
    }
}

static void app_thread_entry2(void *parameter)
{
	int interval_flag = 0;
	rt_tick_t start_tick = rt_tick_get();
	rt_tick_t end_tick = rt_tick_get();
	int check_vol = 0;
	int vol_full = 0;
	
	while(1)
	{
		rt_thread_mdelay(1000);
		if(power_down_flag)
		{
			pmic_power_off();
		}
		
		end_tick = rt_tick_get();

		if(!bt_connect_status())
		{
			if(interval_flag == 1)
			{
				interval_flag = 0;
				start_tick = end_tick;
			}
			
			//五分钟内没有连接上蓝牙，切断电源
			if(end_tick - start_tick > 1000 * 60)
			{
				//rt_kprintf("not connect bt device!\n");
				//rt_kprintf("start_tick is %d, end_tick is %d\n", start_tick, end_tick);
				start_tick = end_tick;
			}
		}
		else 
		{
			if(interval_flag == 0)
			{
				interval_flag = 1;
				start_tick = end_tick;
			}
			
			//连接上蓝牙，20分钟没有任何操作，休眠；一小时没有操作，切断电源		
			if(!(bt_a2dp_play_status() || bt_get_hfp_status()))			
			{				
				if(end_tick - start_tick > 1000 * 10)
				{					
					rt_kprintf("connect bt, but not doing anything\n");
					rt_kprintf("start_tick is %d, end_tick is %d\n", start_tick, end_tick);
					start_tick = end_tick;
					timer_wakeup();
					rt_pm_release(1);
				}
			}
			else 
			{
				rt_kprintf("connect bt, doing now!!!\n");
				start_tick = end_tick;
			}
		}

		//检测电池电量
		check_vol++;
		if(check_vol >= 5)
		{
			check_vol = 0;
			if(charger_vol_get() >= 4177)
			{
				vol_full++;
				if(vol_full >= 3)
				{
					vol_full = 0;
					//rt_kprintf("voltage is full\n");
					//HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A7, 0);
				}
				
			}
		}
		
	}
	
}


//所有的线程都可以在这里面创建
void app_thread_init(void)
{
	static struct rt_thread app;
	static struct rt_thread app2;
	
    /* initialize thread */
    rt_thread_init(&app,
                   "app",
                   app_thread_entry,
                   RT_NULL,
                   &rt_thread_stack[0],
                   sizeof(rt_thread_stack),
                   RT_THREAD_PRIORITY_MAX - 5,
                   50);

	/* initialize thread */
    rt_thread_init(&app2,
                   "app2",
                   app_thread_entry2,
                   RT_NULL,
                   &rt_thread_stack2[0],
                   sizeof(rt_thread_stack2),
                   RT_THREAD_PRIORITY_MAX - 1,
                   50);
		
    /* startup */
    rt_thread_startup(&app);
	//rt_thread_startup(&app2);
	
	LOG_I("create task suc! \r\n");
	
}


void blsx_entry()
{	
	//i/o初始化		
	//app_gpio_init();

	//唤醒按键中断初始化
	//gpio_interrupt_init();
	
	//创建线程
	app_thread_init();
}



