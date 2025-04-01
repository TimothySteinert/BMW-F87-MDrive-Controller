// ignition.ino

void ignition_status() {
  last_ignition_seen_ms = millis();

  if (!ignition) {
    ignition = true;
    Serial.println("🔑 Ignition ON detected.");
  }
}

void ignition_timeout() {
  if (ignition && (millis() - last_ignition_seen_ms > IGNITION_TIMEOUT)) {
    ignition = false;
    mdrive_initialized = false;  // 🔁 Force MDrive to re-init next time ignition comes on
    Serial.println("🛑 Ignition OFF (timeout)");
  }

  static bool printedStartupMessage = false;
  if (!ignition && !printedStartupMessage) {
    Serial.println("⏳ Waiting for ignition...");
    printedStartupMessage = true;
  }

  if (ignition) {
    printedStartupMessage = false;
  }
}
