void ProtocolV1::discard_out_queue() {
  ldout(cct, 10) << __func__ << " started" << dendl;

  for (list<Message *>::iterator p = sent.begin(); p != sent.end(); ++p) {
    ldout(cct, 20) << __func__ << " discard " << *p << dendl;
    (*p)->put();
  }
  sent.clear();
  for (map<int, list<pair<bufferlist, Message *> > >::iterator p =
           out_q.begin();
       p != out_q.end(); ++p) {
    for (list<pair<bufferlist, Message *> >::iterator r = p->second.begin();
         r != p->second.end(); ++r) {
      ldout(cct, 20) << __func__ << " discard " << r->second << dendl;
      r->second->put();
    }
  }
  out_q.clear();
  write_in_progress = false;
}