
    inline unsigned int prand(const double z, cimg_ulong *const p_rng) {
      if (z<=1.e-10) return 0;
      if (z>100) return (unsigned int)((std::sqrt(z) * cimg::grand(p_rng)) + z);
      unsigned int k = 0;
      const double y = std::exp(-z);
      for (double s = 1.; s>=y; ++k) s*=cimg::rand(1,p_rng);
      return k - 1;