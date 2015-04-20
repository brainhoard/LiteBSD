/*
 * MRF24WG Data Tx/Rx
 *
 * Functions that allocate and deallocate Tx/Rx data messages
 * as well as functions that perform Tx/Rx processing.
 */
#include "wf_universal_driver.h"
#include "wf_global_includes.h"

#define WF_TX_PREAMBLE_SIZE 4
#define ETHER_HEADER_SIZE   16  // 6 bytes dest address, 6 bytes src address, 2 bytes for type field
#define ENC_PREAMBLE_OFFSET 14

typedef struct {
    u_int8_t type;
    u_int8_t subType;
} t_rxPreamble;

// will be overwriting the ethernet header source address with the snap header
static const u_int8_t snapHdr[6] = {
    SNAP_VAL, SNAP_VAL, SNAP_CTRL_VAL, SNAP_TYPE_VAL, SNAP_TYPE_VAL, SNAP_TYPE_VAL
};

static bool g_HostRAWPacketRx;

void SignalPacketRx()
{
    g_HostRAWPacketRx = 1;
}

bool isPacketRx()
{
    return g_HostRAWPacketRx;
}

void ClearPacketRx()
{
    g_HostRAWPacketRx = 0;
}

/*
 * called from WiFi_Task
 */
void RxPacketCheck()
{
    if (isPacketRx()) {
        ClearPacketRx();
        WF_ProcessRxPacket();
    }
}

/*
 * packetSize includes the ethernet header bytes
 */
bool WF_TxPacketAllocate(u_int16_t packetSize)
{
    bool result = 0;
    u_int32_t startTime;

    EnsureWFisAwake();

    startTime = WF_TimerRead();
    while (WF_TimerElapsed(startTime) < 20)
    {
        // allocate an extra 4 bytes for WiFi message preamble
        result = AllocateDataTxBuffer(packetSize + WF_TX_PREAMBLE_SIZE);
        if (result) {
            // set the RAW index at 4 to leave room for internal 4 byte header.  Ethernet
            // packet data starts at index 4.
            RawSetIndex(RAW_DATA_TX_ID, 4);
            result = 1;
            break;
        }
    }
    return result;
}

void WF_TxPacketCopy(u_int8_t *p_buf, u_int16_t length)
{
   RawSetByte(RAW_DATA_TX_ID, p_buf, length);
}

void WF_TxPacketTransmit(u_int16_t packetSize)
{
    /* create internal preamble */
    u_int8_t txDataPreamble[4] = {WF_DATA_REQUEST_TYPE, WF_STD_DATA_MSG_SUBTYPE, 1, 0};

    EnsureWFisAwake();

    // write out internal preamble, starting at index 0 in the raw window
    RawWrite(RAW_DATA_TX_ID, 0, sizeof(txDataPreamble), txDataPreamble);

    // overwrite the Ethernet source address in Tx packet with SNAP header
    // (starts at index 10 within packet, after 4 bytes of preamble and 6 bytes of dest address)
    RawWrite(RAW_DATA_TX_ID, sizeof(txDataPreamble) + WF_MAC_ADDRESS_LENGTH, SNAP_SIZE, snapHdr);

    // Notify WiFi device that there is a transmit frame to send.  The frame will
    // be automatically deallocated after RF transmission is completed.
    RawMove(RAW_DATA_TX_ID, RAW_MAC, 0, packetSize + sizeof(txDataPreamble));

    // this raw window is logically no longer mounted.  The WiFi chip will
    // automatically deallocate after RF transmission.
    SetRawDataWindowState(RAW_DATA_TX_ID, WF_RAW_UNMOUNTED);
}

u_int16_t WF_RxPacketLengthGet()
{
    u_int16_t len;
    t_rxPreamble rxPreamble;

    /* Mount Read FIFO to RAW Rx window.  Allows use of RAW engine to read rx data packet. */
    /* Function call returns number of bytes in the data packet.                           */
    len = RawMountRxBuffer(RAW_DATA_RX_ID);

    // TODO: length must never be 0

    // read the data frame internal preamble (type and subtype) to verify that we did, in
    // fact, mount an Rx data packet.  This read auto-increments the raw index to the first
    // actual data byte in the frame.
    RawGetByte(RAW_DATA_RX_ID, (u_int8_t *)&rxPreamble, sizeof(t_rxPreamble));
    if (rxPreamble.type != WF_DATA_RX_INDICATE_TYPE)
    {
        printf("--- %s: invalid preamble type=%u\n", __func__, rxPreamble.type);
        return 0;
    }

    // set raw pointer to start of 802.11 payload (start of Ethernet packet)
    RawSetIndex(RAW_DATA_RX_ID, sizeof(t_rxPreamble) + ENC_PREAMBLE_OFFSET);

    return len - sizeof(t_rxPreamble) - ENC_PREAMBLE_OFFSET;
}

void WF_RxPacketDeallocate()
{
    DeallocateDataRxBuffer();
}

void WF_RxPacketCopy(u_int8_t *p_buf, u_int16_t length)
{
    RawGetByte(RAW_DATA_RX_ID, p_buf, length);
}
