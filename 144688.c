    **/
    inline void strwindows_reserved(char *const str, const char c='_') {
      for (char *s = str; *s; ++s) {
        const char i = *s;
        if (i=='<' || i=='>' || i==':' || i=='\"' || i=='/' || i=='\\' || i=='|' || i=='?' || i=='*') *s = c;
      }