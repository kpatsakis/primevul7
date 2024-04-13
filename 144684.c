
    const CImg<T>& _save_rgb(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_rgb(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_spectrum!=3)
        cimg::warn(_cimg_instance
                   "save_rgb(): image instance has not exactly 3 channels, for file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const ulongT wh = (ulongT)_width*_height;
      unsigned char *const buffer = new unsigned char[3*wh], *nbuffer = buffer;
      const T
        *ptr1 = data(0,0,0,0),
        *ptr2 = _spectrum>1?data(0,0,0,1):0,
        *ptr3 = _spectrum>2?data(0,0,0,2):0;
      switch (_spectrum) {
      case 1 : { // Scalar image
        for (ulongT k = 0; k<wh; ++k) {
          const unsigned char val = (unsigned char)*(ptr1++);
          *(nbuffer++) = val;
          *(nbuffer++) = val;
          *(nbuffer++) = val;
        }
      } break;
      case 2 : { // RG image
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = 0;
        }
      } break;
      default : { // RGB image
        for (ulongT k = 0; k<wh; ++k) {
          *(nbuffer++) = (unsigned char)(*(ptr1++));
          *(nbuffer++) = (unsigned char)(*(ptr2++));
          *(nbuffer++) = (unsigned char)(*(ptr3++));
        }
      }
      }
      cimg::fwrite(buffer,3*wh,nfile);
      if (!file) cimg::fclose(nfile);
      delete[] buffer;
      return *this;