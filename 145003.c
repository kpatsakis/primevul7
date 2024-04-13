  bool getline(IStream & in, DataPair & d, String & buf)
  {
    if (!in.getline(buf)) return false;
    d.value.str  = buf.mstr();
    d.value.size = buf.size();
    return true;
  }