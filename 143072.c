    **/
    inline int strcasecmp(const char *const str1, const char *const str2) {
      if (!str1) return str2?-1:0;
      const int
        l1 = (int)std::strlen(str1),
        l2 = (int)std::strlen(str2);
      return cimg::strncasecmp(str1,str2,1 + (l1<l2?l1:l2));