/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <defs.h>
#include <stub.h>
#include <hw/common.h>
#include <uart.h>
#include <uart_api.h>

// there is some Caravel issue happening that prevents the usual SET and CLR from working
// a fast read followed by write results in the whole register being cleared.
unsigned int temp;
#define SET(PIN,N) {temp |= (1<<N); PIN = temp;}
#define CLR(PIN,N) {temp &= ~(1<<N); PIN = temp;}

// uses mprj_datah so subtract 32 to map it
#define CTRL_EN     (32 - 32)
#define CTRL_INC    (34 - 32)
#define CTRL_RST_N  (36 - 32)

// define this to allow Caravel to select the desired design
//#define FW_SET_MUX
// define this to show the selected design number on the uio_out pins [31:24]
//#define DEBUG_MUX

void delay(const int d)
{
    // Configure timer for a single-shot countdown */
	reg_timer0_config = 0;
	reg_timer0_data = d;
    reg_timer0_config = 1;

    // Loop, waiting for value to reach zero
    reg_timer0_update = 1;  // latch current value
    while (reg_timer0_value > 0) 
    {
        reg_timer0_update = 1;
    }
}

void configure_io()
{
    // to fix issue on 2306
    reg_mprj_io_1 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_2 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_3 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_4 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;

    // user_clock2:
    reg_mprj_io_5 =   GPIO_MODE_USER_STD_OUTPUT;

    // pad_ui_in[9:0]:
    reg_mprj_io_6 =   GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_7 =   GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_8 =   GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_9 =   GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_10 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_11 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_12 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_13 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_14 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_15 =  GPIO_MODE_USER_STD_INPUT_NOPULL;

    // pad_uo_out[7:0]:
    reg_mprj_io_16 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_17 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_18 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_19 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_20 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_21 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_22 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_23 =  GPIO_MODE_USER_STD_OUTPUT;

    // pad_uio_out[7:0]
    #ifdef DEBUG_MUX
    // set to mgmt out to help debug mux
    reg_mprj_io_24 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_25 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_26 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_27 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_28 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_29 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_30 =  GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_31 =  GPIO_MODE_MGMT_STD_OUTPUT;
    #else
    reg_mprj_io_24 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_25 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_26 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_27 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_28 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_29 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_30 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_31 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
    #endif

    #ifdef FW_SET_MUX
    // ctrl_ena
    reg_mprj_io_32 =  GPIO_MODE_MGMT_STD_BIDIRECTIONAL;
    // ctrl_sel_inc
    reg_mprj_io_34 =  GPIO_MODE_MGMT_STD_BIDIRECTIONAL;
    // ctrl_sel_rst_n
    reg_mprj_io_36 =  GPIO_MODE_MGMT_STD_BIDIRECTIONAL;
    #else
    // ctrl_ena
    reg_mprj_io_32 =  GPIO_MODE_MGMT_STD_INPUT_PULLUP; // changed to pull UP to fix demoboard flub with R31 (pulled DOWN)
    // ctrl_sel_inc
    reg_mprj_io_34 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    // ctrl_sel_rst_n
    reg_mprj_io_36 =  GPIO_MODE_USER_STD_INPUT_NOPULL;
    #endif

    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}


void main()
{
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;

    configure_io();


    #ifdef FW_SET_MUX
    // enable design 0 by sending 54 pulses
    CLR(reg_mprj_datah, CTRL_INC);
    CLR(reg_mprj_datah, CTRL_RST_N);
    CLR(reg_mprj_datah, CTRL_EN);
    delay(1000);
    SET(reg_mprj_datah, CTRL_RST_N);
    delay(1000);
    for(int i = 0; i < 54; i ++ )
    {
        SET(reg_mprj_datah, CTRL_INC);
        delay(1000);
        CLR(reg_mprj_datah, CTRL_INC);
        delay(1000);
    }
    SET(reg_mprj_datah, CTRL_EN);
    delay(1000);
    #endif

    #ifdef DEBUG_MUX
    reg_la0_iena = 0x0; // input enable on for LA bank 0
    #endif
    for (int i=0; i<10; i++) {
        reg_gpio_out = 0x1;
        delay(1000000);
        reg_gpio_out = 0x0;
        delay(1000000);
    }

	while(1) {
        #ifdef DEBUG_MUX
        // check with the LA if the design is selected.
        reg_mprj_datal = reg_la0_data_in << 24;
        #endif
        reg_gpio_out = 0x1;
        delay(50000000);
        reg_gpio_out = 0x0;
        delay(700000);
	}
}
