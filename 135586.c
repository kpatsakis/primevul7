bool ProtocolV1::is_connected() {
  return can_write.load() == WriteStatus::CANWRITE;
}