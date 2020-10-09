#include "lt8522ex_main.h"
#include "lt8522ex_misc.h"

extern lt8522ex_t* curr_lt8522ex;
extern struct i2c_client* curr_client;

char TxHpdChangeDetect(void)
{
    char newtxhpdstatus = 0;

    curr_client = curr_lt8522ex->client_addr2;
    newtxhpdstatus = HDMI_ReadI2C_Byte(0x7A) & 0x80;
    if (curr_lt8522ex->OldTxPlugStatus == newtxhpdstatus) {
        curr_lt8522ex->FlagTxHpdChange = 0;
        return curr_lt8522ex->FlagTxHpdChange;
    }

    if (newtxhpdstatus != curr_lt8522ex->OldTxPlugStatus) {
        if ((curr_lt8522ex->OldTxPlugStatus == 0) && (newtxhpdstatus != 0)) {
            curr_lt8522ex->FlagTxHdmiOff = 0;
            curr_lt8522ex->FlagTxHpdChange = 1;
            curr_lt8522ex->event |= EVENT_HDMI_OUT_PLUG_IN;
            lt8522ex_log("%s: HDMI output port plug in \n", curr_lt8522ex->name);
        } else {
            curr_lt8522ex->FlagTxHdmiOff = 1;
            curr_lt8522ex->event |= EVENT_HDMI_OUT_PLUG_OUT;
            lt8522ex_log("%s: HDMI output port plug out \n", curr_lt8522ex->name);
        }
    }

    curr_lt8522ex->OldTxPlugStatus = newtxhpdstatus;

    return curr_lt8522ex->FlagTxHpdChange;
}

char TxVgaChangeDetect(void)
{
    char newtxhpdstatus = 0;

    if (curr_lt8522ex->vga_out_detect_pin == INVALID_GPIO)
        return curr_lt8522ex->FlagVgaDetChange;

    gpio_direction_input(curr_lt8522ex->vga_out_detect_pin);
    newtxhpdstatus = !gpio_get_value(curr_lt8522ex->vga_out_detect_pin);
    if (curr_lt8522ex->OldVgaDetStatus == newtxhpdstatus) {
        curr_lt8522ex->FlagVgaDetChange = 0;
        return curr_lt8522ex->FlagVgaDetChange;
    }

    if (newtxhpdstatus != curr_lt8522ex->OldVgaDetStatus) {
        if ((curr_lt8522ex->OldVgaDetStatus == 0) && (newtxhpdstatus != 0)) {
            curr_lt8522ex->FlagTxVgaOff = 0;
            curr_lt8522ex->FlagVgaDetChange = 1;
            curr_lt8522ex->event |= EVENT_VGA_OUT_PLUG_IN;
        } else {
            curr_lt8522ex->FlagTxVgaOff = 1;
            curr_lt8522ex->event |= EVENT_VGA_OUT_PLUG_OUT;
        }
    }

    curr_lt8522ex->OldVgaDetStatus = newtxhpdstatus;

    return curr_lt8522ex->FlagVgaDetChange;
}

char RxHdmi5vDetect(void)
{
    char RX_5V_DET = 0;

    if (curr_lt8522ex->hdmi_in_detect_pin == INVALID_GPIO && curr_lt8522ex->FlagRxHdmi5VRise == 1) {
        curr_lt8522ex->event |= EVENT_HDMI_IN_PLUG_IN;
        lt8522ex_log("%s: HDMI input port plug in \n", curr_lt8522ex->name);
        return curr_lt8522ex->FlagRxHdmi5V;
    }

    if (curr_lt8522ex->hdmi_in_detect_pin == INVALID_GPIO) {
        return curr_lt8522ex->FlagRxHdmi5V;
    }

    gpio_direction_input(curr_lt8522ex->hdmi_in_detect_pin);
    RX_5V_DET = !gpio_get_value(curr_lt8522ex->hdmi_in_detect_pin);

    if (RX_5V_DET) {
        if (!curr_lt8522ex->FlagRxHdmi5V) {
            Delay_ms(100);
            RX_5V_DET = !gpio_get_value(curr_lt8522ex->hdmi_in_detect_pin);
            if (RX_5V_DET) {
                curr_lt8522ex->FlagRxHdmi5VRise = 1;
                curr_lt8522ex->FlagRxHdmi5V = 1;
                curr_lt8522ex->event |= EVENT_HDMI_IN_PLUG_IN;
                lt8522ex_log("%s: HDMI input port plug in \n", curr_lt8522ex->name);
            }
        } else {
            curr_lt8522ex->FlagRxHdmi5V = 1;
        }
    } else {
        if (curr_lt8522ex->FlagRxHdmi5V) {
            Delay_ms(100);
            RX_5V_DET = !gpio_get_value(curr_lt8522ex->hdmi_in_detect_pin);
            if (RX_5V_DET == 0) {
                curr_lt8522ex->FlagRxHdmi5VFall = 1;
                curr_lt8522ex->FlagRxHdmi5V = 0;
                curr_lt8522ex->event |= EVENT_HDMI_IN_PLUG_OUT;
                lt8522ex_log("%s: HDMI input port plug out \n", curr_lt8522ex->name);
            }
        } else {
            curr_lt8522ex->FlagRxHdmi5V = 0;
        }
    }

    return curr_lt8522ex->FlagRxHdmi5V;
}
