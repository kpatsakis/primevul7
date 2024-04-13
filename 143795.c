
      static double mp_srand0(_cimg_math_parser& mp) {
        cimg::srand(&mp.rng);

#if cimg_use_openmp!=0
        mp.rng+=omp_get_thread_num();
#endif
        return cimg::type<double>::nan();