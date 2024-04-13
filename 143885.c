    // (Original code by Haz-Edine Assemlal).
    CImg<T>& load_minc2(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_minc2(): Specified filename is (null).",
                                    cimg_instance);
#ifndef cimg_use_minc2
      return load_other(filename);
#else
      minc::minc_1_reader rdr;
      rdr.open(filename);
      assign(rdr.ndim(1)?rdr.ndim(1):1,
             rdr.ndim(2)?rdr.ndim(2):1,
             rdr.ndim(3)?rdr.ndim(3):1,
             rdr.ndim(4)?rdr.ndim(4):1);
      if (cimg::type<T>::string()==cimg::type<unsigned char>::string())
        rdr.setup_read_byte();
      else if (cimg::type<T>::string()==cimg::type<int>::string())
        rdr.setup_read_int();
      else if (cimg::type<T>::string()==cimg::type<double>::string())
        rdr.setup_read_double();
      else
        rdr.setup_read_float();
      minc::load_standard_volume(rdr,this->_data);
      return *this;
#endif