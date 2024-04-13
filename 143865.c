    **/
    inline char *strellipsize(char *const str, const unsigned int l=64,
                              const bool is_ending=true) {
      if (!str) return str;
      const unsigned int nl = l<5?5:l, ls = (unsigned int)std::strlen(str);
      if (ls<=nl) return str;
      if (is_ending) std::strcpy(str + nl - 5,"(...)");
      else {
        const unsigned int ll = (nl - 5)/2 + 1 - (nl%2), lr = nl - ll - 5;
        std::strcpy(str + ll,"(...)");
        std::memmove(str + ll + 5,str + ls - lr,lr);
      }
      str[nl] = 0;
      return str;