
    static longT _distance_sep_edt(const longT i, const longT u, const longT *const g) {
      return (u*u - i*i + g[u] - g[i])/(2*(u - i));