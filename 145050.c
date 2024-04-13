  unsigned find_file(const Config * config, const char * option, String & filename)
  {
    StringList sl;
    config->retrieve_list(option, &sl);
    return find_file(sl, filename);
  }