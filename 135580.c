CtPtr ProtocolV1::read(CONTINUATION_RX_TYPE<ProtocolV1> &next,
                       int len, char *buffer) {
  if (!buffer) {
    buffer = temp_buffer;
  }
  ssize_t r = connection->read(len, buffer,
                               [&next, this](char *buffer, int r) {
                                 next.setParams(buffer, r);
                                 CONTINUATION_RUN(next);
                               });
  if (r <= 0) {
    next.setParams(buffer, r);
    return &next;
  }

  return nullptr;
}