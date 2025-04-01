// Mdrive.ino


// Need to add the can initialise function
// Need to add logic for when 0x42E requests M1 or M2 or Keepalive

void idrive_mdrive_initialise() {
  Serial.println("🚀 Sending MDrive init sequence...");

  f_mdrive_alive_counter = 0;

  // One-time msg1 (not stored)
  uint8_t init_msg1_buf[] = {0x01, 0x22, 0x11, 0x10, 0x00, 0x01, 0x80, 0xE0};
  CAN_message_t init_msg1 = make_msg_buf(0x42E, 8, init_msg1_buf);
  init_msg1.buf[0] = f_mdrive_alive_counter++;
  kcan2_write_msg(init_msg1);
  Serial_Print_Can(init_msg1);
  delay(100);

  // One-time msg2 (not stored)
  uint8_t init_msg2_buf[] = {0x02, 0x22, 0x11, 0x10, 0x10, 0x09, 0x90, 0xFC};
  CAN_message_t init_msg2 = make_msg_buf(0x42E, 8, init_msg2_buf);
  init_msg2.buf[0] = f_mdrive_alive_counter++;
  kcan2_write_msg(init_msg2);
  Serial_Print_Can(init_msg2);
  delay(100);

  // M1 Init: Copy base M1 and modify final bytes
  CAN_message_t m1_init = mdrive_msg_m1;
  m1_init.buf[0] = f_mdrive_alive_counter++;
  m1_init.buf[5] = 0x09;
  m1_init.buf[6] = 0x90;
  m1_init.buf[7] = 0xFC;
  kcan2_write_msg(m1_init);
  Serial_Print_Can(m1_init);
  delay(50);

  // M2 Init: Copy base M2 and modify final bytes
  CAN_message_t m2_init = mdrive_msg_m2;
  m2_init.buf[0] = f_mdrive_alive_counter++;
  m2_init.buf[5] = 0x0A;
  m2_init.buf[6] = 0x90;
  m2_init.buf[7] = 0xFC;
  kcan2_write_msg(m2_init);
  Serial_Print_Can(m2_init);
  delay(500);

  // Send M1 message
  mdrive_msg_m1.buf[0] = f_mdrive_alive_counter++;
  kcan2_write_msg(mdrive_msg_m1);
  Serial_Print_Can(mdrive_msg_m1);
  delay(50);

  // Send M2 message
  mdrive_msg_m2.buf[0] = f_mdrive_alive_counter++;
  kcan2_write_msg(mdrive_msg_m2);
  Serial_Print_Can(mdrive_msg_m2);
  delay(100);

  // Send keepalive
  mdrive_keepalive_msg.buf[0] = f_mdrive_alive_counter++;
  kcan2_write_msg(mdrive_keepalive_msg);
  Serial_Print_Can(mdrive_keepalive_msg);

  mdrive_initialized = true;
  mdrive_message_bn2010_timer = 1000;

  Serial.println("✅ MDrive initialized");

  send_mdrive_keepalive_message();  // Trigger first keepalive immediately
}


void send_mdrive_keepalive_message() {
  if (!ignition) {
    mdrive_initialized = false;  // Reset when ignition turns off
    return;
  }

  if (!mdrive_initialized) {
    idrive_mdrive_initialise();  // Triggers init and calls back here
    return;
  }

  if (mdrive_message_bn2010_timer >= 1000 && kcan2_mode == MCP_NORMAL) {
    mdrive_keepalive_msg.buf[0] = f_mdrive_alive_counter;
    kcan2_write_msg(mdrive_keepalive_msg);       // Send CAN message
    Serial_Print_Can(mdrive_keepalive_msg);      // Log to serial

    f_mdrive_alive_counter = (f_mdrive_alive_counter == 0xE) ? 0 : f_mdrive_alive_counter + 1;
    mdrive_message_bn2010_timer = 0;
  }
}



void update_mdrive_message_settings_nbt() {
  if (!ignition) return;

  uint8_t profile_type = kcan2_msg.buf[0] >> 4;

  switch (profile_type) {
    case 0x0: {  // MDrive Profile 1
      Serial.println("📥 Received MDrive Profile 1 settings");

      if ((kcan2_msg.buf[4] >> 4) == 0xE) {
        reset_mdrive_settings(profile_type);
        return;
      }

      bool changed = false;
      for (uint8_t i = 0; i <= 4; i++) {
        uint8_t incoming = kcan2_msg.buf[i];
        uint8_t current = mdrive_msg_m1.buf[i + 1];  // Offset by 1

        // High nibble
        if ((incoming & 0xF0) != 0 && (incoming & 0xF0) != (current & 0xF0)) {
          mdrive_msg_m1.buf[i + 1] = (current & 0x0F) | (incoming & 0xF0);
          changed = true;
        }

        // Low nibble
        if ((incoming & 0x0F) != 0 && (incoming & 0x0F) != (current & 0x0F)) {
          mdrive_msg_m1.buf[i + 1] = (mdrive_msg_m1.buf[i + 1] & 0xF0) | (incoming & 0x0F);
          changed = true;
        }
      }

      if (changed) {
        Serial.println("🔧 MDrive Profile 1 updated, sending response...");
      } else {
        Serial.println("✅ No changes to MDrive Profile 1.");
      }

      update_mdrive_settings_can_message(0x0);
      break;
    }

    case 0x1: {  // MDrive Profile 2
      Serial.println("📥 Received MDrive Profile 2 settings");

      if ((kcan2_msg.buf[4] >> 4) == 0xE) {
        reset_mdrive_settings(profile_type);
        return;
      }

      bool changed = false;
      for (uint8_t i = 0; i <= 4; i++) {
        uint8_t incoming = kcan2_msg.buf[i];
        uint8_t current = mdrive_msg_m2.buf[i + 1];  // Offset by 1

        // High nibble
        if ((incoming & 0xF0) != 0 && (incoming & 0xF0) != (current & 0xF0)) {
          mdrive_msg_m2.buf[i + 1] = (current & 0x0F) | (incoming & 0xF0);
          changed = true;
        }

        // Low nibble
        if ((incoming & 0x0F) != 0 && (incoming & 0x0F) != (current & 0x0F)) {
          mdrive_msg_m2.buf[i + 1] = (mdrive_msg_m2.buf[i + 1] & 0xF0) | (incoming & 0x0F);
          changed = true;
        }
      }

      if (changed) {
        Serial.println("🔧 MDrive Profile 2 updated, sending response...");
      } else {
        Serial.println("✅ No changes to MDrive Profile 2.");
      }

      update_mdrive_settings_can_message(0x1);
      break;
    }

    case 0xF:  // HU requesting current profile/status
      Serial.println("📤 iDrive requested current MDrive settings");
      update_mdrive_settings_can_message(0x2);  // Send status/keepalive
      break;

    default:
      Serial.print("⚠️ Unknown MDrive profile type: 0x");
      Serial.println(profile_type, HEX);
      break;
  }
}






void update_mdrive_settings_can_message(uint8_t profile_type) {
  switch (profile_type) {
    case 0x0:  // MDrive Profile 1
      Serial.println("📤 Sending updated MDrive Profile 1 (0x42E)...");
      kcan2_write_msg(mdrive_msg_m1);
      Serial_Print_Can(mdrive_msg_m1);
      break;

    case 0x1:  // MDrive Profile 2
      Serial.println("📤 Sending updated MDrive Profile 2 (0x42E)...");
      kcan2_write_msg(mdrive_msg_m2);
      Serial_Print_Can(mdrive_msg_m2);
      break;

    case 0x2:  // Status / Keepalive message
      Serial.println("🔄 Resending MDrive keepalive/status...");
      mdrive_keepalive_msg.buf[0] = f_mdrive_alive_counter++;
      kcan2_write_msg(mdrive_keepalive_msg);
      Serial_Print_Can(mdrive_keepalive_msg);
      break;

    default:
      Serial.print("⚠️ Unknown profile type in update request: 0x");
      Serial.println(profile_type, HEX);
      break;
  }

}

void reset_mdrive_settings(uint8_t profile_type) {
  switch (profile_type) {
    case 0x0:
      Serial.println("🔄 Resetting MDrive Profile 1 to defaults...");

      // M1 Defaults
      mdrive_msg_m1.buf[1] = 0x02;  // DSC + Power
      mdrive_msg_m1.buf[2] = 0x11;  // Steering + EDC
      mdrive_msg_m1.buf[3] = 0x12;  // Trans / Gear logic
      mdrive_msg_m1.buf[4] = 0x30;  // HUD + Exhaust
      mdrive_msg_m1.buf[5] = 0x09;  // Drive logic
      mdrive_msg_m1.buf[6] = 0x90;  // Drive logic cont'd
      mdrive_msg_m1.buf[7] = 0xF4;  // Footer

      update_mdrive_settings_can_message(0x0);
      break;

    case 0x1:
      Serial.println("🔄 Resetting MDrive Profile 2 to defaults...");

      // M2 Defaults
      mdrive_msg_m2.buf[1] = 0x12;  // DSC + Power
      mdrive_msg_m2.buf[2] = 0x32;  // Steering + EDC
      mdrive_msg_m2.buf[3] = 0x20;  // Trans / Gear logic
      mdrive_msg_m2.buf[4] = 0x10;  // HUD + Exhaust
      mdrive_msg_m2.buf[5] = 0x0A;  // Drive logic
      mdrive_msg_m2.buf[6] = 0x90;  // Drive logic cont'd
      mdrive_msg_m2.buf[7] = 0xF4;  // Footer

      update_mdrive_settings_can_message(0x1);
      break;

    default:
      Serial.print("⚠️ Unknown profile type for reset: 0x");
      Serial.println(profile_type, HEX);
      break;
  }
}

