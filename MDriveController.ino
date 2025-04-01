// MDriveController.ino

#include "globals.h"

void setup() {
  initialize_serial();
  configure_mcp2515();
  configure_IO();
  cache_can_message_buffers();
  initialize_can_handlers();

  Serial.println("✅ Setup complete.");
}

void loop() {

/***********************************************************************************************************************************************************************************************************************************************
  General
***********************************************************************************************************************************************************************************************************************************************/
  ignition_timeout();   // Handle ignition timeout
  
  if (ignition) {
    send_mdrive_keepalive_message();
  }

/*
#if LOOPBACK_MODE
    handle_loopback_test();  // ✅ This will only compile if LOOPBACK_MODE is defined as true
  #endif
*/

/***********************************************************************************************************************************************************************************************************************************************
  KCAN2 reception
***********************************************************************************************************************************************************************************************************************************************/
if (digitalRead(KCAN2_INT) == LOW) {
  if (KCAN2.readMsgBuf(&k2rxId, &k2len, k2rxBuf) == CAN_OK) {
    kcan2_msg = make_msg_buf(k2rxId, k2len, k2rxBuf);
    process_kcan2_message();
  }
}

}

/***********************************************************************************************************************************************************************************************************************************************
  KCAN2 processing - Relevant messages sent by modules on KCAN2 to be used by this module follow
***********************************************************************************************************************************************************************************************************************************************/
void process_kcan2_message() {
  // Only print specific message IDs (e.g., ignition and MDrive)
  if (kcan2_msg.id == 0x42F) {
    Serial.print("📥 CAN Msg ID: 0x");
    Serial.print(kcan2_msg.id, HEX);
    Serial.print(" Data: ");
    for (uint8_t i = 0; i < kcan2_msg.len; i++) {
      Serial.print("0x");
      if (kcan2_msg.buf[i] < 0x10) Serial.print("0");
      Serial.print(kcan2_msg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  if (kcan2_handlers[kcan2_msg.id] != nullptr) {
    kcan2_handlers[kcan2_msg.id]();
  }
}