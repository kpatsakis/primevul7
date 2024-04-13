CtPtr ProtocolV1::write(CONTINUATION_TX_TYPE<ProtocolV1> &next,
                        bufferlist &buffer) {
  ssize_t r = connection->write(buffer, [&next, this](int r) {
    next.setParams(r);
    CONTINUATION_RUN(next);
  });
  if (r <= 0) {
    next.setParams(r);
    return &next;
  }

  return nullptr;
}