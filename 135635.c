void ProtocolV1::randomize_out_seq() {
  if (connection->get_features() & CEPH_FEATURE_MSG_AUTH) {
    // Set out_seq to a random value, so CRC won't be predictable.
    auto rand_seq = ceph::util::generate_random_number<uint64_t>(0, SEQ_MASK);
    ldout(cct, 10) << __func__ << " randomize_out_seq " << rand_seq << dendl;
    out_seq = rand_seq;
  } else {
    // previously, seq #'s always started at 0.
    out_seq = 0;
  }
}