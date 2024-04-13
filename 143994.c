      return filename_local;
    }

    // Implement a tic/toc mechanism to display elapsed time of algorithms.
    inline cimg_ulong tictoc(const bool is_tic) {
      cimg::mutex(2);
      static CImg<cimg_ulong> times(64);
      static unsigned int pos = 0;
      const cimg_ulong t1 = cimg::time();
      if (is_tic) {
        // Tic
        times[pos++] = t1;
        if (pos>=times._width)
          throw CImgArgumentException("cimg::tic(): Too much calls to 'cimg::tic()' without calls to 'cimg::toc()'.");
        cimg::mutex(2,0);
        return t1;
      }

      // Toc
      if (!pos)
        throw CImgArgumentException("cimg::toc(): No previous call to 'cimg::tic()' has been made.");
      const cimg_ulong
        t0 = times[--pos],
        dt = t1>=t0?(t1 - t0):cimg::type<cimg_ulong>::max();
      const unsigned int
        edays = (unsigned int)(dt/86400000.),
        ehours = (unsigned int)((dt - edays*86400000.)/3600000.),
        emin = (unsigned int)((dt - edays*86400000. - ehours*3600000.)/60000.),
        esec = (unsigned int)((dt - edays*86400000. - ehours*3600000. - emin*60000.)/1000.),
        ems = (unsigned int)(dt - edays*86400000. - ehours*3600000. - emin*60000. - esec*1000.);
      if (!edays && !ehours && !emin && !esec)
        std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u ms%s\n",
                     cimg::t_red,1 + 2*pos,"",ems,cimg::t_normal);
      else {
        if (!edays && !ehours && !emin)
          std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u sec %u ms%s\n",
                       cimg::t_red,1 + 2*pos,"",esec,ems,cimg::t_normal);
        else {
          if (!edays && !ehours)
            std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u min %u sec %u ms%s\n",
                         cimg::t_red,1 + 2*pos,"",emin,esec,ems,cimg::t_normal);
          else{
            if (!edays)
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",ehours,emin,esec,ems,cimg::t_normal);
            else{
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u days %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",edays,ehours,emin,esec,ems,cimg::t_normal);
            }
          }
        }