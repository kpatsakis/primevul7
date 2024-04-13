  PosibErr<void> Config::read_in_file(ParmStr file) {
    FStream in;
    RET_ON_ERR(in.open(file, "r"));
    return read_in(in, file);
  }