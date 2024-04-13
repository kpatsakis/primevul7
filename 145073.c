  void to_lower(String & res, const char * str)
  {
    for (; *str; str++) res += asc_tolower(*str);
  }