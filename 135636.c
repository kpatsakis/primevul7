CtPtr ProtocolV1::send_connect_message_reply(char tag,
                                             ceph_msg_connect_reply &reply,
                                             bufferlist &authorizer_reply) {
  ldout(cct, 20) << __func__ << dendl;
  bufferlist reply_bl;
  reply.tag = tag;
  reply.features =
      ((uint64_t)connect_msg.features & connection->policy.features_supported) |
      connection->policy.features_required;
  reply.authorizer_len = authorizer_reply.length();
  reply_bl.append((char *)&reply, sizeof(reply));

  ldout(cct, 10) << __func__ << " reply features 0x" << std::hex
		 << reply.features << " = (policy sup 0x"
		 << connection->policy.features_supported
		 << " & connect 0x" << (uint64_t)connect_msg.features
		 << ") | policy req 0x"
		 << connection->policy.features_required
		 << dendl;

  if (reply.authorizer_len) {
    reply_bl.append(authorizer_reply.c_str(), authorizer_reply.length());
    authorizer_reply.clear();
  }

  return WRITE(reply_bl, handle_connect_message_reply_write);
}