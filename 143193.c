    **/
    const CImg<T>& save_magick(const char *const filename, const unsigned int bytes_per_pixel=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_magick(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifdef cimg_use_magick
      double stmin, stmax = (double)max_min(stmin);
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (_spectrum>3)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance is multispectral, only the three first channels will be "
                   "saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_magick(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   filename,stmin,stmax);

      Magick::Image image(Magick::Geometry(_width,_height),"black");
      image.type(Magick::TrueColorType);
      image.depth(bytes_per_pixel?(8*bytes_per_pixel):(stmax>=256?16:8));
      const T
        *ptr_r = data(0,0,0,0),
        *ptr_g = _spectrum>1?data(0,0,0,1):0,
        *ptr_b = _spectrum>2?data(0,0,0,2):0;
      Magick::PixelPacket *pixels = image.getPixels(0,0,_width,_height);
      switch (_spectrum) {
      case 1 : // Scalar images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = pixels->green = pixels->blue = (Magick::Quantum)*(ptr_r++);
          ++pixels;
        }
        break;
      case 2 : // RG images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = (Magick::Quantum)*(ptr_r++);
          pixels->green = (Magick::Quantum)*(ptr_g++);
          pixels->blue = 0; ++pixels;
        }
        break;
      default : // RGB images
        for (ulongT off = (ulongT)_width*_height; off; --off) {
          pixels->red = (Magick::Quantum)*(ptr_r++);
          pixels->green = (Magick::Quantum)*(ptr_g++);
          pixels->blue = (Magick::Quantum)*(ptr_b++);
          ++pixels;
        }
      }
      image.syncPixels();
      image.write(filename);
      return *this;
#else
      cimg::unused(bytes_per_pixel);
      throw CImgIOException(_cimg_instance
                            "save_magick(): Unable to save file '%s' unless libMagick++ is enabled.",
                            cimg_instance,
                            filename);
#endif