
    CImg<T>& _load_rgb(std::FILE *const file, const char *const filename,
                       const unsigned int dimw, const unsigned int dimh) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_rgb(): Specified filename is (null).",
                                    cimg_instance);

      if (!dimw || !dimh) return assign();
      const longT cimg_iobuffer = (longT)24*1024*1024;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<ucharT> raw;
      assign(dimw,dimh,1,3);
      T
        *ptr_r = data(0,0,0,0),
        *ptr_g = data(0,0,0,1),
        *ptr_b = data(0,0,0,2);
      for (longT to_read = (longT)size(); to_read>0; ) {
        raw.assign(std::min(to_read,cimg_iobuffer));
        cimg::fread(raw._data,raw._width,nfile);
        to_read-=raw._width;
        const unsigned char *ptrs = raw._data;
        for (ulongT off = raw._width/3UL; off; --off) {
          *(ptr_r++) = (T)*(ptrs++);
          *(ptr_g++) = (T)*(ptrs++);
          *(ptr_b++) = (T)*(ptrs++);
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;