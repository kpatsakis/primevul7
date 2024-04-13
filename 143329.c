    }
    inline int mod(const char x, const char m) {
#if defined(CHAR_MAX) && CHAR_MAX==255
      return x%m;
#else
      return x>=0?x%m:(x%m?m + x%m:0);
#endif