
    static longT _distance_dist_mdt(const longT x, const longT i, const longT *const g) {
      return (x<i?i - x:x - i) + g[i];