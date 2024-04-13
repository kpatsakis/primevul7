
    inline void srand(cimg_ulong *const p_rng) {
#if cimg_OS==1
      *p_rng = cimg::time() + (cimg_ulong)getpid();
#elif cimg_OS==2
      *p_rng = cimg::time() + (cimg_ulong)_getpid();
#endif