    // Return number of available CPU cores.
    inline unsigned int nb_cpus() {
      unsigned int res = 1;
#if cimg_OS==2
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      res = (unsigned int)sysinfo.dwNumberOfProcessors;
#elif cimg_OS == 1
      res = (unsigned int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
      return res?res:1U;