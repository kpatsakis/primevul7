uint64_t ProtocolV1::discard_requeued_up_to(uint64_t out_seq, uint64_t seq) {
  ldout(cct, 10) << __func__ << " " << seq << dendl;
  std::lock_guard<std::mutex> l(connection->write_lock);
  if (out_q.count(CEPH_MSG_PRIO_HIGHEST) == 0) {
    return seq;
  }
  list<pair<bufferlist, Message *> > &rq = out_q[CEPH_MSG_PRIO_HIGHEST];
  uint64_t count = out_seq;
  while (!rq.empty()) {
    pair<bufferlist, Message *> p = rq.front();
    if (p.second->get_seq() == 0 || p.second->get_seq() > seq) break;
    ldout(cct, 10) << __func__ << " " << *(p.second) << " for resend seq "
                   << p.second->get_seq() << " <= " << seq << ", discarding"
                   << dendl;
    p.second->put();
    rq.pop_front();
    count++;
  }
  if (rq.empty()) out_q.erase(CEPH_MSG_PRIO_HIGHEST);
  return count;
}