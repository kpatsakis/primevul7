void ProtocolV1::read_event() {
  ldout(cct, 20) << __func__ << dendl;
  switch (state) {
    case START_CONNECT:
      CONTINUATION_RUN(CONTINUATION(send_client_banner));
      break;
    case START_ACCEPT:
      CONTINUATION_RUN(CONTINUATION(send_server_banner));
      break;
    case OPENED:
      CONTINUATION_RUN(CONTINUATION(wait_message));
      break;
    case THROTTLE_MESSAGE:
      CONTINUATION_RUN(CONTINUATION(throttle_message));
      break;
    case THROTTLE_BYTES:
      CONTINUATION_RUN(CONTINUATION(throttle_bytes));
      break;
    case THROTTLE_DISPATCH_QUEUE:
      CONTINUATION_RUN(CONTINUATION(throttle_dispatch_queue));
      break;
    default:
      break;
  }
}