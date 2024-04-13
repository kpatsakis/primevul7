      static bool is_float() { return true; }
      static bool is_inf(const long double val) {
#ifdef isinf
        return (bool)isinf(val);
#else
        return !is_nan(val) && (val<cimg::type<long double>::min() || val>cimg::type<long double>::max());
#endif