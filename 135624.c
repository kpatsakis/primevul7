bool ProtocolV1::is_queued() {
  return !out_q.empty() || connection->is_queued();
}