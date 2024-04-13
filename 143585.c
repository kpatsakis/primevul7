    //! Load image from a DICOM file, using XMedcon's external tool 'medcon' \newinstance.
    static CImg<T> get_load_medcon_external(const char *const filename) {
      return CImg<T>().load_medcon_external(filename);