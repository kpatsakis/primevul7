    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const char axis, const bool is_inverse=false,
                    const unsigned int nb_threads=0) {
      if (!real)
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part is empty.",
                                    pixel_type());
      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImg<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);
      const char _axis = cimg::lowercase(axis);
      if (_axis!='x' && _axis!='y' && _axis!='z')
        throw CImgArgumentException("CImgList<%s>::FFT(): Invalid specified axis '%c' for real and imaginary parts "
                                    "(%u,%u,%u,%u) "
                                    "(should be { x | y | z }).",
                                    pixel_type(),axis,
                                    real._width,real._height,real._depth,real._spectrum);
      cimg::unused(nb_threads);
#ifdef cimg_use_fftw3
      cimg::mutex(12);
#ifndef cimg_use_fftw3_singlethread
      fftw_plan_with_nthreads(nb_threads?nb_threads:cimg::nb_cpus());
#endif
      fftw_complex *data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width*real._height*real._depth);
      if (!data_in)
        throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                    "for computing FFT of image (%u,%u,%u,%u) along the X-axis.",
                                    pixel_type(),
                                    cimg::strbuffersize(sizeof(fftw_complex)*real._width),
                                    real._width,real._height,real._depth,real._spectrum);
      double *const ptrf = (double*)data_in;
      fftw_plan data_plan =
        _axis=='x'?fftw_plan_many_dft(1,(int*)&real._width,real.height()*real.depth(),
                                      data_in,0,1,real.width(),
                                      data_in,0,1,real.width(),
                                      is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        _axis=='y'?fftw_plan_many_dft(1,(int*)&real._height,real.width()*real.depth(),
                                      data_in,0,real.width(),1,
                                      data_in,0,real.width(),1,
                                      is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        fftw_plan_many_dft(1,(int*)&real._depth,real.width()*real.height(),
                           data_in,0,real.width()*real.height(),1,
                           data_in,0,real.width()*real.height(),1,
                           is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
      cimg_forC(real,c) {
        CImg<T> realc = real.get_shared_channel(c), imagc = imag.get_shared_channel(c);
        cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_rofoff(realc,i) { const ulongT i2 = 2*i; ptrf[i2] = (double)realc[i]; ptrf[i2 + 1] = (double)imagc[i]; }
        fftw_execute(data_plan);
        if (is_inverse) {
          const double a = 1.0/(_axis=='x'?real.width():_axis=='y'?real.height():real.depth());
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
            cimg_rofoff(realc,i) { const ulongT i2 = 2*i; realc[i] = (T)(a*ptrf[i2]); imagc[i] = (T)(a*ptrf[i2 + 1]); }
        } else
          cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
            cimg_rofoff(realc,i) { const ulongT i2 = 2*i; realc[i] = (T)ptrf[i2]; imagc[i] = (T)ptrf[i2 + 1]; }
      }
      fftw_destroy_plan(data_plan);
      fftw_free(data_in);
#ifndef cimg_use_fftw3_singlethread
      fftw_cleanup_threads();
#endif
      cimg::mutex(12,0);
#else
      switch (_axis) {
      case 'x' : { // Fourier along X, using built-in functions
        const unsigned int N = real._width, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the X-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forYZC(real,y,z,c) {
              cimg::swap(real(i,y,z,c),real(j,y,z,c));
              cimg::swap(imag(i,y,z,c),imag(j,y,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(ri,y,z,c),real(rj,y,z,c));
                cimg::swap(imag(ri,y,z,c),imag(rj,y,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = delta>>1;
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forYZC(real,y,z,c) {
                T &ir = real(j,y,z,c), &ii = imag(j,y,z,c), &nir = real(nj,y,z,c), &nii = imag(nj,y,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      case 'y' : { // Fourier along Y, using built-in functions
        const unsigned int N = real._height, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Y-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXZC(real,x,z,c) {
              cimg::swap(real(x,i,z,c),real(x,j,z,c));
              cimg::swap(imag(x,i,z,c),imag(x,j,z,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,ri,z,c),real(x,rj,z,c));
                cimg::swap(imag(x,ri,z,c),imag(x,rj,z,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXZC(real,x,z,c) {
                T &ir = real(x,j,z,c), &ii = imag(x,j,z,c), &nir = real(x,nj,z,c), &nii = imag(x,nj,z,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      default : { // Fourier along Z, using built-in functions
        const unsigned int N = real._depth, N2 = N>>1;
        if (((N - 1)&N) && N!=1)
          throw CImgInstanceException("CImgList<%s>::FFT(): Specified real and imaginary parts (%u,%u,%u,%u) "
                                      "have non 2^N dimension along the Z-axis.",
                                      pixel_type(),
                                      real._width,real._height,real._depth,real._spectrum);

        for (unsigned int i = 0, j = 0; i<N2; ++i) {
          if (j>i) cimg_forXYC(real,x,y,c) {
              cimg::swap(real(x,y,i,c),real(x,y,j,c));
              cimg::swap(imag(x,y,i,c),imag(x,y,j,c));
              if (j<N2) {
                const unsigned int ri = N - 1 - i, rj = N - 1 - j;
                cimg::swap(real(x,y,ri,c),real(x,y,rj,c));
                cimg::swap(imag(x,y,ri,c),imag(x,y,rj,c));
              }
            }
          for (unsigned int m = N, n = N2; (j+=n)>=m; j-=m, m = n, n>>=1) {}
        }
        for (unsigned int delta = 2; delta<=N; delta<<=1) {
          const unsigned int delta2 = (delta>>1);
          for (unsigned int i = 0; i<N; i+=delta) {
            float wr = 1, wi = 0;
            const float
              angle = (float)((is_inverse?+1:-1)*2*cimg::PI/delta),
              ca = (float)std::cos(angle),
              sa = (float)std::sin(angle);
            for (unsigned int k = 0; k<delta2; ++k) {
              const unsigned int j = i + k, nj = j + delta2;
              cimg_forXYC(real,x,y,c) {
                T &ir = real(x,y,j,c), &ii = imag(x,y,j,c), &nir = real(x,y,nj,c), &nii = imag(x,y,nj,c);
                const float tmpr = (float)(wr*nir - wi*nii), tmpi = (float)(wr*nii + wi*nir);
                nir = (T)(ir - tmpr);
                nii = (T)(ii - tmpi);
                ir+=(T)tmpr;
                ii+=(T)tmpi;
              }
              const float nwr = wr*ca-wi*sa;
              wi = wi*ca + wr*sa;
              wr = nwr;
            }
          }
        }
        if (is_inverse) { real/=N; imag/=N; }
      } break;
      }
#endif