   **/
    inline bool strpare(char *const str, const char delimiter,
                        const bool is_symmetric, const bool is_iterative) {
      if (!str) return false;
      const int l = (int)std::strlen(str);
      int p, q;
      if (is_symmetric) for (p = 0, q = l - 1; p<q && str[p]==delimiter && str[q]==delimiter; ) {
          --q; ++p; if (!is_iterative) break;
        } else {
        for (p = 0; p<l && str[p]==delimiter; ) { ++p; if (!is_iterative) break; }
        for (q = l - 1; q>p && str[q]==delimiter; ) { --q; if (!is_iterative) break; }
      }
      const int n = q - p + 1;
      if (n!=l) { std::memmove(str,str + p,(unsigned int)n); str[n] = 0; return true; }
      return false;