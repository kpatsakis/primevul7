  static void get_lang(String & final_str) 
  {
    // FIXME: THIS IS NOT THREAD SAFE
    String locale = setlocale (LC_ALL, NULL);
    if (locale == "C")
      setlocale (LC_ALL, "");
    const char * lang = setlocale (LC_MESSAGES, NULL);
    bool res = proc_locale_str(lang, final_str);
    if (locale == "C")
      setlocale(LC_MESSAGES, locale.c_str());
    if (!res)
      get_lang_env(final_str);
  }