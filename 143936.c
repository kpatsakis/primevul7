
    const CImg<T>& _save_bmp(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_bmp(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_bmp(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_bmp(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      CImg<ucharT> header(54,1,1,1,0);
      unsigned char align_buf[4] = { 0 };
      const unsigned int
        align = (4 - (3*_width)%4)%4,
        buf_size = (3*_width + align)*height(),
        file_size = 54 + buf_size;
      header[0] = 'B'; header[1] = 'M';
      header[0x02] = file_size&0xFF;
      header[0x03] = (file_size>>8)&0xFF;
      header[0x04] = (file_size>>16)&0xFF;
      header[0x05] = (file_size>>24)&0xFF;
      header[0x0A] = 0x36;
      header[0x0E] = 0x28;
      header[0x12] = _width&0xFF;
      header[0x13] = (_width>>8)&0xFF;
      header[0x14] = (_width>>16)&0xFF;
      header[0x15] = (_width>>24)&0xFF;
      header[0x16] = _height&0xFF;
      header[0x17] = (_height>>8)&0xFF;
      header[0x18] = (_height>>16)&0xFF;
      header[0x19] = (_height>>24)&0xFF;
      header[0x1A] = 1;
      header[0x1B] = 0;
      header[0x1C] = 24;
      header[0x1D] = 0;
      header[0x22] = buf_size&0xFF;
      header[0x23] = (buf_size>>8)&0xFF;
      header[0x24] = (buf_size>>16)&0xFF;
      header[0x25] = (buf_size>>24)&0xFF;
      header[0x27] = 0x1;
      header[0x2B] = 0x1;
      cimg::fwrite(header._data,54,nfile);

      const T
        *ptr_r = data(0,_height - 1,0,0),
        *ptr_g = (_spectrum>=2)?data(0,_height - 1,0,1):0,
        *ptr_b = (_spectrum>=3)?data(0,_height - 1,0,2):0;

      switch (_spectrum) {
      case 1 : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            const unsigned char val = (unsigned char)*(ptr_r++);
            std::fputc(val,nfile); std::fputc(val,nfile); std::fputc(val,nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width;
        }
      } break;
      case 2 : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            std::fputc(0,nfile);
            std::fputc((unsigned char)(*(ptr_g++)),nfile);
            std::fputc((unsigned char)(*(ptr_r++)),nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width; ptr_g-=2*_width;
        }
      } break;
      default : {
        cimg_forY(*this,y) {
          cimg_forX(*this,x) {
            std::fputc((unsigned char)(*(ptr_b++)),nfile);
            std::fputc((unsigned char)(*(ptr_g++)),nfile);
            std::fputc((unsigned char)(*(ptr_r++)),nfile);
          }
          cimg::fwrite(align_buf,align,nfile);
          ptr_r-=2*_width; ptr_g-=2*_width; ptr_b-=2*_width;
        }
      }
      }
      if (!file) cimg::fclose(nfile);
      return *this;