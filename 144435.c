    // Convert a CImg<T> to a cv::Mat.
    cv::Mat _cimg2cvmat() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Instance image is empty.",
                                    cimg_instance);
      if (_spectrum==2)
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Invalid number of channels (should be '1' or '3+').",
                                    cimg_instance);
      if (_depth!=1)
        throw CImgInstanceException(_cimg_instance
                                    "_cimg2cvmat() : Invalid number of slices (should be '1').",
                                    cimg_instance);
      int mat_type = -1;
      if (cimg::type<T>::string()==cimg::type<unsigned char>::string()) mat_type = CV_8UC1;
      if (cimg::type<T>::string()==cimg::type<char>::string()) mat_type = CV_8SC1;
      if (cimg::type<T>::string()==cimg::type<unsigned short>::string()) mat_type = CV_16UC1;
      if (cimg::type<T>::string()==cimg::type<short>::string()) mat_type = CV_16SC1;
      if (cimg::type<T>::string()==cimg::type<int>::string()) mat_type = CV_32SC1;
      if (cimg::type<T>::string()==cimg::type<float>::string()) mat_type = CV_32FC1;
      if (cimg::type<T>::string()==cimg::type<double>::string()) mat_type = CV_64FC1;
      if (mat_type<0)
        throw CImgInstanceException(_cimg_instance
                                    "_cvmat2cimg() : pixel type '%s' is not supported.",
                                    cimg_instance,pixel_type());
      cv::Mat res;
      std::vector<cv::Mat> channels(_spectrum);
      if (_spectrum>1) {
        cimg_forC(*this,c)
          channels[c] = cv::Mat(_height,_width,mat_type,_data + _width*_height*(_spectrum - 1 - c));
        cv::merge(channels,res);
      } else res = cv::Mat(_height,_width,mat_type,_data).clone();
      return res;