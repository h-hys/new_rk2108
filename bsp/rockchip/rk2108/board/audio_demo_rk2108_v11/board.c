/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      first implementation
 *
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_PMIC
#include "drv_pmic.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED

#ifdef RT_USING_PWM_REGULATOR
#include "drv_pwm_regulator.h"

struct pwr_pwm_info_desc dcdc_pwr_pwm_desc =
{
    .name = "pwm0",
    .chanel = 3,
    .invert = 1,
};
#endif

#ifdef RT_USING_PMIC_RK816
RT_WEAK struct pwr_i2c_desc pmic_pwr_i2c_desc =
{
    .name = "i2c2",
    .addr = RK816_I2C_ADDR,
};
#endif


struct regulator_desc regulators[] =
{
#ifdef RT_USING_PWM_REGULATOR
    /****** vdd_dsp_core **********/
    {
        .flag = REGULATOR_FLG_PWM | REGULATOR_FLG_LOCK,
        .desc.pwm_desc.pwrId = PWR_ID_CORE,
        .desc.pwm_desc.period = 2500,
        .desc.pwm_desc.minVolt = 685000,
        .desc.pwm_desc.maxVlot = 1000000,
        .desc.pwm_desc.pwm = &dcdc_pwr_pwm_desc,
    },
#endif
#ifdef RT_USING_PMIC_RK816
    /* BUCK2 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK1_2(RK816_ID_DCDC2, PWR_ID_BUCK_1V8),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },

    /* BUCK4 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK4(RK816_ID_DCDC4, PWR_ID_VCCIO_3V3),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO1 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO1, PWR_ID_VCC_3V0_BL),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO2 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO2, PWR_ID_VCCIO_1V8),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO4 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO4, PWR_ID_VCC_AUDIO),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
#endif
    { /* sentinel */ },
};

RT_WEAK const struct regulator_init regulator_inits[] =
{
#ifdef RT_USING_PWM_REGULATOR
    REGULATOR_INIT("vdd_core", PWR_ID_CORE,  900000, 1, 900000, 1),
#endif
#ifdef RT_USING_PMIC_RK816
    REGULATOR_INIT("dcdc2", PWR_ID_BUCK_1V8,  1100000, 1, 1100000, 1),
    REGULATOR_INIT("dcdc4", PWR_ID_VCCIO_3V3,  3300000, 1, 3300000, 1),
    REGULATOR_INIT("ldo1",  PWR_ID_VCC_3V0_BL,  3300000, 1, 3300000, 1),
    REGULATOR_INIT("ldo2",  PWR_ID_VCCIO_1V8,  1800000, 1, 1800000, 1),
    REGULATOR_INIT("ldo4",  PWR_ID_VCC_AUDIO,   3300000, 1, 3300000, 1),
#endif
    { /* sentinel */ },
};
#endif


#ifdef RT_USING_CODEC_ES8311
const struct codec_desc codec_es8311 =
{
    .if_type = IF_TYPE_I2C,
    .name = "es8311",
    .i2c_bus = "i2c0",
    .i2c_addr = 0x19,
};
#endif

#ifdef RT_USING_AUDIO
extern const struct audio_mdais_desc rk_mdais;
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_ADC_ES8311_ECHO
    {
        .name = "echo",
        .dai = (void *) &rk_mdais,
        .codec = ACDCDIG,
        .capture = true,
        .playback = false,
        .multi_dais = true,
        .format = AUDIO_FMT_PDM,
        .trcm_mode = TRCM_TXONLY,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_ACDCDIG
    {
        .name = "adc",
#ifdef RT_USING_ACDCDIG_I2STDM0
        .dai = I2STDM0,
#elif  defined(RT_USING_ACDCDIG_I2STDM1)
        .dai = I2STDM1,
        .rxMap = 0x3012, /* re-mapping sdi2 to path0 */
#elif  defined(RT_USING_ACDCDIG_PDM0) || defined(RT_USING_ACDCDIG_PDM0_SDI2)
        .dai = PDM0,
#endif
#ifdef RT_USING_VAD
        .vad = VAD,
#endif
        .codec = ACDCDIG,
        .capture = true,
        .mclkfs = 2048,
#if defined(RT_USING_ACDCDIG_PDM0) || defined(RT_USING_ACDCDIG_PDM0_SDI2)
        .format = AUDIO_FMT_PDM,
        .pdm_mode = PDM_HIGH_MODE,
#else
        .format = AUDIO_FMT_I2S,
#endif
    },
#endif
#ifdef RT_USING_AUDIO_CARD_PDM_MIC
    {
        .name = "pdm",
        .dai = PDM0,
#ifdef RT_USING_VAD
        .vad = VAD,
#endif
        .codec = NULL,
        .capture = true,
        .format = AUDIO_FMT_PDM,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_I2S_MIC
    {
        .name = "i2s",
#ifdef RT_USING_I2STDM0_MIC
        .dai = I2STDM0,
#elif  defined(RT_USING_I2STDM1_MIC)
        .dai = I2STDM1,
#endif
        .codec = NULL,
        .capture = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_AUDIOPWM
    {
        .name = "audpwm",
        .dai = AUDIOPWM,
        .codec = NULL,
        .playback = true,
        .mclkfs = 4096,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_ES8311
    {
        .name = "es8311",
        .dai = I2STDM1,
        .codec = (void *) &codec_es8311,
        .capture = true,
        .playback = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
    { /* sentinel */ }
};
#endif
