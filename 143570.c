
    static void wait_all() {
      if (!cimg::X11_attr().display) return;
      pthread_mutex_lock(&cimg::X11_attr().wait_event_mutex);
      pthread_cond_wait(&cimg::X11_attr().wait_event,&cimg::X11_attr().wait_event_mutex);
      pthread_mutex_unlock(&cimg::X11_attr().wait_event_mutex);