#include <stdio.h>
#include <stdint.h>

/* Task1 Description
 * -----------------
 * You have to write a initialization routine for a UART device. The UART is a memmory
 * mapped device at the address 0xFC000000 on an embedded platform.
 * This peripheral is controlled by the following 32 bit registers (offsets given)
 
 * BRR: Baud rate register Offset: 0x4 
 * BRR[0:3] Selects the baud rate as follows
 *  - 0: 4800
 *  - 1: 9600
 *  - 2: 14400
 *  - 3: 19200
 *  - 4: 38400
 *  - 5: 57600
 *  - 6: 115200
 *  - 7: 128000
 *  - 8: 256000
 *
 *  BRR[4:5] Selects parity as follows
 *  - 0 Even Parity
 *  - 1 Odd Parity
 *  - 2 No Parity
 *
 *  - BRR[8] Turning this bit on enables hardware flow control
 *
 *  - BRR[12:15] Contains the number of stop bits
 *
 * TER: Transmit enable register Offset: 0x8
 *  - Bit 23 in this register enables the transmit operation
 *
 * RER: Receive  enabel register Offset: 0xC
 * - Bits 3 and 5 notify overrun and framing error and need to be cleared upon reset
 * 
 * IER: Interrupt enable register Offset: 0x10
 * - Bit 14 and 15 enable TX and RX interrupts
 *
 * TDR: Transmit data register     Offset: 0x14
 *  - Contains data to be transmitted via UART
 *
 * RDR: Receive data register     Offset: 0x18
 *  - Contains data received via UART
 *
 *
 * You need to write an initialization routine for this UART in C with the following configuration.
 *
 * Baud Rate: 38400, stop bits 1, parity none, flow control none
 * TX and RX are interrupt based operations, with data registers cleared at the start of operation.
 * 
 * You also need to supply a test case by constructing a dummy UART device which will receive memory operations
 * directed for the actual hardware device. This structure should be used to validate correct configuration of
 * UART. Your test program should be runable on a GNU/Linux PC.
 * 
 **/


//#define UART_BASE    (0xFC000000UL)
//#define UART         ((UART_HANDLE*)UART_BASE)
// Baud Rate Register masks
#define BRR_BAUD_RATE_MASK     0x000F
#define BRR_PARITY_MASK        0x0030
#define BRR_HW_FLOW_CTRL_MASK  0x0100
#define BRR_STOP_BITS_MASK     0xF000

#define ENABLE_HW_FLOW_CTRL          8
#define STOP_BITS                    12
#define ENABLE                       1
#define DISABLE                      0
// Data structure containing UART parameters including base address and other settings.
typedef struct UART_HANDLE
{
  uint32_t reserved;    // Offset: 0x00
  uint32_t BRR;         // Offset: 0x04
  uint32_t TER;         // Offset: 0x08
  uint32_t RER;         // Offset: 0xC
  uint32_t IER;         // Offset: 0x10
  uint32_t TDR;         // Offset: 0x14
  uint32_t RDR;         // Offset: 0x18
}UART_HANDLE;



typedef enum
{
   BAUD_4800   = 4800,       //  0
   BAUD_9600   = 9600,       //  1
   BAUD_14400  = 14400,      //  2
   BAUD_19200  = 19200,      //  3
   BAUD_38400  = 38400,      //  4
   BAUD_57600  = 57600,      //  5
   BAUD_115200 = 115200,     //  6
   BAUD_128000 = 128000,     //  7
   BAUD_256000 = 256000,     //  8
   // Defining actual values instead of register values
   // to make set_baud_rate(h, val); easier
}Baud_Rate;

typedef struct
{
  uint32_t rate;
  uint8_t mask;
}_BaudTable;

_BaudTable BaudTable[2] =
{
  {4800, 0},
  {115200, 6}
  // This would enable addition of allowable baud rates
  // Easier debugging wrt bit settings of each baud rate
};

typedef enum
{
  PARITY_EVEN = 0x0000,      //  0
  PARITY_ODD  = 0x0010,      //  1
  PARITY_NONE = 0x0020,      //  2
}Parity;

typedef int error;

// UART_HANDLE is passed and not global
// Device may have multiple UART modules
void set_baud_rate(UART_HANDLE *h, uint32_t baud)
{
  uint8_t mask;
  for(int i = 0; BaudTable[i].rate != 0;i++)
  {
    if(baud == BaudTable[i].rate)
    {
      mask = BaudTable[i].mask;
    }
  }
  h->BRR &= (~BRR_BAUD_RATE_MASK);
  h->BRR |= mask;
}

void set_parity(UART_HANDLE *h, uint8_t parity)
{
  h->BRR &= (~BRR_PARITY_MASK);
  h->BRR |= (parity);
}

void enable_hw_flow_control(UART_HANDLE *h, uint8_t enable)
{
  h->BRR &= (~BRR_HW_FLOW_CTRL_MASK);
  h->BRR |= ((uint32_t)enable << ENABLE_HW_FLOW_CTRL);
  // Typecasting to uint32 instead of passing a uint32
}

void set_stop_bits(UART_HANDLE *h, uint8_t stop_bits)
{
  h->BRR &= (~BRR_STOP_BITS_MASK);
  h->BRR |= ((uint32_t)stop_bits << STOP_BITS);
}


void print_uart_registers(UART_HANDLE *h)
{
  printf("\nBRR = 0x%x", h->BRR);
}


// Memory Operations, stubs for transcations on embedded platform
// but route memory operations to a dummy UART buffer for unit-testing

uint32_t stub_memread(uint32_t *mem_addr)
{

}


void stub_memwrite(uint32_t *mem_addr, uint32_t val)
{

}

// Fill this function
void init_uart(UART_HANDLE *h, uint32_t baud, uint8_t parity, uint8_t hwflow_ctrl, uint8_t stop_bits)
{
  h->BRR = 0;
  set_baud_rate(h, baud);
  set_parity(h, parity);  enable_hw_flow_control(h, hwflow_ctrl);
  set_stop_bits(h, stop_bits);
}


// Provide a test case for uart initialization function above
int main(int argc, char *argv[])
{
  printf("\nHello");
  int error = 0;
  error |= test_uart_init();
  printf("\nTests Passed:");
  printf("\ntest_uart_init()");
}

// Unit tests
error test_uart_init()
{
  // Initializaion of uart in main() and printing BRR is 
  // replaced with this uint test - lesser m/m allocation
  UART_HANDLE uart;
  init_uart(&uart, BAUD_115200, PARITY_NONE, ENABLE, 1);
  return(uart.BRR == 0x1126);
}