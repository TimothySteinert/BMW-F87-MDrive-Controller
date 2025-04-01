// board.ino

void initialize_serial() {
  Serial.begin(115200);
  delay(1000);  // Let serial come online
  Serial.println("🖥️ Serial initialized.");
}

void configure_mcp2515() {
  SPI.begin(18, 19, 23, KCAN2_CS);

  if (KCAN2.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("❌ MCP2515 (KCAN2) init failed.");
    while (1);
  }

  #if LOOPBACK_MODE
    KCAN2.setMode(MCP_LOOPBACK);
    Serial.println("🧪 KCAN2 (MCP2515) initialized in LOOPBACK mode.");
  #else
    KCAN2.setMode(MCP_NORMAL);
    Serial.println("✅ KCAN2 (MCP2515) initialized in NORMAL mode.");
  #endif
}


void configure_IO() {
  pinMode(KCAN2_INT, INPUT);
  Serial.println("📶 KCAN2 INT pin configured.");
}
