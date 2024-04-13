    **/
    CImg<T>& load_camera(const unsigned int camera_index=0,
                         const unsigned int capture_width=0, const unsigned int capture_height=0,
                         const unsigned int skip_frames=0, const bool release_camera=true) {
#ifdef cimg_use_opencv
      if (camera_index>=64)
        throw CImgArgumentException(_cimg_instance
                                    "load_camera(): Invalid request for camera #%u "
                                    "(no more than 100 cameras can be managed simultaneously).",
                                    cimg_instance,
                                    camera_index);
      static cv::VideoCapture *captures[64] = { 0 };
      static unsigned int captures_w[64], captures_h[64];
      if (release_camera) {
        cimg::mutex(9);
        if (captures[camera_index]) captures[camera_index]->release();
        delete captures[camera_index];
        captures[camera_index] = 0;
        captures_w[camera_index] = captures_h[camera_index] = 0;
        cimg::mutex(9,0);
        return *this;
      }
      if (!captures[camera_index]) {
        cimg::mutex(9);
        captures[camera_index] = new cv::VideoCapture(camera_index);
        captures_w[camera_index] = captures_h[camera_index] = 0;
        if (!captures[camera_index]->isOpened()) {
          delete captures[camera_index];
          captures[camera_index] = 0;
          cimg::mutex(9,0);
          throw CImgIOException(_cimg_instance
                                "load_camera(): Failed to initialize camera #%u.",
                                cimg_instance,
                                camera_index);
        }
        cimg::mutex(9,0);
      }
      cimg::mutex(9);
      if (capture_width!=captures_w[camera_index]) {
        captures[camera_index]->set(_cimg_cap_prop_frame_width,capture_width);
        captures_w[camera_index] = capture_width;
      }
      if (capture_height!=captures_h[camera_index]) {
        captures[camera_index]->set(_cimg_cap_prop_frame_height,capture_height);
        captures_h[camera_index] = capture_height;
      }
      for (unsigned int i = 0; i<skip_frames; ++i) captures[camera_index]->grab();
      cv::Mat cvimg;
      captures[camera_index]->read(cvimg);
      if (cvimg.empty()) assign(); else _cvmat2cimg(cvimg).move_to(*this);
      cimg::mutex(9,0);
      return *this;
#else
      cimg::unused(camera_index,skip_frames,release_camera,capture_width,capture_height);
      throw CImgIOException(_cimg_instance
                            "load_camera(): This function requires the OpenCV library to run "
                            "(macro 'cimg_use_opencv' must be defined).",
                            cimg_instance);
#endif