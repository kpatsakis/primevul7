    // Find max root of a 2nd-order polynomial.
    static bool ___distance_eikonal(const Tfloat a, const Tfloat b, const Tfloat c, Tfloat &root) {
      const Tfloat delta = b*b - 4*a*c;
      if (delta<0) return false;
      root = 0.5f*(-b + std::sqrt(delta))/a;
      return true;