    **/
    inline char *strellipsize(const char *const str, char *const res, const unsigned int l=64,
                              const bool is_ending=true) {
      const unsigned int nl = l<5?5:l, ls = (unsigned int)std::strlen(str);
      if (ls<=nl) { std::strcpy(res,str); return res; }
      if (is_ending) {
        std::strncpy(res,str,nl - 5);
        std::strcpy(res + nl -5,"(...)");
      } else {
        const unsigned int ll = (nl - 5)/2 + 1 - (nl%2), lr = nl - ll - 5;
        std::strncpy(res,str,ll);
        std::strcpy(res + ll,"(...)");
        std::strncpy(res + ll + 5,str + ls - lr,lr);
      }
      res[nl] = 0;
      return res;