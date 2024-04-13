
    const CImg<T>& _save_jpeg(std::FILE *const file, const char *const filename, const unsigned int quality) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_jpeg(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_jpeg(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

#ifndef cimg_use_jpeg
      if (!file) return save_other(filename,quality);
      else throw CImgIOException(_cimg_instance
                                 "save_jpeg(): Unable to save data in '(*FILE)' unless libjpeg is enabled.",
                                 cimg_instance);
#else
      unsigned int dimbuf = 0;
      J_COLOR_SPACE colortype = JCS_RGB;

      switch (_spectrum) {
      case 1 : dimbuf = 1; colortype = JCS_GRAYSCALE; break;
      case 2 : dimbuf = 3; colortype = JCS_RGB; break;
      case 3 : dimbuf = 3; colortype = JCS_RGB; break;
      default : dimbuf = 4; colortype = JCS_CMYK; break;
      }

      // Call libjpeg functions
      struct jpeg_compress_struct cinfo;
      struct jpeg_error_mgr jerr;
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_compress(&cinfo);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      jpeg_stdio_dest(&cinfo,nfile);
      cinfo.image_width = _width;
      cinfo.image_height = _height;
      cinfo.input_components = dimbuf;
      cinfo.in_color_space = colortype;
      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo,quality<100?quality:100,TRUE);
      jpeg_start_compress(&cinfo,TRUE);

      JSAMPROW row_pointer[1];
      CImg<ucharT> buffer(_width*dimbuf);

      while (cinfo.next_scanline<cinfo.image_height) {
        unsigned char *ptrd = buffer._data;

        // Fill pixel buffer
        switch (_spectrum) {
        case 1 : { // Greyscale images
          const T *ptr_g = data(0, cinfo.next_scanline);
          for (unsigned int b = 0; b<cinfo.image_width; b++)
            *(ptrd++) = (unsigned char)*(ptr_g++);
        } break;
        case 2 : { // RG images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = 0;
          }
        } break;
        case 3 : { // RGB images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1),
            *ptr_b = data(0,cinfo.next_scanline,0,2);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = (unsigned char)*(ptr_b++);
          }
        } break;
        default : { // CMYK images
          const T *ptr_r = data(0,cinfo.next_scanline,0,0),
            *ptr_g = data(0,cinfo.next_scanline,0,1),
            *ptr_b = data(0,cinfo.next_scanline,0,2),
            *ptr_a = data(0,cinfo.next_scanline,0,3);
          for (unsigned int b = 0; b<cinfo.image_width; ++b) {
            *(ptrd++) = (unsigned char)*(ptr_r++);
            *(ptrd++) = (unsigned char)*(ptr_g++);
            *(ptrd++) = (unsigned char)*(ptr_b++);
            *(ptrd++) = (unsigned char)*(ptr_a++);
          }
        }
        }
        *row_pointer = buffer._data;
        jpeg_write_scanlines(&cinfo,row_pointer,1);
      }
      jpeg_finish_compress(&cinfo);
      if (!file) cimg::fclose(nfile);
      jpeg_destroy_compress(&cinfo);
      return *this;
#endif