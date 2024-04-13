  static inline void get_encoding(const Config & c, String & final_str)
  {
    const char * codeset = nl_langinfo(CODESET);
    if (ascii_encoding(c, codeset)) codeset = "none";
    final_str = codeset;
  }