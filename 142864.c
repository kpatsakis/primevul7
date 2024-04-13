    **/
    CImgDisplay& flush() {
      set_key().set_button().set_wheel();
      _is_resized = _is_moved = _is_event = false;
      _fps_timer = _fps_frames = _timer = 0;
      _fps_fps = 0;
      return *this;