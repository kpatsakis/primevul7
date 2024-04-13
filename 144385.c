     **/
    CImg<T>& rand(const T& val_min, const T& val_max) {
      const float delta = (float)val_max - (float)val_min + (cimg::type<T>::is_float()?0:1);
      if (cimg::type<T>::is_float()) cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),524288)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) _data[off] = (T)(val_min + delta*cimg::rand(1,&rng));
          cimg::srand(rng);
        } else cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),524288)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) _data[off] = std::min(val_max,(T)(val_min + delta*cimg::rand(1,&rng)));
          cimg::srand(rng);
        }
      return *this;