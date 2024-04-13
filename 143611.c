
    CImg<T>& _load_dlm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_dlm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"r");
      CImg<charT> delimiter(256), tmp(256); *delimiter = *tmp = 0;
      unsigned int cdx = 0, dx = 0, dy = 0;
      int err = 0;
      double val;
      assign(256,256,1,1,(T)0);
      while ((err = std::fscanf(nfile,"%lf%255[^0-9eEinfa.+-]",&val,delimiter._data))>0) {
        if (err>0) (*this)(cdx++,dy) = (T)val;
        if (cdx>=_width) resize(3*_width/2,_height,1,1,0);
        char c = 0;
        if (!cimg_sscanf(delimiter,"%255[^\n]%c",tmp._data,&c) || c=='\n') {
          dx = std::max(cdx,dx);
          if (++dy>=_height) resize(_width,3*_height/2,1,1,0);
          cdx = 0;
        }
      }
      if (cdx && err==1) { dx = cdx; ++dy; }
      if (!dx || !dy) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_dlm(): Invalid DLM file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      resize(dx,dy,1,1,0);
      if (!file) cimg::fclose(nfile);
      return *this;