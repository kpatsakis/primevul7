
    const CImg<T>& _save_pnm(std::FILE *const file, const char *const filename,
                             const unsigned int bytes_per_pixel=0) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pnm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

      double stmin, stmax = (double)max_min(stmin);
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_pnm(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   stmin,stmax,filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = (_spectrum>=2)?data(0,0,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,0,0,2):0;
      const ulongT buf_size = std::min((ulongT)(1024*1024),(ulongT)(_width*_height*(_spectrum==1?1UL:3UL)));

      std::fprintf(nfile,"P%c\n%u %u\n%u\n",
                   (_spectrum==1?'5':'6'),_width,_height,stmax<256?255:(stmax<4096?4095:65535));

      switch (_spectrum) {
      case 1 : { // Scalar image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PGM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned char)*(ptr_r++);
            cimg::fwrite(buf._data,N,nfile);
            to_write-=N;
          }
        } else { // Binary PGM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned short)*(ptr_r++);
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,N,nfile);
            to_write-=N;
          }
        }
      } break;
      case 2 : { // RG image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PPM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned char)*(ptr_r++);
              *(ptrd++) = (unsigned char)*(ptr_g++);
              *(ptrd++) = 0;
            }
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        } else {             // Binary PPM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned short)*(ptr_r++);
              *(ptrd++) = (unsigned short)*(ptr_g++);
              *(ptrd++) = 0;
            }
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        }
      } break;
      default : { // RGB image
        if (bytes_per_pixel==1 || (!bytes_per_pixel && stmax<256)) { // Binary PPM 8 bits
          CImg<ucharT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned char *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned char)*(ptr_r++);
              *(ptrd++) = (unsigned char)*(ptr_g++);
              *(ptrd++) = (unsigned char)*(ptr_b++);
            }
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        } else { // Binary PPM 16 bits
          CImg<ushortT> buf((unsigned int)buf_size);
          for (longT to_write = (longT)width()*height(); to_write>0; ) {
            const ulongT N = std::min((ulongT)to_write,buf_size/3);
            unsigned short *ptrd = buf._data;
            for (ulongT i = N; i>0; --i) {
              *(ptrd++) = (unsigned short)*(ptr_r++);
              *(ptrd++) = (unsigned short)*(ptr_g++);
              *(ptrd++) = (unsigned short)*(ptr_b++);
            }
            if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
            cimg::fwrite(buf._data,3*N,nfile);
            to_write-=N;
          }
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;