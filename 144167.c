
    const CImg<T>& _save_raw(std::FILE *const file, const char *const filename, const bool is_multiplexed) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_raw(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      if (!is_multiplexed) cimg::fwrite(_data,size(),nfile);
      else {
        CImg<T> buf(_spectrum);
        cimg_forXYZ(*this,x,y,z) {
          cimg_forC(*this,c) buf[c] = (*this)(x,y,z,c);
          cimg::fwrite(buf._data,_spectrum,nfile);
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;