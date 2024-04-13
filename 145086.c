  bool rename_file(ParmString orig_name, ParmString new_name)
  {
    remove(new_name);
    return rename(orig_name, new_name) == 0;
  }