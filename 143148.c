
    const CImg<T>& _save_tiff(TIFF *tif, const unsigned int directory, const unsigned int z,
                              const unsigned int compression_type, const float *const voxel_size,
                              const char *const description) const {
      _cimg_save_tiff("bool",unsigned char,compression_type);
      _cimg_save_tiff("unsigned char",unsigned char,compression_type);
      _cimg_save_tiff("char",char,compression_type);
      _cimg_save_tiff("unsigned short",unsigned short,compression_type);
      _cimg_save_tiff("short",short,compression_type);
      _cimg_save_tiff("unsigned int",unsigned int,compression_type);
      _cimg_save_tiff("int",int,compression_type);
      _cimg_save_tiff("unsigned int64",unsigned int,compression_type);
      _cimg_save_tiff("int64",int,compression_type);
      _cimg_save_tiff("float",float,compression_type);
      _cimg_save_tiff("double",float,compression_type);
      const char *const filename = TIFFFileName(tif);
      throw CImgInstanceException(_cimg_instance
                                  "save_tiff(): Unsupported pixel type '%s' for file '%s'.",
                                  cimg_instance,
                                  pixel_type(),filename?filename:"(FILE*)");
      return *this;