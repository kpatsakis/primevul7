      }
      static double nan() {
#ifdef NAN
        return (double)NAN;
#else
        const double val_nan = -std::sqrt(-1.); return val_nan;
#endif