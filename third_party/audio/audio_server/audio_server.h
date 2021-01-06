/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 */

#ifndef _AUDIO_SERVER_H_
#define _AUDIO_SERVER_H_

typedef void *audio_player_semaphore_handle;
typedef void *audio_player_mutex_handle;
typedef void *audio_player_thread_handle;
typedef void *audio_player_timer_handle;
struct audio_player_queue;
struct audio_player_stream;

#ifndef bool
#define bool    _Bool
#endif

struct audio_config
{
    int sample_rate;
    int bits;
    int channels;
    int32_t bitrate;
};

/*
 * playback_device
 */
typedef struct
{
    int samplerate;
    int bits;
    int channels;
    const char *card_name;
    const char *device_name;
    int frame_size;
} playback_device_cfg_t;

struct playback_device
{
    int (*open)(struct playback_device *self, playback_device_cfg_t *cfg);
    int (*start)(struct playback_device *self);
    int (*write)(struct playback_device *self, const char *data, size_t data_len);
    int (*stop)(struct playback_device *self);
    int (*abort)(struct playback_device *self);
    void (*close)(struct playback_device *self);
    void *userdata;
};

typedef struct playback_device playback_device_t;

/*
 * play_preprocessor
 */
typedef struct
{
    char *target;
    const char *tag;
    int frame_size;
    char type[32];
    char isOta;
    uint32_t seek_pos;
    uint32_t file_size;
} play_preprocessor_cfg_t;

struct play_preprocessor
{
    const char *type;
    int (*init)(struct play_preprocessor *self, play_preprocessor_cfg_t *cfg);
    int (*read)(struct play_preprocessor *self, char *data, size_t data_len);
    int (*seek)(struct play_preprocessor *self, play_preprocessor_cfg_t *cfg);
    void (*destroy)(struct play_preprocessor *self);
    void *userdata;
};

typedef struct play_preprocessor play_preprocessor_t;

/*
 * play_decoder
 */
typedef int (*decode_input_t)(void *userdata, char *data, size_t data_len);
typedef int (*decode_output_t)(void *userdata, char *data, size_t data_len);
typedef int (*decode_post_t)(void *userdata, int samplerate, int bits, int channels);

typedef struct
{
    decode_input_t input;
    decode_output_t output;
    decode_post_t post;
    uint32_t start_time;
    void *userdata;
} play_decoder_cfg_t;

typedef enum
{
    PLAY_DECODER_SUCCESS = 0,
    PLAY_DECODER_INPUT_ERROR,
    PLAY_DECODER_OUTPUT_ERROR,
    PLAY_DECODER_DECODE_ERROR,
} play_decoder_error_t;

struct play_decoder
{
    struct play_decoder *next;
    uint8_t support_seek;
    char *type;
#if  CONFIG_FWANALYSIS_SEGMENT
    char segment;
#endif
    int (*init)(struct play_decoder *self, play_decoder_cfg_t *cfg);
    play_decoder_error_t (*process)(struct play_decoder *self);
    bool (*get_post_state)(struct play_decoder *self);
    void (*destroy)(struct play_decoder *self);
    void *userdata;
};

typedef struct play_decoder play_decoder_t;
#define    MP3_DECODER_TPYE "mp3"
#define    AMR_DECODER_TYPE "amr"

typedef enum
{
    TYPE_AUDIO_MP3_DEC,
    TYPE_AUDIO_AMR_DEC,
    TYPE_AUDIO_AMR_ENC,
    TYPE_AUDIO_SPEEX_ENC,
    TYPE_AUDIO_OPUS_ENC,
    TYPE_AUDIO_OPUS_DEC,

    TYPE_DATA_MAX
} audio_data_type;

typedef enum _MSGBOX_SYSTEM_CMD
{
    MEDIA_MSGBOX_CMD_DECODE_OPEN,
    MEDIA_MSGBOX_CMD_DECODE_OPEN_CMPL,

    MEDIA_MSGBOX_CMD_DECODE,
    MEDIA_MSGBOX_CMD_DECODE_CMPL,

    MEDIA_MSGBOX_CMD_DECODE_CLOSE,
    MEDIA_MSGBOX_CMD_DECODE_CLOSE_CMPL,

    MEDIA_MSGBOX_CMD_ENCODE_OPEN,
    MEDIA_MSGBOX_CMD_ENCODE_OPEN_CMPL,

    MEDIA_MSGBOX_CMD_ENCODE,
    MEDIA_MSGBOX_CMD_ENCODE_CMPL,

    MEDIA_MSGBOX_CMD_ENCODE_FINISH,
    MEDIA_MSGBOX_CMD_ENCODE_FINISH_CMPL,

    MEDIA_MSGBOX_CMD_ENCODE_CLOSE,
    MEDIA_MSGBOX_CMD_ENCODE_CLOSE_CMPL,

    MEDIA_MSGBOX_CMD_ASR_OPEN,
    MEDIA_MSGBOX_CMD_ASR_OPEN_CMPL,

    MEDIA_MSGBOX_CMD_ASR_PROCESS,
    MEDIA_MSGBOX_CMD_ASR_PROCESS_CMPL,

    MEDIA_MSGBOX_CMD_ASR_CLOSE,
    MEDIA_MSGBOX_CMD_ASR_CLOSE_CMPL,

    MEDIA_MSGBOX_CMD_MAX,

} MSGBOX_SYSTEM_CMD;

#define    AUDIO_SERVER_DATA_SHARE_MAX 2
struct audio_server_data_share
{
    size_t type;
    size_t dat[AUDIO_SERVER_DATA_SHARE_MAX];
};

/*
 * player
 */
typedef struct player *player_handle_t;

typedef enum
{
    PLAY_INFO_SUCCESS = 0,
    PLAY_INFO_PREPROCESS,
    PLAY_INFO_DECODE,
    PLAY_INFO_PAUSED,
    PLAY_INFO_RESUMED,
    PLAY_INFO_IDLE,
    PLAY_INFO_STOP,
} play_info_t;

typedef enum
{
    PLAY_FREQ_NULL = 0,
    PLAY_FREQ_LOCALPLAY,
    PLAY_FREQ_NETPLAY,
    PLAY_FREQ_MAX,
} play_freq_info_t;

typedef void (*player_listen_cb)(player_handle_t self, play_info_t info, void *userdata);

typedef struct
{
    const char *tag;
    char *name;
    playback_device_t device;
    size_t preprocess_buf_size;
    size_t decode_buf_size;
    uint32_t preprocess_stack_size;
    uint32_t decoder_stack_size;
    uint32_t playback_stack_size;

    player_listen_cb listen;
    int resample_rate;
    void *userdata;
} player_cfg_t;

typedef struct
{
    char *target;
    bool need_free;
    play_preprocessor_t preprocessor;

    int samplerate;
    int bits;
    int channels;
    uint32_t start_time;

    int freq_t;
    int enable_reverb;
    int enable_mix;
    int info_only;
} play_cfg_t;

typedef enum
{
    PLAYER_STATE_IDLE = 0,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_PAUSED,
    PLAYER_STATE_ERROR,
    PLAYER_STATE_STOP
} player_state_t;

uint32_t player_get_target_and_seek(player_handle_t self, char *file_name);
void player_set_seek(long offset);
__attribute((visibility("default"))) int player_init(void);
__attribute((visibility("default"))) int player_list_decoder(void);
__attribute((visibility("default"))) int player_register_decoder(const char *type, play_decoder_t *decoder);
__attribute((visibility("default"))) int player_unregister_decoder(const char *type);
__attribute((visibility("default"))) player_handle_t player_create(player_cfg_t *cfg);
__attribute((visibility("default"))) int player_play(player_handle_t self, play_cfg_t *cfg);
__attribute((visibility("default"))) player_state_t player_get_state(player_handle_t self);
__attribute((visibility("default"))) int player_stop(player_handle_t self);
__attribute((visibility("default"))) int player_device_stop(player_handle_t self, int wait);
__attribute((visibility("default"))) int player_pause(player_handle_t self);
__attribute((visibility("default"))) int player_resume(player_handle_t self);
__attribute((visibility("default"))) int player_wait_idle(player_handle_t self);
__attribute((visibility("default"))) int player_close(player_handle_t self);
__attribute((visibility("default"))) void player_destroy(player_handle_t self);
__attribute((visibility("default"))) void player_deinit(void);
__attribute((visibility("default"))) int player_get_cur_time(player_handle_t self);
__attribute((visibility("default"))) void player_set_total_time(player_handle_t self, uint32_t time);
__attribute((visibility("default"))) uint32_t player_get_total_time(player_handle_t self);
__attribute((visibility("default"))) uint32_t player_get_file_length(player_handle_t self);
__attribute((visibility("default"))) int player_preprocess_seek(player_handle_t self, uint32_t pos);

int playback_device_open_impl(struct playback_device *self, playback_device_cfg_t *cfg);
int playback_device_start_impl(struct playback_device *self);
int playback_set_volume(int vol);
int playback_get_volume(void);
int playback_device_write_impl(struct playback_device *self, const char *data, size_t data_len);
int playback_device_stop_impl(struct playback_device *self);
int playback_device_abort_impl(struct playback_device *self);
void playback_device_close_impl(struct playback_device *self);

#define DEFAULT_PLAYBACK_DEVICE { \
        .open = playback_device_open_impl, \
        .start = playback_device_start_impl, \
        .write = playback_device_write_impl, \
        .stop = playback_device_stop_impl, \
        .abort = playback_device_abort_impl, \
        .close = playback_device_close_impl, \
    }

__attribute((visibility("default"))) int file_preprocessor_init_impl(struct play_preprocessor *self, play_preprocessor_cfg_t *cfg);
__attribute((visibility("default"))) int file_preprocessor_read_impl(struct play_preprocessor *self, char *data, size_t data_len);
__attribute((visibility("default"))) int file_preprocessor_seek_impl(struct play_preprocessor *self, play_preprocessor_cfg_t *cfg);
__attribute((visibility("default"))) void file_preprocessor_destroy_impl(struct play_preprocessor *self);

#define DEFAULT_FILE_PREPROCESSOR { \
        .type = "file", \
        .init = file_preprocessor_init_impl, \
        .read = file_preprocessor_read_impl, \
        .seek = file_preprocessor_seek_impl, \
        .destroy = file_preprocessor_destroy_impl \
    }

__attribute((visibility("default"))) int http_preprocessor_init_impl(struct play_preprocessor *self, play_preprocessor_cfg_t *cfg);
__attribute((visibility("default"))) int http_preprocessor_read_impl(struct play_preprocessor *self, char *data, size_t data_len);
__attribute((visibility("default"))) void http_preprocessor_destroy_impl(struct play_preprocessor *self);

#define DEFAULT_HTTP_PREPROCESSOR { \
        .type = "http", \
        .init = http_preprocessor_init_impl, \
        .read = http_preprocessor_read_impl, \
        .seek = NULL, \
        .destroy = http_preprocessor_destroy_impl \
    }

/*
 * record_encoder
 */
typedef int (*encode_input_t)(void *userdata, char *data, size_t data_len);
typedef int (*encode_output_t)(void *userdata, const char *data, size_t data_len);
typedef int (*encode_post_t)(void *userdata, int samplerate, int bits, int channels);

typedef struct
{
    encode_input_t input;
    encode_output_t output;
    encode_post_t post;
    int header_size;
    void *userdata;
} record_encoder_cfg_t;

typedef enum
{
    RECORD_ENCODER_SUCCESS = 0,
    RECORD_ENCODER_INPUT_ERROR,
    RECORD_ENCODER_OUTPUT_ERROR,
    RECORD_ENCODER_ENCODE_ERROR,
} record_encoder_error_t;

struct record_encoder
{
    struct record_encoder *next;
    char *type;
    int (*init)(struct record_encoder *self, record_encoder_cfg_t *cfg);
    record_encoder_error_t (*process)(struct record_encoder *self);
    bool (*get_post_state)(struct record_encoder *self);
    void (*destroy)(struct record_encoder *self);
    void *userdata;
};

typedef struct record_encoder record_encoder_t;

/*
 * capture_device
 */
typedef struct
{
    int samplerate;
    int bits;
    int channels;
    const char *device_name;
    int frame_size;
} capture_device_cfg_t;

struct capture_device
{
    int (*open)(struct capture_device *self, capture_device_cfg_t *cfg);
    int (*start)(struct capture_device *self);
    int (*read)(struct capture_device *self, const char *data, size_t data_len);
    int (*stop)(struct capture_device *self);
    int (*abort)(struct capture_device *self);
    int (*close)(struct capture_device *self);
    void *userdata;
};

typedef struct capture_device capture_device_t;

/*
 * recoder
 */
__attribute((visibility("default"))) int recorder_init(void);
__attribute((visibility("default"))) int recorder_list_encoder(void);
__attribute((visibility("default"))) int recorder_register_encoder(const char *type, record_encoder_t *encoder);
__attribute((visibility("default"))) int recorder_unregister_encoder(const char *type);

typedef struct recorder *recorder_handle_t;

typedef enum
{
    RECORD_INFO_SUCCESS = 0,
    RECORD_INFO_WRITER,
    RECORD_INFO_ENCODE,
    RECORD_INFO_PAUSED,
    RECORD_INFO_RESUMED,
    RECORD_INFO_IDLE,
    RECORD_INFO_STOP,
} record_info_t;

typedef enum
{
    RECORD_FREQ_NULL = 0,
    RECORD_FREQ_LOCAL,
    RECORD_FREQ_MAX,
} record_freq_info_t;

typedef void (*recorder_listen_cb)(recorder_handle_t self, record_info_t info, void *userdata);

typedef struct
{
    const char *tag;
    char *device_name;
    capture_device_t device;
    size_t record_buf_size;
    size_t encode_buf_size;
    uint32_t recordr_stack_size;
    uint32_t encoder_stack_size;
    uint32_t writer_stack_size;

    recorder_listen_cb listen;
    void *userdata;
} recorder_cfg_t;

__attribute((visibility("default"))) recorder_handle_t recorder_create(recorder_cfg_t *cfg);

/*
 * record_writer
 */
typedef struct
{
    char *target;
    const char *tag;
    int frame_size;
    char type[32];
} record_writer_cfg_t;

struct record_writer
{
    const char *type;
    int (*init)(struct record_writer *self, record_writer_cfg_t *cfg);
    int (*write)(struct record_writer *self, char *data, size_t data_len);
    void (*destroy)(struct record_writer *self);
    void *fd;
    uint32_t out_bytes;
    void *userdata;
};

typedef struct record_writer record_writer_t;

/*
 * record_writer
 */
typedef struct
{
    char *target;
    char *type;
    bool need_free;
    record_writer_t writer;

    int samplerate;
    int bits;
    int channels;
    long duration;

    int freq_t;
} record_cfg_t;

__attribute((visibility("default"))) int recorder_record(recorder_handle_t self, record_cfg_t *cfg);

typedef enum
{
    RECORDER_STATE_IDLE = 0,
    RECORDER_STATE_RUNNING,
    RECORDER_STATE_PAUSED
} recorder_state_t;

__attribute((visibility("default"))) uint32_t recorder_get_cur_time(recorder_handle_t self);
__attribute((visibility("default"))) recorder_state_t recorder_get_state(recorder_handle_t self);
__attribute((visibility("default"))) int recorder_stop(recorder_handle_t self);
__attribute((visibility("default"))) int recorder_pause(recorder_handle_t self);
__attribute((visibility("default"))) int recorder_resume(recorder_handle_t self);
__attribute((visibility("default"))) int recorder_wait_idle(recorder_handle_t self);
__attribute((visibility("default"))) int recorder_close(recorder_handle_t self);
__attribute((visibility("default"))) void recorder_destroy(recorder_handle_t self);
__attribute((visibility("default"))) void recorder_deinit(void);

int capture_device_open_impl(struct capture_device *self, capture_device_cfg_t *cfg);
int capture_device_start_impl(struct capture_device *self);
int capture_set_volume(int vol);
int capture_get_volume(void);
int capture_device_read_impl(struct capture_device *self, const char *data, size_t data_len);
int capture_device_stop_impl(struct capture_device *self);
int capture_device_abort_impl(struct capture_device *self);
int capture_device_close_impl(struct capture_device *self);

#define DEFAULT_CAPTURE_DEVICE { \
        .open = capture_device_open_impl, \
        .start = capture_device_start_impl, \
        .read = capture_device_read_impl, \
        .stop = capture_device_stop_impl, \
        .abort = capture_device_abort_impl, \
        .close = capture_device_close_impl, \
    }

__attribute((visibility("default"))) int file_writer_init_impl(struct record_writer *self, record_writer_cfg_t *cfg);
__attribute((visibility("default"))) int file_writer_write_impl(struct record_writer *self, char *data, size_t data_len);
__attribute((visibility("default"))) void file_writer_destroy_impl(struct record_writer *self);

#define DEFAULT_FILE_WRITER { \
        .type = "file", \
        .init = file_writer_init_impl, \
        .write = file_writer_write_impl, \
        .destroy = file_writer_destroy_impl \
    }

/*
 * media_sdk.h
 */
typedef enum
{
    CMD_RECORDER_START,
    CMD_RECORDER_STOP,
    CMD_PLAYER_PLAY,
    CMD_PLAYER_SEEK,
}
media_sdk_msg_type_t;

typedef enum
{
    PLAY_MODE_PROMPT = 0,
    RECORD_MODE_PROMPT,
} media_sdk_play_mode_t;

typedef struct
{
    media_sdk_msg_type_t type;
    union
    {
        struct
        {
            media_sdk_play_mode_t mode;
            char *target;
            bool need_free;
            uint32_t seek_pos;
            bool end_session;
            char *priv_data;
        } player;
        struct
        {
            media_sdk_play_mode_t mode;
            char *target;
            char *type;
            bool need_free;
            record_writer_t writer;
            bool end_session;
            char *priv_data;
        } recorder;
    };
} media_sdk_msg_t;

/*
 * play_amr
 */
enum Frame_Type_3GPP
{
    AMR_475 = 0,
    AMR_515,
    AMR_59,
    AMR_67,
    AMR_74,
    AMR_795,
    AMR_102,
    AMR_122,
    AMR_SID,
    GSM_EFR_SID,
    TDMA_EFR_SID,
    PDC_EFR_SID,
    FOR_FUTURE_USE1,
    FOR_FUTURE_USE2,
    FOR_FUTURE_USE3,
    AMR_NO_DATA
};

/* MR122 uses 244bit, 244 / 8 = 30.5 */
#define WMF_MR122_FRAME_SIZE 31
#define SAMPLE_PER_FRAME 160

/**
 * According to RFC-4867 Document
 * https://tools/ietf.org/html/rfc4867
 */
#define AMR_HEADER_AMRNB_SC "#!AMR\n"
#define AMR_HEADER_AMRWB_SC "#!AMR-WB\n"
#define AMR_HEADER_AMRNB_MC "#!AMR_MC1.0\n"
#define AMR_HEADER_AMRWB_MC "#!AMR-WB_MC1.0\n"

#define AMR_HEADER_AMRNB_SC_LENGTH 6
#define AMR_HEADER_AMRWB_SC_LENGTH 9
#define AMR_HEADER_AMRNB_MC_LENGTH 11
#define AMR_HEADER_AMRWB_MC_LENGTH 14

typedef struct _amr_dec_s
{
    void               *_decoder;
    unsigned char       _buffer_in[WMF_MR122_FRAME_SIZE + 1];
    short               _buffer_out[SAMPLE_PER_FRAME];
    struct audio_server_data_share *ab_share_dat;
} amr_dec_t;

int play_amr_init_impl(struct play_decoder *self, play_decoder_cfg_t *cfg);
play_decoder_error_t play_amr_process_impl(struct play_decoder *self);
bool play_amr_get_post_state_impl(struct play_decoder *self);
void play_amr_destroy_impl(struct play_decoder *self);

#ifdef  CONFIG_FWANALYSIS_SEGMENT
#define DEFAULT_AMR_DECODER { \
    .type = AMR_DECODER_TYPE, \
    .support_seek = 0, \
    .segment = SEGMENT_ID_PLAY_AMR, \
    .init = play_amr_init_impl, \
    .process = play_amr_process_impl, \
    .get_post_state = play_amr_get_post_state_impl, \
    .destroy = play_amr_destroy_impl, \
}
#else
#define DEFAULT_AMR_DECODER { \
    .type = AMR_DECODER_TYPE, \
    .support_seek = 0, \
    .init = play_amr_init_impl, \
    .process = play_amr_process_impl, \
    .get_post_state = play_amr_get_post_state_impl, \
    .destroy = play_amr_destroy_impl, \
}
#endif

/*
 * play_wav
 */
__attribute((visibility("default"))) int play_wav_init_impl(struct play_decoder *self, play_decoder_cfg_t *cfg);
__attribute((visibility("default"))) play_decoder_error_t play_wav_process_impl(struct play_decoder *self);
__attribute((visibility("default"))) bool play_wav_get_post_state_impl(struct play_decoder *self);
__attribute((visibility("default"))) void play_wav_destroy_impl(struct play_decoder *self);

#ifdef  CONFIG_FWANALYSIS_SEGMENT
#define DEFAULT_WAV_DECODER { \
        .type = "wav", \
        .support_seek = 1, \
        .segment = SEGMENT_ID_PLAY_WAV, \
        .init = play_wav_init_impl, \
        .process = play_wav_process_impl, \
        .get_post_state = play_wav_get_post_state_impl, \
        .destroy = play_wav_destroy_impl, \
    }
#else
#define DEFAULT_WAV_DECODER { \
        .type = "wav", \
        .support_seek = 1, \
        .init = play_wav_init_impl, \
        .process = play_wav_process_impl, \
        .get_post_state = play_wav_get_post_state_impl, \
        .destroy = play_wav_destroy_impl, \
    }
#endif

/*
 * playback_impl
 */
int playback_device_open_impl(struct playback_device *self, playback_device_cfg_t *cfg);
int playback_device_start_impl(struct playback_device *self);
int playback_device_write_impl(struct playback_device *self, const char *data, size_t data_len);
int playback_device_stop_impl(struct playback_device *self);
int playback_device_abort_impl(struct playback_device *self);
void playback_device_close_impl(struct playback_device *self);
struct audio_player_queue *audio_queue_create(size_t item_count, size_t item_size);
int audio_queue_send(struct audio_player_queue *self, const void *data);
int audio_queue_send_font(struct audio_player_queue *self, const void *data);
int audio_queue_receive(struct audio_player_queue *self, void *data);
int audio_queue_receive_back(struct audio_player_queue *self, void *data);
int audio_queue_stop(struct audio_player_queue *self);
bool audio_queue_is_full(struct audio_player_queue *self);
bool audio_queue_is_empty(struct audio_player_queue *self);
int audio_queue_finish(struct audio_player_queue *self);
int audio_queue_peek(struct audio_player_queue *self, void *data);
void audio_queue_destroy(struct audio_player_queue *self);
struct audio_player_stream *audio_stream_create(size_t size);
int audio_stream_start(struct audio_player_stream *self);
int audio_stream_read(struct audio_player_stream *self, char *data, size_t data_len);
int audio_stream_read2(struct audio_player_stream *self, char *data, size_t data_len);
int audio_stream_write(struct audio_player_stream *self, const char *data, size_t data_len);
int audio_stream_write2(struct audio_player_stream *self, const char *data, size_t data_len);
int audio_stream_finish(struct audio_player_stream *self);
int audio_stream_stop(struct audio_player_stream *self);
int audio_stream_stop2(struct audio_player_stream *self);
int audio_stream_reset(struct audio_player_stream *self);
int audio_stream_resume(struct audio_player_stream *self);
void audio_stream_destroy(struct audio_player_stream *self);

#define DEFAULT_PLAYBACK_DEVICE { \
        .open = playback_device_open_impl, \
        .start = playback_device_start_impl, \
        .write = playback_device_write_impl, \
        .stop = playback_device_stop_impl, \
        .abort = playback_device_abort_impl, \
        .close = playback_device_close_impl, \
    }

struct wav_header
{
    char chunk_id[4];   //The "RIFF" chunk descriptor
    uint32_t chunk_size;
    char format[4];     //"WAVE"

    char subchunk1_id[4];//"fmt "
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;

    char subchunk2_id[4];//"data"
    uint32_t subchunk2_size;
};

int wav_header_init(struct wav_header *head, int samplerate, int bits, int channels);
int wav_header_complete(struct wav_header *head, int samples);

/* For resample */
/****************************************************************************
 *
 * The number of taps in the sample rate conversion filters.  This is an
 * indication of the number of MACs in the unrolled filter loop.  There is a
 * direct correlation between the number of taps and the execution time of
 * the sample rate converter.  There is one MAC per tap per output sample.
 * This is also the amount of extra memory required before any input data
 * buffer passed to the sample rate converter.
 *
 ****************************************************************************/

#define NUMTAPS_4CH (64)

/****************************************************************************
 *
 * The perisistent state of the sample rate converter.
 *
 ****************************************************************************/

typedef short ssrc_type;

typedef struct
{
    ssrc_type lastSample;
    ssrc_type Left[NUMTAPS_4CH];
} SRCbuf;

typedef struct
{
    /* The internal, persistent state of the sample rate converter. */
    unsigned char ucInternal[12];
    short *psFilter;
    int lFilterIncr;
    int lFilterSize;
    int lFilterOffset;
    ssrc_type lastSample1;
    ssrc_type lastSample2;
    ssrc_type lastSample3;
    ssrc_type lastSample4;
    ssrc_type Left[NUMTAPS_4CH * 4];
} SRCState_4ch;

typedef struct
{
    //
    // The internal, persistent state of the sample rate converter.
    //
    unsigned char ucInternal[12];
    short *psFilter;
    int lFilterIncr;
    int lFilterOffset;
    int lFilterSize;
    //BufferState sInputBuffer;
    //short Left_right[NUMTAPS*2];
    int Left_right[51 * 2];
    int Last_sample[180 * 2];
    int last_sample_num;
    int process_num;
} SRCState;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

long resample_48to16_4ch_init(SRCState_4ch *pSRC, unsigned long ulInputRate,
                              unsigned long ulOutputRate);
void resample_48to16_4ch_process(SRCState_4ch *pSRC, ssrc_type *psInData, ssrc_type *psOutData,
                                 long lNumInputSamples, long *plNumOutputSamples);
int SRCInit(SRCState *pSRC, unsigned long ulInputRate,
            unsigned long ulOutputRate);
long SRCFilter(SRCState *pSRC, short *psLeft, short *psRight,
               long lNumSamples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

