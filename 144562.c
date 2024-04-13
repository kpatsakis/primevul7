    /**
       \param filename Filename, as a C-string.
       \return \c true if the file format supports multiple images, \c false otherwise.
    **/
    static bool is_saveable(const char *const filename) {
      const char *const ext = cimg::split_filename(filename);
      if (!cimg::strcasecmp(ext,"cimgz") ||
#ifdef cimg_use_tiff
          !cimg::strcasecmp(ext,"tif") ||
          !cimg::strcasecmp(ext,"tiff") ||
#endif
          !cimg::strcasecmp(ext,"yuv") ||
          !cimg::strcasecmp(ext,"avi") ||
          !cimg::strcasecmp(ext,"mov") ||
          !cimg::strcasecmp(ext,"asf") ||
          !cimg::strcasecmp(ext,"divx") ||
          !cimg::strcasecmp(ext,"flv") ||
          !cimg::strcasecmp(ext,"mpg") ||
          !cimg::strcasecmp(ext,"m1v") ||
          !cimg::strcasecmp(ext,"m2v") ||
          !cimg::strcasecmp(ext,"m4v") ||
          !cimg::strcasecmp(ext,"mjp") ||
          !cimg::strcasecmp(ext,"mp4") ||
          !cimg::strcasecmp(ext,"mkv") ||
          !cimg::strcasecmp(ext,"mpe") ||
          !cimg::strcasecmp(ext,"movie") ||
          !cimg::strcasecmp(ext,"ogm") ||
          !cimg::strcasecmp(ext,"ogg") ||
          !cimg::strcasecmp(ext,"ogv") ||
          !cimg::strcasecmp(ext,"qt") ||
          !cimg::strcasecmp(ext,"rm") ||
          !cimg::strcasecmp(ext,"vob") ||
          !cimg::strcasecmp(ext,"wmv") ||