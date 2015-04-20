/*
 * MRF24WG Initialization
 *
 * Functions pertaining Universal Driver and MRF24WG initialization.
 */
#include "wf_universal_driver.h"
#include "wf_global_includes.h"

extern void WF_SetTxDataConfirm(u_int8_t state);

/*
 * Initialize interrupts generated by MRF24WG module.
 */
static void Init_Interrupts()
{
    unsigned mask, mask2;

    // disable the interrupts gated by the 16-bit host int register
    WF_Write(WF_HOST_INTR2_MASK_REG, 0);
    WF_Write(WF_HOST_INTR2_REG, 0xffff);

    // disable the interrupts gated the by main 8-bit host int register
    WF_WriteByte(WF_HOST_MASK_REG, 0);
    WF_WriteByte(WF_HOST_INTR_REG, 0xff);

    // Initialize the External Interrupt for the MRF24W allowing the MRF24W to interrupt
    // the Host from this point forward.
    WF_EintInit();
    WF_EintEnable();

    // enable the following MRF24W interrupts in the INT1 8-bit register
    mask = WF_HOST_INT_MASK_FIFO_1 |        // Mgmt Rx Msg interrupt
           WF_HOST_INT_MASK_FIFO_0 |        // Data Rx Msg interrupt
           WF_HOST_INT_MASK_RAW_0 |         // RAW0 Move Complete (Data Rx) interrupt
           WF_HOST_INT_MASK_RAW_1 |         // RAW1 Move Complete (Data Tx) interrupt
           WF_HOST_INT_MASK_INT2;           // Interrupt 2 interrupt
    WF_WriteByte(WF_HOST_MASK_REG, mask);
    WF_WriteByte(WF_HOST_INTR_REG, mask);

    // enable the following MRF24W interrupts in the INT2 16-bit register
    mask2 = WF_HOST_INT2_MASK_RAW_2 |       // RAW2 Move Complete (Mgmt Rx) interrupt
            WF_HOST_INT2_MASK_RAW_3 |       // RAW3 Move Complete (Mgmt Tx) interrupt
            WF_HOST_INT2_MASK_RAW_4 |       // RAW4 Move Complete (Scratch) interrupt
            WF_HOST_INT2_MASK_RAW_5 |       // RAW5 Move Complete (Scratch) interrupt
            WF_HOST_INT2_MASK_MAIL_BOX;     // MRF24WG assertion interrupt
    WF_Write(WF_HOST_INTR2_MASK_REG, mask2);
    WF_Write(WF_HOST_INTR2_REG, mask2);
}

/*
 * Initialize the MRF24WG for operations.
 * Must be called before any other WiFi API calls.
 */
void WF_Init(t_deviceInfo *deviceInfo)
{
    unsigned msec, value;

    UdStateInit();          // initialize internal state machine
    ClearMgmtConfirmMsg();  // no mgmt response messages received
    UdSetInitInvalid();     // init not valid until it gets through this state machine

    /*
     * Reset the chip.
     */

    // clear the power bit to disable low power mode on the MRF24W
    WF_Write(WF_PSPOLL_H_REG, 0x0000);

    // Set HOST_RESET bit in register to put device in reset
    WF_Write(WF_HOST_RESET_REG, WF_Read(WF_HOST_RESET_REG) | WF_HOST_RESET_MASK);

    // Clear HOST_RESET bit in register to take device out of reset
    WF_Write(WF_HOST_RESET_REG, WF_Read(WF_HOST_RESET_REG) & ~WF_HOST_RESET_MASK);

    /*
     * Wait for chip to initialize itself, up to 2 sec.
     * Usually it takes about 140 msec until all registers are ready..
     */
    for (msec=0; msec<2000; msec++) {
        value = WF_Read(WF_HOST_WFIFO_BCNT0_REG);
        if (value & 0x0fff)
            break;
        udelay(1000);
    }

    /*
     * Finish the MRF24WG intitialization.
     */
    Init_Interrupts();                  // Initialize MRF24WG interrupts
    RawInit();                          // initialize RAW driver
    WFEnableMRF24WB0MMode();            // legacy, but still needed
    WF_DeviceInfoGet(deviceInfo);       // get MRF24WG module version numbers
    switch (deviceInfo->deviceType) {
    case WF_UNKNOWN_DEVICE:
        /* Cannot happen. */
        break;

    case WF_MRF24WB_DEVICE:
        /* MRF24WB chip not supported by this driver. */
        break;

    case WF_MRF24WG_DEVICE:
        WF_SetTxDataConfirm(WF_DISABLED); // Disable Tx Data confirms (from the MRF24W)
        WF_CPCreate();                  // create a connection profile, get its ID and store it
        WF_PsPollDisable();
        ClearPsPollReactivate();
        UdSetInitValid();               // Chip initialized successfully.
        break;
    }
}
