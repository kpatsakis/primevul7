
    static longT _distance_sep_cdt(const longT i, const longT u, const longT *const g) {
      const longT h = (i + u)/2;
      if (g[i]<=g[u]) { return h<i + g[u]?i + g[u]:h; }
      return h<u - g[i]?h:u - g[i];