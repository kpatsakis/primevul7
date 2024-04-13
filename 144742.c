    **/
    CImg<T>& noise(const double sigma, const unsigned int noise_type=0) {
      if (is_empty()) return *this;
      const Tfloat vmin = (Tfloat)cimg::type<T>::min(), vmax = (Tfloat)cimg::type<T>::max();
      Tfloat nsigma = (Tfloat)sigma, m = 0, M = 0;
      if (nsigma==0 && noise_type!=3) return *this;
      if (nsigma<0 || noise_type==2) m = (Tfloat)min_max(M);
      if (nsigma<0) nsigma = (Tfloat)(-nsigma*(M-m)/100.);
      switch (noise_type) {
      case 0 : { // Gaussian noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) {
            Tfloat val = (Tfloat)(_data[off] + nsigma*cimg::grand(&rng));
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      case 1 : { // Uniform noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) {
            Tfloat val = (Tfloat)(_data[off] + nsigma*cimg::rand(-1,1,&rng));
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      case 2 : { // Salt & Pepper noise
        if (nsigma<0) nsigma = -nsigma;
        if (M==m) {
          if (cimg::type<T>::is_float()) { --m; ++M; }
          else { m = (Tfloat)cimg::type<T>::min(); M = (Tfloat)cimg::type<T>::max(); }
        }
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) if (cimg::rand(100,&rng)<nsigma) _data[off] = (T)(cimg::rand(1,&rng)<0.5?M:m);
          cimg::srand(rng);
          }
      } break;
      case 3 : { // Poisson Noise
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) _data[off] = (T)cimg::prand(_data[off],&rng);
          cimg::srand(rng);
        }
      } break;
      case 4 : { // Rice noise
        const Tfloat sqrt2 = (Tfloat)std::sqrt(2.);
        cimg_pragma_openmp(parallel cimg_openmp_if_size(size(),131072)) {
          ulongT rng = (cimg::_rand(),cimg::rng());

#if cimg_use_openmp!=0
          rng+=omp_get_thread_num();
#endif
          cimg_pragma_openmp(for)
            cimg_rofoff(*this,off) {
            const Tfloat
              val0 = (Tfloat)_data[off]/sqrt2,
              re = (Tfloat)(val0 + nsigma*cimg::grand(&rng)),
              im = (Tfloat)(val0 + nsigma*cimg::grand(&rng));
            Tfloat val = cimg::hypot(re,im);
            if (val>vmax) val = vmax;
            if (val<vmin) val = vmin;
            _data[off] = (T)val;
          }
          cimg::srand(rng);
        }
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "noise(): Invalid specified noise type %d "
                                    "(should be { 0=gaussian | 1=uniform | 2=salt&Pepper | 3=poisson }).",
                                    cimg_instance,
                                    noise_type);
      }
      return *this;