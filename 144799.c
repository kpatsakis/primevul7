
    CImg<T>& _load_jpeg(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_jpeg(): Specified filename is (null).",
                                    cimg_instance);

#ifndef cimg_use_jpeg
      if (file)
        throw CImgIOException(_cimg_instance
                              "load_jpeg(): Unable to load data from '(FILE*)' unless libjpeg is enabled.",
                              cimg_instance);
      else return load_other(filename);
#else

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      struct jpeg_decompress_struct cinfo;
      struct _cimg_error_mgr jerr;
      cinfo.err = jpeg_std_error(&jerr.original);
      jerr.original.error_exit = _cimg_jpeg_error_exit;
      if (setjmp(jerr.setjmp_buffer)) { // JPEG error
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                             "load_jpeg(): Error message returned by libjpeg: %s.",
                             cimg_instance,jerr.message);
      }

      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo,nfile);
      jpeg_read_header(&cinfo,TRUE);
      jpeg_start_decompress(&cinfo);

      if (cinfo.output_components!=1 && cinfo.output_components!=3 && cinfo.output_components!=4) {
        if (!file) {
          cimg::fclose(nfile);
          return load_other(filename);
        } else
          throw CImgIOException(_cimg_instance
                                "load_jpeg(): Failed to load JPEG data from file '%s'.",
                                cimg_instance,filename?filename:"(FILE*)");
      }
      CImg<ucharT> buffer(cinfo.output_width*cinfo.output_components);
      JSAMPROW row_pointer[1];
      try { assign(cinfo.output_width,cinfo.output_height,1,cinfo.output_components); }
      catch (...) { if (!file) cimg::fclose(nfile); throw; }
      T *ptr_r = _data, *ptr_g = _data + 1UL*_width*_height, *ptr_b = _data + 2UL*_width*_height,
        *ptr_a = _data + 3UL*_width*_height;
      while (cinfo.output_scanline<cinfo.output_height) {
        *row_pointer = buffer._data;
        if (jpeg_read_scanlines(&cinfo,row_pointer,1)!=1) {
          cimg::warn(_cimg_instance
                     "load_jpeg(): Incomplete data in file '%s'.",
                     cimg_instance,filename?filename:"(FILE*)");
          break;
        }
        const unsigned char *ptrs = buffer._data;
        switch (_spectrum) {
        case 1 : {
          cimg_forX(*this,x) *(ptr_r++) = (T)*(ptrs++);
        } break;
        case 3 : {
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
          }
        } break;
        case 4 : {
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
            *(ptr_a++) = (T)*(ptrs++);
          }
        } break;
        }
      }
      jpeg_finish_decompress(&cinfo);
      jpeg_destroy_decompress(&cinfo);
      if (!file) cimg::fclose(nfile);
      return *this;
#endif