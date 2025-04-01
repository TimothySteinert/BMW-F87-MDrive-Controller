
// globals.h

#pragma
#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>
#include <elapsedMillis.h>

#define LOOPBACK_MODE false  // Set to false to use normal mode

// === CAN Message Type ===
typedef struct CAN_message_t {
  uint32_t id = 0;          // can identifier
  uint8_t len = 8;      // length of data
  uint8_t buf[8] = { 0 };       // data
} CAN_message_t;

uint8_t kcan2_mode = MCP_NORMAL;


// === Flags ===
bool ignition = false;
bool mdrive_initialized = false;

// === Timers ===
unsigned long last_ignition_seen_ms = 0; //0 seconds

// === Constants ===
const uint16_t IGNITION_TIMEOUT = 10000;  // 10 seconds

// === MCP2515 KCAN2 Config ===
#define KCAN2_CS 5
#define KCAN2_INT 2

// Instantiate MCP2515 for KCAN2
MCP_CAN KCAN2(KCAN2_CS);

// === Core Setup Function Declarations ===
void initialize_serial();
void configure_mcp2515();
void configure_IO();
void cache_can_message_buffers();
CAN_message_t make_msg_buf(uint16_t txID, uint8_t txLen, uint8_t* txBuf);
void kcan2_write_msg(const CAN_message_t &msg);
void process_kcan2_message();
void (*kcan2_handlers[0x800])() = { nullptr };
void initialize_can_handlers();
void ignition_timeout();
void send_mdrive_keepalive_message();
void handle_loopback_test();

// === CAN Msg Handlers ===
unsigned long k2rxId;
byte k2len;
byte k2rxBuf[8];
CAN_message_t kcan2_msg;

uint8_t f_mdrive_alive_counter = 0;
elapsedMillis mdrive_message_bn2010_timer = 1000;

CAN_message_t mdrive_msg_m1;
CAN_message_t mdrive_msg_m2;
CAN_message_t mdrive_keepalive_msg;

