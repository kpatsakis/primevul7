
    static longT _distance_dist_edt(const longT x, const longT i, const longT *const g) {
      return (x - i)*(x - i) + g[i];