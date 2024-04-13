    **/
    inline int strncasecmp(const char *const str1, const char *const str2, const int l) {
      if (!l) return 0;
      if (!str1) return str2?-1:0;
      const char *nstr1 = str1, *nstr2 = str2;
      int k, diff = 0; for (k = 0; k<l && !(diff = lowercase(*nstr1) - lowercase(*nstr2)); ++k) { ++nstr1; ++nstr2; }
      return k!=l?diff:0;