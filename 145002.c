  PosibErr<bool> ListDefaultDump::add(ParmStr d) 
  {
    if (first) {
      out.write(first_prefix);
    } else {
      out.put('#');
      for (unsigned i = 0; i != num_blanks; ++i)
        out.put(' ');
    }
    VARARRAY(char, buf, d.size() * 2 + 1);
    escape(buf, d);
    out.printl(buf);
    first = false;
    return true;
  }