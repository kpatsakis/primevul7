  PosibErr<void> ListDump::clear() {
    out.printf("clear-%s\n", name);
    return no_err;
  }