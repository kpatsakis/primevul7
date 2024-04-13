  PosibErr<bool> ListDump::remove(ParmStr d) {
    VARARRAY(char, buf, d.size() * 2 + 1);
    escape(buf, d);
    out.printf("remove-%s %s\n", name, buf);
    return true;
  }