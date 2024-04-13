
    const CImg<T>& _save_pfm(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pfm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_pfm(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_pfm(): image instance is multispectral, only the three first channels will be saved "
                   "in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = (_spectrum>=2)?data(0,0,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,0,0,2):0;
      const unsigned int buf_size = std::min(1024*1024U,_width*_height*(_spectrum==1?1:3));

      std::fprintf(nfile,"P%c\n%u %u\n1.0\n",
                   (_spectrum==1?'f':'F'),_width,_height);

      switch (_spectrum) {
      case 1 : { // Scalar image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,(ulongT)buf_size);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (float)*(ptr_r++);
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } break;
      case 2 : { // RG image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const unsigned int N = std::min((unsigned int)to_write,buf_size/3);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) {
            *(ptrd++) = (float)*(ptr_r++);
            *(ptrd++) = (float)*(ptr_g++);
            *(ptrd++) = 0;
          }
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,3*N,nfile);
          to_write-=N;
        }
      } break;
      default : { // RGB image
        CImg<floatT> buf(buf_size);
        for (longT to_write = (longT)width()*height(); to_write>0; ) {
          const unsigned int N = std::min((unsigned int)to_write,buf_size/3);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) {
            *(ptrd++) = (float)*(ptr_r++);
            *(ptrd++) = (float)*(ptr_g++);
            *(ptrd++) = (float)*(ptr_b++);
          }
          if (!cimg::endianness()) cimg::invert_endianness(buf._data,buf_size);
          cimg::fwrite(buf._data,3*N,nfile);
          to_write-=N;
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;