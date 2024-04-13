CtPtr ProtocolV1::handle_message_header(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read message header failed" << dendl;
    return _fault();
  }

  ldout(cct, 20) << __func__ << " got MSG header" << dendl;

  current_header = *((ceph_msg_header *)buffer);

  ldout(cct, 20) << __func__ << " got envelope type=" << current_header.type << " src "
                 << entity_name_t(current_header.src) << " front=" << current_header.front_len
                 << " data=" << current_header.data_len << " off " << current_header.data_off
                 << dendl;

  if (messenger->crcflags & MSG_CRC_HEADER) {
    __u32 header_crc = 0;
    header_crc = ceph_crc32c(0, (unsigned char *)&current_header,
                             sizeof(current_header) - sizeof(current_header.crc));
    // verify header crc
    if (header_crc != current_header.crc) {
      ldout(cct, 0) << __func__ << " got bad header crc " << header_crc
                    << " != " << current_header.crc << dendl;
      return _fault();
    }
  }

  // Reset state
  data_buf.clear();
  front.clear();
  middle.clear();
  data.clear();

  state = THROTTLE_MESSAGE;
  return CONTINUE(throttle_message);
}