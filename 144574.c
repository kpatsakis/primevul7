
    const CImg<T>& _save_ascii(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_ascii(): Specified filename is (null).",
                                    cimg_instance);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      std::fprintf(nfile,"%u %u %u %u\n",_width,_height,_depth,_spectrum);
      const T* ptrs = _data;
      cimg_forYZC(*this,y,z,c) {
        cimg_forX(*this,x) std::fprintf(nfile,"%.17g ",(double)*(ptrs++));
        std::fputc('\n',nfile);
      }
      if (!file) cimg::fclose(nfile);
      return *this;