
    static longT _distance_dist_cdt(const longT x, const longT i, const longT *const g) {
      const longT d = x<i?i - x:x - i;
      return d<g[i]?g[i]:d;