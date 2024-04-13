void ProtocolV1::run_continuation(CtPtr pcontinuation) {
  if (pcontinuation) {
    CONTINUATION_RUN(*pcontinuation);
  }
}