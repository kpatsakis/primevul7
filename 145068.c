  static void get_lang_env(String & str) 
  {
    if (proc_locale_str(getenv("LC_MESSAGES"), str)) return;
    if (proc_locale_str(getenv("LANG"), str)) return;
    if (proc_locale_str(getenv("LANGUAGE"), str)) return;
    str = DEFAULT_LANG;
  }