  PosibErr<bool> ListDump::add(ParmStr d) {
    VARARRAY(char, buf, d.size() * 2 + 1);
    escape(buf, d);
    out.printf("add-%s %s\n", name, buf);
    return true;
  }