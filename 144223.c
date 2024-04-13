
    inline unsigned int wait(const unsigned int milliseconds, cimg_ulong *const p_timer) {
      if (!*p_timer) *p_timer = cimg::time();
      const cimg_ulong current_time = cimg::time();
      if (current_time>=*p_timer + milliseconds) { *p_timer = current_time; return 0; }
      const unsigned int time_diff = (unsigned int)(*p_timer + milliseconds - current_time);
      *p_timer = current_time + time_diff;
      cimg::sleep(time_diff);
      return time_diff;