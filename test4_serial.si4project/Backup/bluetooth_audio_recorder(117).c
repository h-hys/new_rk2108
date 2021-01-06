/*
 * $ Copyright Cypress Semiconductor $
 */
#include "wiced.h"
#include "wiced_audio.h"
#include "bt_types.h"
#include "bluetooth_audio.h"
#ifdef USE_MEM_POOL
#include "mem_pool.h"
#endif

#include "wwd_debug.h"
/*****************************************************************************
**
**  Name:           bta_wiced_audio_player.c
**
**  Description:    BTA AVK interface to the Wiced audio subsystem
**
*****************************************************************************/


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define BT_SCO_OUTPUT_BUFFER_SIZE                    60*2 //30 samples of 2 bytes each for two channels

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

bt_audio_context_t recorder;

static int mute_flag = 0;

#ifdef USE_MEM_POOL
extern bt_buffer_pool_handle_t  mem_pool_pcm;
#endif

void mute_control()
{
	mute_flag = 1&(~mute_flag);
}

/******************************************************
*               Function Definitions
******************************************************/
void bt_audio_tx_recorder_data_to_sco(bt_audio_codec_data_t *pcm)
{
    uint16_t *p_sco_data = (uint16_t *)(pcm->data + pcm->offset);
    uint16_t len = pcm->length;
    int i = 0, j = 0;

    do
    {
        p_sco_data[j++] = p_sco_data[i];
        i += 2;
    }
    while (i < len);
    pcm->length = len / 2;

    bt_audio_hfp_send_sco_data(pcm);
}

void write_bluetooth_audio_data(uint8_t *buffer, uint16_t size)
{
    static bt_audio_codec_data_t *pcm;
    uint8_t        *pdata = NULL;
    uint32_t        cpy_len = 0;
    uint32_t        remained_bytes_length = size;
#ifdef PUSH_SCO_TO_QUEUE
    wiced_result_t result;
#endif

    while (remained_bytes_length > 0)
    {
        if (!pcm)
        {
#ifdef USE_MEM_POOL
            pcm = bt_buffer_pool_allocate_buffer(mem_pool_pcm);
#else
            pcm = rt_malloc(sizeof(bt_audio_codec_data_t) + BT_SCO_OUTPUT_BUFFER_SIZE);
#endif
            if (pcm == NULL)
            {
                return;
            }
            pcm->offset = 0;
            pcm->length = 0;
        }

        pdata = pcm->data + pcm->length;
        cpy_len = (remained_bytes_length >= (BT_SCO_OUTPUT_BUFFER_SIZE - pcm->length)) ? (BT_SCO_OUTPUT_BUFFER_SIZE - pcm->length) : remained_bytes_length;
        memcpy(pdata, buffer + (size - remained_bytes_length), cpy_len);
        remained_bytes_length -= cpy_len;
        pcm->length += cpy_len;

        if (pcm->length == BT_SCO_OUTPUT_BUFFER_SIZE)
        {
#ifdef PUSH_SCO_TO_QUEUE
            result = wiced_rtos_push_to_queue(&recorder.queue, &pcm, WICED_NO_WAIT);
            if (result != WICED_SUCCESS)
            {
#ifdef USE_MEM_POOL
                bt_buffer_pool_free_buffer(pcm);
#else
                free(pcm);
#endif
            }
            pcm = NULL;
#else
            bt_audio_tx_recorder_data_to_sco(pcm);
            pcm->offset = 0;
            pcm->length = 0;
#endif
        }
    }
}

extern uint32_t rk_recorder_capture_sample(char *ptr, int len);
extern bool get_record_flag();
static uint8_t r_audio_buffer[12 * 1024];

#define my_debug 1

void bta_wiced_audio_recorder_task(uint32_t context)
{
    uint8_t          *ptr;
    uint16_t          n;
    wiced_result_t    result;
    uint16_t          available_bytes;
    uint32_t   flags_set;

	#ifdef my_debug
	FILE *file = RT_NULL;
	uint8_t tmp_buf[4 * 1024] = {0}; 
	int i = 0, j = 0, id = 0;
	char buf[50] = {0};	
  	#endif
	
    for (;;)
    {

        result = wiced_rtos_wait_for_event_flags(&recorder.events, BT_AUDIO_EVENT_ALL, &flags_set, WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_WAIT_FOREVER);
        if (result != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("[BT-RECORDER] error in wait for event... \n"));
            continue;
        }

        if ((flags_set & BT_AUDIO_EVENT_DEINIT) == BT_AUDIO_EVENT_DEINIT)
        {
            WPRINT_APP_INFO(("[BT-RECORDER] exit bluetooth record task \n"));
            break;
        }

        WPRINT_APP_INFO(("[BT-RECORDER] Start task... (flags_set 0x%x)\n", flags_set));
		rt_kprintf("record while(1) ..................\n");
		
		if(get_record_flag())
		{
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "/mnt/sdcard/record_%d.wav", id++);
			file = fopen(buf, "w+");
		    if (!file)
		    {
		        rt_kprintf("Unable to create file %s\n", buf);
		    }
		}
		
        while (!(flags_set & BT_AUDIO_EVENT_STOP_LOOP))
        {
            //ensure that recorder is initialized at this point of time.
            uint32_t wait_time = ((NUM_USECONDS_IN_SECONDS / recorder.bluetooth_audio_config.sample_rate) * (BT_AUDIO_DEFAULT_PERIOD_SIZE / (recorder.bluetooth_audio_config.channels))) / NUM_MSECONDS_IN_SECONDS;
            available_bytes = 1024; //BT_AUDIO_DEFAULT_PERIOD_SIZE;            
            ptr = r_audio_buffer;
			j = 0;
            //APPL_TRACE_ERROR0( "[BT-RECORDER] wait buffer done..." );
            while (available_bytes > 0)
            {
                /* request for at least for a period size */
                n = available_bytes;
                if (wiced_rtos_wait_for_event_flags(&recorder.events, BT_AUDIO_EVENT_STOP_LOOP, &flags_set, WICED_FALSE, WAIT_FOR_ANY_EVENT, WICED_NO_WAIT) == WICED_SUCCESS)
                {
                    if ((flags_set & BT_AUDIO_EVENT_STOP_LOOP) == BT_AUDIO_EVENT_STOP_LOOP)
                    {
                        break;
                    }
                }

                rk_recorder_capture_sample(ptr, n /*/ 4*/);
				if(get_record_flag())
				{
					if (fwrite(ptr, 1, n*12, file) != n*12)
			        {
			            rt_kprintf("Error capturing sample\n");
			        }	
					/*if (fwrite(tmp_buf, 1, j, file) != j)
			        {
			            rt_kprintf("Error capturing sample\n");
			        }*/
				}
				
				for(i = 0; i < n*12; i+=12)
				{
					tmp_buf[j++] = ptr[i+0];
					tmp_buf[j++] = ptr[i+1];
					tmp_buf[j++] = ptr[i+2];
					tmp_buf[j++] = ptr[i+3];
				}							
				
				if(mute_flag != 1) 
                	write_bluetooth_audio_data(tmp_buf, 1024*4);
				
                available_bytes -= n;
            }
            result = wiced_rtos_wait_for_event_flags(&recorder.events, BT_AUDIO_EVENT_ALL, &flags_set, WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_NO_WAIT);
        }
        bt_audio_stop(BT_AUDIO_DEVICE_RECORDER);
		#ifdef my_debug
		if(file)
			fclose(file);
		#endif
        WPRINT_APP_INFO(("%s: Out of RECORDER loop\n", __func__));
    }
}
