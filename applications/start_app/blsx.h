
#ifndef _BLSX_H_
#define _BLSX_H_

#define RT_DEVICE_CTRL_WDT_GET_TIMEOUT    (1) /* get timeout(in seconds) */
#define RT_DEVICE_CTRL_WDT_SET_TIMEOUT    (2) /* set timeout(in seconds) */
#define RT_DEVICE_CTRL_WDT_GET_TIMELEFT   (3) /* get the left time before reboot(in seconds) */
#define RT_DEVICE_CTRL_WDT_KEEPALIVE      (4) /* refresh watchdog */
#define RT_DEVICE_CTRL_WDT_START          (5) /* start watchdog */
#define RT_DEVICE_CTRL_WDT_STOP           (6) /* stop watchdog */

#define BT_UART_NAME1 "/dev/uart0"

#define rt_printf(format, ...)    rt_kprintf("[%s:%d] " format " \r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

typedef enum
{
    TYPE_POWER = 1,		//电源控制
	TYPE_RECORD,	//录音控制
	TYPE_MUTE,		//静音控制
	TYPE_VOL_ADD,	//增加音量
	TYPE_VOL_SUB,	//减少音量
	YTPE_MAX,
}cmd_type;

typedef enum
{
	PLAYER_START = 1,		//播放
	PLAYER_PAUSE,		//暂停
	PLAYER_RESUME,		//恢复播放
	PLAYER_STOP,		//终止
	PLAYER_MAX,	
}player_type_e;

typedef enum
{
	RECORD_START = 1,	//开始录音
	RECORD_PAUSE,		//录音暂停
	RECORD_RESUEM,		//录音恢复
	RECORD_STOP,		//终止录音
	RECORD_MAX,
}record_type_e;

typedef enum
{
	MUTE_TYPE,		//静音
	UNMUTE_ETYPE,	//非静音
}mute_e;

//灯的状态
typedef enum
{
	LED_RECORD = 1, 	//录音灯
	LED_MUTE,			//静音灯
	LED_BT_PAIR,		//蓝牙匹配
	LED_BATTERY,		//充电	
}led_state_e;

//灯的控制方向
typedef enum
{
	LED_NEGAT = 1,    //取反
	LED_CLOSE,		  //灭灯
	LED_OPEN,		  //点亮
}led_ctr_e;

typedef enum
{
    WICED_BT_HFP_HF_STATE_DISCONNECTED, /**< HF control connection is closed */
    WICED_BT_HFP_HF_STATE_CONNECTED,    /**< HF control connection established */
    WICED_BT_HFP_HF_STATE_SLC_CONNECTED /**< HF synchronized with AG's state, ready to send/recive commands/notifications */
} wiced_bt_hfp_hf_connection_state_t;

void blsx_entry();
void app_led_control(led_state_e led_state, led_ctr_e led_ctr);

#endif
