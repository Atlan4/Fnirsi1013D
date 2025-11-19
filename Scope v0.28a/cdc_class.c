#include "types.h"
#include "cdc_class.h"
#include "usb_interface.h" /* for usb_read_from_fifo / usb_write_ep2_data etc. */
#include <string.h>


 /* Buffers and indices */
 uint8 usb_rx[1024];
 volatile uint32 usb_rx_in_idx = 0;
 volatile uint32 usb_rx_out_idx = 0;

 uint8 usb_tx[32768];
 volatile uint32 usb_tx_in_idx = 0;
 volatile uint32 usb_tx_out_idx = 0;
/*
static uint8 usb_rx[1024];
static volatile uint32 usb_rx_in_idx = 0;
static volatile uint32 usb_rx_out_idx = 0;

static uint8 usb_tx[1024];
static volatile uint32 usb_tx_in_idx = 0;
static volatile uint32 usb_tx_out_idx = 0;
*/
  uint32 circular_buffer_used(const volatile uint32 in, const volatile uint32 out, const uint32 size);
  uint32 circular_buffer_free(const volatile uint32 in, const volatile uint32 out, const uint32 size);
  

//volatile uint32 EP2DisableTX = 0; /* flag to disable EP transmission */
volatile uint32 EP1DisableTX = 0;       /* flag to disable EP transmission */

/* Forward helpers */
 inline uint32 circular_buffer_used(const volatile uint32 in, const volatile uint32 out, const uint32 size) {
    if (in >= out) return in - out;
    return size - (out - in);
}

 inline uint32 circular_buffer_free(const volatile uint32 in, const volatile uint32 out, const uint32 size) {
    uint32 used = circular_buffer_used(in, out, size);
    return size - used - 1; // keep one byte free to distinguish full/empty 
}


//==============================================================================
/* Device descriptor */
 const USB_DeviceDescriptor CDC_DevDesc =
{
  sizeof(USB_DeviceDescriptor),
  DEVICE_DESCRIPTOR,
  0x0200,
  0x02,
  0x00,
  0x00,
  USB_EP0_FIFO_SIZE,
  0x0483,
  0x5740,
  0x0310,
  0x01,
  0x02,
  0x00,
  0x01
};

/* Configuration descriptor & related descriptors -- keep structure as before,
   but ensure endpoint packet sizes are consistent with CH340_BULK_MAX_PACKET / CH340_INTR_MAX_PACKET */
const CDC_Descriptor CDC_ConfDesc =
{
  {
    sizeof(USB_ConfigDescriptor),
    CONFIGURATION_DESCRIPTOR,
    CONFIG_CDC_DESCRIPTOR_LEN,
    0x02,
    0x01,
    0x00,
    0xC0,
    0x32
  },
  {
    sizeof(USB_InterfaceDescriptor),
    INTERFACE_DESCRIPTOR,
    0x00,
    0x00,
    0x01,
    0x02,
    0x02,
    0x01,
    0x00
  },
  {
    sizeof(USB_CDC_HeaderDescriptor),
    USB_DT_CS_INTERFACE,
    0x00,
    0x0110
  },
  {
    sizeof(USB_CDC_CallMgmtDescriptor),
    USB_DT_CS_INTERFACE,
    0x01,
    0x00,
    0x01
  },
  {
    sizeof(USB_CDC_AcmDescriptor),
    USB_DT_CS_INTERFACE,
    0x02,
    0x02
  },
  {
    sizeof(USB_CDC_UnionDescriptor),
    USB_DT_CS_INTERFACE,
    0x06,
    0x00,
    0x01
  },
  /* Interrupt endpoint for notifications (IN) */
  {
    sizeof(USB_EndPointDescriptor),
    ENDPOINT_DESCRIPTOR,
    (1 << 7) | 1,         // 0x81 — IN endpoint 1 (interrupt) 
    3,                    // interrupt 
    CDC_INTR_MAX_PACKET,  // wMaxPacketSize = 8 bytes
    10                    // bInterval = 10 (both FS and HS)
  },
  /* Data interface descriptor */
  {
    sizeof(USB_InterfaceDescriptor),
    INTERFACE_DESCRIPTOR,
    0x01,
    0x00,
    0x02,
    0x0A,
    0x00,
    0x00,
    0x00
  },
  /* Bulk IN and OUT endpoints (IN = 0x82, OUT = 0x02) */
  {
    {
      sizeof(USB_EndPointDescriptor),
      ENDPOINT_DESCRIPTOR,
      (1 << 7) | 2, /* 0x82 — IN endpoint 2 (bulk) */
      2,            /* bulk */
      CDC_BULK_MAX_PACKET,
      0
    },
    {
      sizeof(USB_EndPointDescriptor),
      ENDPOINT_DESCRIPTOR,
      (0 << 7) | 2, /* 0x02 — OUT endpoint 2 (bulk) */
      2,            /* bulk */
      CDC_BULK_MAX_PACKET,
      0
    }
  }
};

/* String descriptors (FNIRSI Custom CDC ACM) */
const uint8 str_vendor[14] = {
    0x0E, 0x03,   // 14 bytes = 2 + (6 characters * 2)
    'F',0,'N',0,'I',0,'R',0,'S',0,'I',0
};

const uint8 str_ret[26] = {
    0x1A, 0x03,   // 26 bytes = 2 + (12 characters * 2)
    'F',0,'N',0,'I',0,'R',0,'S',0,'I',0,' ',0,
    '1',0,'0',0,'1',0,'3',0,'D',0
};

/*
 const uint8 str_isernum[24] = {
    0x18, 0x03,   // 24 bytes = 2 + (11 characters * 2)
    'C',0,'D',0,'C',0,' ',0,'A',0,'C',0,'M',0,
    ' ',0,'D',0,'E',0,'V',0
};
 */

const uint8 str_isernum[20] = {
    0x12, 0x03, '1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0
};


const uint8 str_config[24] = {
    0x18, 0x03,   // 24 bytes = 2 + (11 characters * 2)
    'F',0,'N',0,'I',0,'R',0,'S',0,'I',0,' ',0,
    'C',0,'o',0,'n',0,'f',0
};

const uint8 str_interface1[30] = {
    0x1E, 0x03,   // 30 bytes = 2 + (14 characters * 2)
    'C',0,'u',0,'s',0,'t',0,'o',0,'m',0,' ',0,
    'C',0,'D',0,'C',0,' ',0,'A',0,'C',0,'M',0
};

const uint8 str_interface2[30] = {
    0x1E, 0x03,   // 30 bytes = 2 + (14 characters * 2)
    'D',0,'a',0,'t',0,'a',0,' ',0,'I',0,'n',0,
    't',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0
};

/* vendor specifics */
const uint8 vendorVersion[2] = { 0x31, 0x00 };
const uint8 vendorAttach[2]  = { 0xC3, 0x00 };
const uint8 vendorStatus[2]  = { 0xFF, 0xEE };

/* Read one character from RX circular buffer.
   Returns -1 if no data available, otherwise 0..255 value. */
int16 CDCReceive(void)
{
    if (usb_rx_out_idx == usb_rx_in_idx) return -1;
    int16 c = usb_rx[usb_rx_out_idx++];
    usb_rx_out_idx %= sizeof(usb_rx);
    return c;
}

/* Put one byte into TX circular buffer (blocking until room available).
   Note: This is a simple busy-wait; consider using signaling (semaphores/flags)
   if you need non-blocking or low-power behavior. */
void CDCSend(uint8 c)
{
    while(circular_buffer_free(usb_tx_in_idx, usb_tx_out_idx, sizeof(usb_tx)) == 0);
    EP1DisableTX = 1;
    usb_tx[usb_tx_in_idx++] = c;
    usb_tx_in_idx %= sizeof(usb_tx);
    EP1DisableTX = 0;
}

//==============================================================================

/* OUT endpoint callback: read data from USB FIFO into RX buffer */
void usb_CDC_out_ep_callback(uint8 *data, int length)
{
    for (int i = 0; i < length; i++) {
        uint32 next_idx = (usb_rx_in_idx + 1) % sizeof(usb_rx);
        if (next_idx == usb_rx_out_idx)
            break;
        usb_rx[usb_rx_in_idx] = data[i];
        usb_rx_in_idx = next_idx;
    }
}

//------------------------------------------------------------------------------
volatile uint8 usb_tx_busy = 0;   // 0 = idle, 1 = sending

static uint16 lcd_y3 = 70;
/* CDC IN endpoint callback - send all available data in 512-byte packets */
void usb_CDC_in_ep_callback(void)
{
    #define USB_CDC_PACKET_SIZE 512
    uint8 packet[USB_CDC_PACKET_SIZE];
    
       if (usb_tx_busy == 0)
        usb_tx_busy = 1;   // start the transfer

    // --- Disable interrupts during sending (if ISR is used) ---
    //USB_CDC_TX_INT_DISABLE();

    // Vyber endpoint 2 (CDC Data IN)
    *USBC_REG_EPIND = 2;
    //    uint32 count = circular_buffer_used(usb_tx_in_idx, usb_tx_out_idx, sizeof(usb_tx));
    //    display_decimal(500,lcd_y3, count );
   //     lcd_y3 += 10;

    // Continue as long as there is something to send in the TX buffer
    while (1)
    {
        uint32 count = circular_buffer_used(usb_tx_in_idx, usb_tx_out_idx, sizeof(usb_tx));
        if (count == 0)
            break;  // nothing more to send

        // send in blocks of 512 bytes
        if (count > USB_CDC_PACKET_SIZE)
            count = USB_CDC_PACKET_SIZE;

        // Copy data from the circular buffer
        for (uint32 i = 0; i < count; ++i)
        {
            packet[i] = usb_tx[usb_tx_out_idx++];
            usb_tx_out_idx %= sizeof(usb_tx);
        }

        // Wait for the endpoint to become free
        while (*USBC_REG_TXCSR & USBC_BP_TXCSR_D_TX_READY);

        // Write data to FIFO
        usb_write_to_fifo((void *)USBC_REG_EPFIFO2, packet, count);

        // Clear under run flag and set TX ready
        *USBC_REG_TXCSR &= ~USBC_BP_TXCSR_D_UNDER_RUN;
        *USBC_REG_TXCSR |= USBC_BP_TXCSR_D_TX_READY;
    }

    // --- Enable interrupt back ---
    //USB_CDC_TX_INT_ENABLE();
        
    // all done, TX free
    usb_tx_busy = 0;
}

//==============================================================================

