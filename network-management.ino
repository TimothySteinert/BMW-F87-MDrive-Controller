// network-management.ino

void cache_can_message_buffers(void) {
  // These will be reused throughout runtime and modified as needed
  uint8_t m1_base[] = {0x03, 0x02, 0x22, 0x10, 0x10, 0x09, 0x90, 0xF4};  // Base M1
  uint8_t m2_base[] = {0x04, 0x12, 0x32, 0x20, 0x10, 0x0A, 0x90, 0xF4};  // Base M2
  uint8_t keepalive[] = {0x0B, 0x22, 0x10, 0x10, 0x10, 0x08, 0x90, 0xE4}; // Keepalive

  mdrive_msg_m1 = make_msg_buf(0x42E, 8, m1_base);
  mdrive_msg_m2 = make_msg_buf(0x42E, 8, m2_base);
  mdrive_keepalive_msg = make_msg_buf(0x42E, 8, keepalive);
}

CAN_message_t make_msg_buf(uint16_t txID, uint8_t txLen, uint8_t* txBuf) {
  CAN_message_t tx_msg;
  tx_msg.id = txID;
  tx_msg.len = txLen;
  for (uint8_t i = 0; i < txLen; i++) {
      tx_msg.buf[i] = txBuf[i];
  }
  return tx_msg;
}


void initialize_can_handlers(void) {
  // If only one function (without arguments) is called to handle the message, it is called directly.
  // Otherwise, a wrapper (process_*) is called instead.

  kcan2_handlers[0x12F] = ignition_status;
  kcan2_handlers[0x42F] = update_mdrive_message_settings_nbt;
}

void kcan2_write_msg(const CAN_message_t &msg) {
  byte send_buf[msg.len];
  for (uint8_t i = 0; i < msg.len; i++) {
    send_buf[i] = msg.buf[i];
  }

  if (KCAN2.sendMsgBuf(msg.id, 0, msg.len, send_buf) != CAN_OK) {
    // Optionally log or retry here
  }
}
