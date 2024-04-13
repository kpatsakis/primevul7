  void to_lower(char * str)
  {
    for (; *str; str++) *str = asc_tolower(*str);
  }