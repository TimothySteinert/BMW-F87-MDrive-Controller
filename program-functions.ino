// program-functions.ino

void Serial_Print_Can(const CAN_message_t& msg) {
  Serial.print("📥 CAN Msg: ");
  for (uint8_t i = 0; i < msg.len; i++) {
    Serial.print("0x");
    if (msg.buf[i] < 0x10) Serial.print("0");
    Serial.print(msg.buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}


void handle_loopback_test() {
  if (!Serial.available()) return;

  // Example input: 12F 8 52 8A DD F1 05 30 88
  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.length() == 0) return;

  // Parse CAN ID, length, and data
  uint32_t id;
  int len;
  uint8_t data[8];

  int parsed = sscanf(input.c_str(), "%lx %d %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx",
                      &id, &len,
                      &data[0], &data[1], &data[2], &data[3],
                      &data[4], &data[5], &data[6], &data[7]);

  if (parsed >= 2 + len) {
    // Construct message
    CAN_message_t loopback_msg;
    loopback_msg.id = id;
    loopback_msg.len = len;
    memcpy(loopback_msg.buf, data, len);

    Serial.print("🔁 Injected loopback: ");
    for (int i = 0; i < len; i++) {
      Serial.print("0x");
      if (data[i] < 0x10) Serial.print("0");
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Set the global msg and call handler
    kcan2_msg = loopback_msg;
    process_kcan2_message();
  } else {
    Serial.println("❌ Failed to parse input. Format: 12F 8 52 8A DD F1 05 30 88");
  }
}






