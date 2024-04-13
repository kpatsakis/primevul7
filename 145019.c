  ListDefaultDump::ListDefaultDump(OStream & o) 
    : out(o), first(false)
  {
    first_prefix = _("# default: ");
    num_blanks = strlen(first_prefix) - 1;
  }