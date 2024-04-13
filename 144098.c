    **/
    const CImg<T>& save_minc2(const char *const filename,
                              const char *const imitate_file=0) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                   "save_minc2(): Specified filename is (null).",
                                   cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

#ifndef cimg_use_minc2
     cimg::unused(imitate_file);
     return save_other(filename);
#else
     minc::minc_1_writer wtr;
     if (imitate_file)
       wtr.open(filename, imitate_file);
     else {
       minc::minc_info di;
       if (width()) di.push_back(minc::dim_info(width(),width()*0.5,-1,minc::dim_info::DIM_X));
       if (height()) di.push_back(minc::dim_info(height(),height()*0.5,-1,minc::dim_info::DIM_Y));
       if (depth()) di.push_back(minc::dim_info(depth(),depth()*0.5,-1,minc::dim_info::DIM_Z));
       if (spectrum()) di.push_back(minc::dim_info(spectrum(),spectrum()*0.5,-1,minc::dim_info::DIM_TIME));
       wtr.open(filename,di,1,NC_FLOAT,0);
     }
     if (cimg::type<T>::string()==cimg::type<unsigned char>::string())
       wtr.setup_write_byte();
     else if (cimg::type<T>::string()==cimg::type<int>::string())
       wtr.setup_write_int();
     else if (cimg::type<T>::string()==cimg::type<double>::string())
       wtr.setup_write_double();
     else
       wtr.setup_write_float();
     minc::save_standard_volume(wtr, this->_data);
     return *this;
#endif