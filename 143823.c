    **/
    const CImg<T>& save_other(const char *const filename, const unsigned int quality=100) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_other(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_other(): File '%s', saving a volumetric image with an external call to "
                   "ImageMagick or GraphicsMagick only writes the first image slice.",
                   cimg_instance,filename);

      const unsigned int omode = cimg::exception_mode();
      bool is_saved = true;
      cimg::exception_mode(0);
      try { save_magick(filename); }
      catch (CImgException&) {
        try { save_imagemagick_external(filename,quality); }
        catch (CImgException&) {
          try { save_graphicsmagick_external(filename,quality); }
          catch (CImgException&) {
            is_saved = false;
          }
        }
      }
      cimg::exception_mode(omode);
      if (!is_saved)
        throw CImgIOException(_cimg_instance
                              "save_other(): Failed to save file '%s'. Format is not natively supported, "
                              "and no external commands succeeded.",
                              cimg_instance,
                              filename);
      return *this;