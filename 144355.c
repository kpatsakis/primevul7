
    inline unsigned int _rand(cimg_ulong *const p_rng) {
      *p_rng = *p_rng*1103515245 + 12345U;
      return (unsigned int)*p_rng;