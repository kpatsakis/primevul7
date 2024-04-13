
      static void _mp_complex_pow(const double r1, const double i1,
                                  const double r2, const double i2,
                                  double *ptrd) {
        double ro, io;
        if (cimg::abs(i2)<1e-15) { // Exponent is real
          if (cimg::abs(r1)<1e-15 && cimg::abs(i1)<1e-15) {
            if (cimg::abs(r2)<1e-15) { ro = 1; io = 0; }
            else ro = io = 0;
          } else {
            const double
              mod1_2 = r1*r1 + i1*i1,
              phi1 = std::atan2(i1,r1),
              modo = std::pow(mod1_2,0.5*r2),
              phio = r2*phi1;
            ro = modo*std::cos(phio);
            io = modo*std::sin(phio);
          }
        } else { // Exponent is complex
          if (cimg::abs(r1)<1e-15 && cimg::abs(i1)<1e-15) ro = io = 0;
          const double
            mod1_2 = r1*r1 + i1*i1,
            phi1 = std::atan2(i1,r1),
            modo = std::pow(mod1_2,0.5*r2)*std::exp(-i2*phi1),
            phio = r2*phi1 + 0.5*i2*std::log(mod1_2);
          ro = modo*std::cos(phio);
          io = modo*std::sin(phio);
        }
        *(ptrd++) = ro;
        *ptrd = io;