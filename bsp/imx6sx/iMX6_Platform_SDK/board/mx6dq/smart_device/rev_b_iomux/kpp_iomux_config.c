/*
 * Copyright (C) 2012, [Your Company Here] All Rights Reserved.
 * IT IS EXPECTED THAT THIS TEXT BE REPLACED WITH THE BOARD SOFTWARE
 * PROVIDER'S COPYRIGHT INFORMATION. THIS TEXT WILL BE DISPLAYED AT 
 * THE TOP OF ALL SOURCE FILES GENERATED FOR THIS BOARD DESIGN.
*/

// File: kpp_iomux_config.c

/* ------------------------------------------------------------------------------
 * <auto-generated>
 *     This code was generated by a tool.
 *     Runtime Version:3.4.0.0
 *
 *     Changes to this file may cause incorrect behavior and will be lost if
 *     the code is regenerated.
 * </auto-generated>
 * ------------------------------------------------------------------------------
*/

#include "iomux_config.h"
#include "iomux_define.h"
#include "registers/regsiomuxc.h"

// Function to configure IOMUXC for kpp module.
void kpp_iomux_config(void)
{
    // Config kpp.KEY_COL4 to pad KEY_COL4(T6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_COL4(0x020E0218)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: flexcan2 signal: FLEXCAN2_TX
    //     ALT1 (1) - Select instance: ipu1 signal: IPU1_SISG4
    //     ALT2 (2) - Select instance: usb signal: USB_OTG_OC
    //     ALT3 (3) - Select instance: kpp signal: KEY_COL4
    //     ALT4 (4) - Select instance: uart5 signal: UART5_RTS_B
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO14
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_COL4(0x020E05E8)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_SRE_V(SLOW));

    // Config kpp.KEY_COL5 to pad GPIO19(P5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO19_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO19_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_COL5_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO19(0x020E0254)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: kpp signal: KEY_COL5
    //     ALT1 (1) - Select instance: enet signal: ENET_1588_EVENT0_OUT
    //     ALT2 (2) - Select instance: spdif signal: SPDIF_OUT
    //     ALT3 (3) - Select instance: ccm signal: CCM_CLKO1
    //     ALT4 (4) - Select instance: ecspi1 signal: ECSPI1_RDY
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO05
    //     ALT6 (6) - Select instance: enet signal: ENET_TX_ER
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO19_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO19_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO19_MUX_MODE_V(ALT0));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO19(0x020E0624)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO19_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO19_SRE_V(SLOW));
    // Pad GPIO19 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL5_SELECT_INPUT(0x020E08E8)
    //   DAISY [1:0] - MUX Mode Select Field Reset: GPIO00_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     GPIO00_ALT2 (0) - Select signal kpp KEY_COL5 as input from pad GPIO00(ALT2).
    //     GPIO19_ALT0 (1) - Select signal kpp KEY_COL5 as input from pad GPIO19(ALT0).
    //     CSI0_DATA04_ALT3 (2) - Select signal kpp KEY_COL5 as input from pad CSI0_DATA04(ALT3).
    //     SD2_CLK_ALT2 (3) - Select signal kpp KEY_COL5 as input from pad SD2_CLK(ALT2).
    HW_IOMUXC_KEY_COL5_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL5_SELECT_INPUT_DAISY_V(GPIO19_ALT0));

    // Config kpp.KEY_COL6 to pad GPIO09(T2)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO09_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO09_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_COL6_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO09(0x020E0228)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_RX_FS
    //     ALT1 (1) - Select instance: wdog1 signal: WDOG1_B
    //     ALT2 (2) - Select instance: kpp signal: KEY_COL6
    //     ALT3 (3) - Select instance: ccm signal: CCM_REF_EN_B
    //     ALT4 (4) - Select instance: pwm1 signal: PWM1_OUT
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO09
    //     ALT6 (6) - Select instance: usdhc1 signal: SD1_WP
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO09_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO09_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO09_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO09(0x020E05F8)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO09_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO09_SRE_V(SLOW));
    // Pad GPIO09 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL6_SELECT_INPUT(0x020E08EC)
    //   DAISY [1:0] - MUX Mode Select Field Reset: GPIO09_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     GPIO09_ALT2 (0) - Select signal kpp KEY_COL6 as input from pad GPIO09(ALT2).
    //     CSI0_DATA06_ALT3 (1) - Select signal kpp KEY_COL6 as input from pad CSI0_DATA06(ALT3).
    //     SD2_DATA3_ALT2 (2) - Select signal kpp KEY_COL6 as input from pad SD2_DATA3(ALT2).
    HW_IOMUXC_KEY_COL6_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL6_SELECT_INPUT_DAISY_V(GPIO09_ALT2));

    // Config kpp.KEY_COL7 to pad GPIO04(R6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO04_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO04_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_COL7_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO04(0x020E0238)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_TX_HF_CLK
    //     ALT2 (2) - Select instance: kpp signal: KEY_COL7
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO04
    //     ALT6 (6) - Select instance: usdhc2 signal: SD2_CD_B
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO04_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO04_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO04_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO04(0x020E0608)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO04_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO04_SRE_V(SLOW));
    // Pad GPIO04 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_COL7_SELECT_INPUT(0x020E08F0)
    //   DAISY [1:0] - MUX Mode Select Field Reset: SD2_DATA1_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     SD2_DATA1_ALT4 (0) - Select signal kpp KEY_COL7 as input from pad SD2_DATA1(ALT4).
    //     GPIO04_ALT2 (1) - Select signal kpp KEY_COL7 as input from pad GPIO04(ALT2).
    //     CSI0_DATA08_ALT3 (2) - Select signal kpp KEY_COL7 as input from pad CSI0_DATA08(ALT3).
    HW_IOMUXC_KEY_COL7_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_COL7_SELECT_INPUT_DAISY_V(GPIO04_ALT2));

    // Config kpp.KEY_ROW4 to pad KEY_ROW4(V5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_WR(0x0001B0B0);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4(0x020E021C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: flexcan2 signal: FLEXCAN2_RX
    //     ALT1 (1) - Select instance: ipu1 signal: IPU1_SISG5
    //     ALT2 (2) - Select instance: usb signal: USB_OTG_PWR
    //     ALT3 (3) - Select instance: kpp signal: KEY_ROW4
    //     ALT4 (4) - Select instance: uart5 signal: UART5_CTS_B
    //     ALT5 (5) - Select instance: gpio4 signal: GPIO4_IO15
    HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_MUX_MODE_V(ALT3));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4(0x020E05EC)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_SRE_V(SLOW));

    // Config kpp.KEY_ROW5 to pad GPIO01(T4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO01_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO01_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_ROW5_SELECT_INPUT_WR(0x00000000);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO01(0x020E0224)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_RX_CLK
    //     ALT1 (1) - Select instance: wdog2 signal: WDOG2_B
    //     ALT2 (2) - Select instance: kpp signal: KEY_ROW5
    //     ALT3 (3) - Select instance: usb signal: USB_OTG_ID
    //     ALT4 (4) - Select instance: pwm2 signal: PWM2_OUT
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO01
    //     ALT6 (6) - Select instance: usdhc1 signal: SD1_CD_B
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO01_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO01_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO01_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO01(0x020E05F4)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO01_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO01_SRE_V(SLOW));
    // Pad GPIO01 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW5_SELECT_INPUT(0x020E08F4)
    //   DAISY [1:0] - MUX Mode Select Field Reset: GPIO01_ALT2
    //                 Selecting Pads Involved in Daisy Chain.
    //     GPIO01_ALT2 (0) - Select signal kpp KEY_ROW5 as input from pad GPIO01(ALT2).
    //     CSI0_DATA05_ALT3 (1) - Select signal kpp KEY_ROW5 as input from pad CSI0_DATA05(ALT3).
    //     SD2_CMD_ALT2 (2) - Select signal kpp KEY_ROW5 as input from pad SD2_CMD(ALT2).
    HW_IOMUXC_KEY_ROW5_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW5_SELECT_INPUT_DAISY_V(GPIO01_ALT2));

    // Config kpp.KEY_ROW6 to pad GPIO02(T1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO02_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_ROW6_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO02(0x020E0234)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_TX_FS
    //     ALT2 (2) - Select instance: kpp signal: KEY_ROW6
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO02
    //     ALT6 (6) - Select instance: usdhc2 signal: SD2_WP
    //     ALT7 (7) - Select instance: mlb signal: MLB_DATA
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO02_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO02_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO02(0x020E0604)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO02_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO02_SRE_V(SLOW));
    // Pad GPIO02 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW6_SELECT_INPUT(0x020E08F8)
    //   DAISY [1:0] - MUX Mode Select Field Reset: SD2_DATA2_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     SD2_DATA2_ALT4 (0) - Select signal kpp KEY_ROW6 as input from pad SD2_DATA2(ALT4).
    //     GPIO02_ALT2 (1) - Select signal kpp KEY_ROW6 as input from pad GPIO02(ALT2).
    //     CSI0_DATA07_ALT3 (2) - Select signal kpp KEY_ROW6 as input from pad CSI0_DATA07(ALT3).
    HW_IOMUXC_KEY_ROW6_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW6_SELECT_INPUT_DAISY_V(GPIO02_ALT2));

    // Config kpp.KEY_ROW7 to pad GPIO05(R4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO05_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO05_WR(0x0001B0B0);
    // HW_IOMUXC_KEY_ROW7_SELECT_INPUT_WR(0x00000001);
    // Mux Register:
    // IOMUXC_SW_MUX_CTL_PAD_GPIO05(0x020E023C)
    //   SION [4] - Software Input On Field Reset: DISABLED
    //              Force the selected mux mode Input path no matter of MUX_MODE functionality.
    //     DISABLED (0) - Input Path is determined by functionality of the selected mux mode (regular).
    //     ENABLED (1) - Force input path of pad.
    //   MUX_MODE [2:0] - MUX Mode Select Field Reset: ALT5
    //                    Select iomux modes to be used for pad.
    //     ALT0 (0) - Select instance: esai signal: ESAI_TX2_RX3
    //     ALT2 (2) - Select instance: kpp signal: KEY_ROW7
    //     ALT3 (3) - Select instance: ccm signal: CCM_CLKO1
    //     ALT5 (5) - Select instance: gpio1 signal: GPIO1_IO05
    //     ALT6 (6) - Select instance: i2c3 signal: I2C3_SCL
    //     ALT7 (7) - Select instance: arm signal: ARM_EVENTI
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO05_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO05_SION_V(DISABLED) | 
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO05_MUX_MODE_V(ALT2));
    // Pad Control Register:
    // IOMUXC_SW_PAD_CTL_PAD_GPIO05(0x020E060C)
    //   HYS [16] - Hysteresis Enable Field Reset: ENABLED
    //     DISABLED (0) - CMOS input
    //     ENABLED (1) - Schmitt trigger input
    //   PUS [15:14] - Pull Up / Down Config. Field Reset: 100K_OHM_PU
    //     100K_OHM_PD (0) - 100K Ohm Pull Down
    //     47K_OHM_PU (1) - 47K Ohm Pull Up
    //     100K_OHM_PU (2) - 100K Ohm Pull Up
    //     22K_OHM_PU (3) - 22K Ohm Pull Up
    //   PUE [13] - Pull / Keep Select Field Reset: PULL
    //     KEEP (0) - Keeper Enabled
    //     PULL (1) - Pull Enabled
    //   PKE [12] - Pull / Keep Enable Field Reset: ENABLED
    //     DISABLED (0) - Pull/Keeper Disabled
    //     ENABLED (1) - Pull/Keeper Enabled
    //   ODE [11] - Open Drain Enable Field Reset: DISABLED
    //              Enables open drain of the pin.
    //     DISABLED (0) - Output is CMOS.
    //     ENABLED (1) - Output is Open Drain.
    //   SPEED [7:6] - Speed Field Reset: 100MHZ
    //     TBD (0) - TBD
    //     50MHZ (1) - Low (50 MHz)
    //     100MHZ (2) - Medium (100 MHz)
    //     200MHZ (3) - Maximum (200 MHz)
    //   DSE [5:3] - Drive Strength Field Reset: 40_OHM
    //     HIZ (0) - HI-Z
    //     240_OHM (1) - 240 Ohm
    //     120_OHM (2) - 120 Ohm
    //     80_OHM (3) - 80 Ohm
    //     60_OHM (4) - 60 Ohm
    //     48_OHM (5) - 48 Ohm
    //     40_OHM (6) - 40 Ohm
    //     34_OHM (7) - 34 Ohm
    //   SRE [0] - Slew Rate Field Reset: SLOW
    //             Slew rate control.
    //     SLOW (0) - Slow Slew Rate
    //     FAST (1) - Fast Slew Rate
    HW_IOMUXC_SW_PAD_CTL_PAD_GPIO05_WR(
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_HYS_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_PUS_V(100K_OHM_PU) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_PUE_V(PULL) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_PKE_V(ENABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_ODE_V(DISABLED) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_SPEED_V(100MHZ) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_DSE_V(40_OHM) | 
            BF_IOMUXC_SW_PAD_CTL_PAD_GPIO05_SRE_V(SLOW));
    // Pad GPIO05 is involved in Daisy Chain.
    // Input Select Register:
    // IOMUXC_KEY_ROW7_SELECT_INPUT(0x020E08FC)
    //   DAISY [1:0] - MUX Mode Select Field Reset: SD2_DATA0_ALT4
    //                 Selecting Pads Involved in Daisy Chain.
    //     SD2_DATA0_ALT4 (0) - Select signal kpp KEY_ROW7 as input from pad SD2_DATA0(ALT4).
    //     GPIO05_ALT2 (1) - Select signal kpp KEY_ROW7 as input from pad GPIO05(ALT2).
    //     CSI0_DATA09_ALT3 (2) - Select signal kpp KEY_ROW7 as input from pad CSI0_DATA09(ALT3).
    HW_IOMUXC_KEY_ROW7_SELECT_INPUT_WR(
            BF_IOMUXC_KEY_ROW7_SELECT_INPUT_DAISY_V(GPIO05_ALT2));
}
