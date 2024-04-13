
    static longT _distance_sep_mdt(const longT i, const longT u, const longT *const g) {
      return (u - i<=g[u] - g[i]?999999999:(g[u] - g[i] + u + i)/2);