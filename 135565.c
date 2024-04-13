static int amqp_frame_to_bytes(const amqp_frame_t *frame, amqp_bytes_t buffer,
                               amqp_bytes_t *encoded) {
  void *out_frame = buffer.bytes;
  size_t out_frame_len;
  int res;

  amqp_e8(frame->frame_type, amqp_offset(out_frame, 0));
  amqp_e16(frame->channel, amqp_offset(out_frame, 1));

  switch (frame->frame_type) {
    case AMQP_FRAME_BODY: {
      const amqp_bytes_t *body = &frame->payload.body_fragment;

      memcpy(amqp_offset(out_frame, HEADER_SIZE), body->bytes, body->len);

      out_frame_len = body->len;
      break;
    }
    case AMQP_FRAME_METHOD: {
      amqp_bytes_t method_encoded;

      amqp_e32(frame->payload.method.id, amqp_offset(out_frame, HEADER_SIZE));

      method_encoded.bytes = amqp_offset(out_frame, HEADER_SIZE + 4);
      method_encoded.len = buffer.len - HEADER_SIZE - 4 - FOOTER_SIZE;

      res = amqp_encode_method(frame->payload.method.id,
                               frame->payload.method.decoded, method_encoded);
      if (res < 0) {
        return res;
      }

      out_frame_len = res + 4;
      break;
    }

    case AMQP_FRAME_HEADER: {
      amqp_bytes_t properties_encoded;

      amqp_e16(frame->payload.properties.class_id,
               amqp_offset(out_frame, HEADER_SIZE));
      amqp_e16(0, amqp_offset(out_frame, HEADER_SIZE + 2)); /* "weight" */
      amqp_e64(frame->payload.properties.body_size,
               amqp_offset(out_frame, HEADER_SIZE + 4));

      properties_encoded.bytes = amqp_offset(out_frame, HEADER_SIZE + 12);
      properties_encoded.len = buffer.len - HEADER_SIZE - 12 - FOOTER_SIZE;

      res = amqp_encode_properties(frame->payload.properties.class_id,
                                   frame->payload.properties.decoded,
                                   properties_encoded);
      if (res < 0) {
        return res;
      }

      out_frame_len = res + 12;
      break;
    }

    case AMQP_FRAME_HEARTBEAT:
      out_frame_len = 0;
      break;

    default:
      return AMQP_STATUS_INVALID_PARAMETER;
  }

  amqp_e32((uint32_t)out_frame_len, amqp_offset(out_frame, 3));
  amqp_e8(AMQP_FRAME_END, amqp_offset(out_frame, HEADER_SIZE + out_frame_len));

  encoded->bytes = out_frame;
  encoded->len = out_frame_len + HEADER_SIZE + FOOTER_SIZE;

  return AMQP_STATUS_OK;
}