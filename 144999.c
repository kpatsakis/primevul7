  PosibErr<void> Config::read_in_string(ParmStr str, const char * what) {
    StringIStream in(str);
    return read_in(in, what);
  }