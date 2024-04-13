  static bool proc_locale_str(ParmStr lang, String & final_str)
  {
    if (lang == 0) return false;
    const char * i = lang;
    if (!(asc_islower(i[0]) && asc_islower(i[1]))) return false;
    final_str.assign(i, 2);
    i += 2;
    if (! (i[0] == '_' || i[0] == '-')) return true;
    i += 1;
    if (!(asc_isupper(i[0]) && asc_isupper(i[1]))) return true;
    final_str += '_';
    final_str.append(i, 2);
    return true;
  }