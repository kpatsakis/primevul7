    DataBuf &DataBuf::operator=(DataBufRef rhs) { reset(rhs.p); return *this; }