    **/
    static void FFT(CImg<T>& real, CImg<T>& imag, const bool is_inverse=false,
                    const unsigned int nb_threads=0) {
      if (!real)
        throw CImgInstanceException("CImgList<%s>::FFT(): Empty specified real part.",
                                    pixel_type());
      if (!imag) imag.assign(real._width,real._height,real._depth,real._spectrum,(T)0);
      if (!real.is_sameXYZC(imag))
        throw CImgInstanceException("CImgList<%s>::FFT(): Specified real part (%u,%u,%u,%u,%p) and "
                                    "imaginary part (%u,%u,%u,%u,%p) have different dimensions.",
                                    pixel_type(),
                                    real._width,real._height,real._depth,real._spectrum,real._data,
                                    imag._width,imag._height,imag._depth,imag._spectrum,imag._data);
      cimg::unused(nb_threads);
#ifdef cimg_use_fftw3
      cimg::mutex(12);
#ifndef cimg_use_fftw3_singlethread
      fftw_plan_with_nthreads(nb_threads?nb_threads:cimg::nb_cpus());
#endif
      fftw_complex *data_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*real._width*real._height*real._depth);
      if (!data_in)
        throw CImgInstanceException("CImgList<%s>::FFT(): Failed to allocate memory (%s) "
                                    "for computing FFT of image (%u,%u,%u,%u).",
                                    pixel_type(),
                                    cimg::strbuffersize(sizeof(fftw_complex)*real._width*
                                                        real._height*real._depth*real._spectrum),
                                    real._width,real._height,real._depth,real._spectrum);
      double *const ptrf = (double*)data_in;
      fftw_plan data_plan =
        real._depth>1?fftw_plan_dft_3d(real._depth,real._height,real._width,data_in,data_in,
                                       is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        real._height>1?fftw_plan_dft_2d(real._height,real._width,data_in,data_in,
                                        is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE):
        fftw_plan_dft_1d(real._width,data_in,data_in,
                         is_inverse?FFTW_BACKWARD:FFTW_FORWARD,FFTW_ESTIMATE);
      cimg_forC(real,c) {
        CImg<T> realc = real.get_shared_channel(c), imagc = imag.get_shared_channel(c);
        cimg_pragma_openmp(parallel for cimg_openmp_if_size(real.width()*real.height()*real.depth(),125000))
          cimg_rofoff(realc,i) { const ulongT i2 = 2*i; ptrf[i2] = (double)realc[i]; ptrf[i2 + 1] = (double)imagc[i]; }
        fftw_execute(data_plan);
        if (is_inverse) {
          const double a = 1.0/(real.width()*real.height()*real.depth());
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
      if (real._depth>1) FFT(real,imag,'z',is_inverse);
      if (real._height>1) FFT(real,imag,'y',is_inverse);
      if (real._width>1) FFT(real,imag,'x',is_inverse);
#endif