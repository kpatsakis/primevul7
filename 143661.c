
    inline double rand(const double val_max, cimg_ulong *const p_rng) {
      const double val = cimg::_rand(p_rng)/(double)~0U;
      return val_max*val;