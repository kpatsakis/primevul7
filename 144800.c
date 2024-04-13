
    static void wait_all() {
      WaitForSingleObject(cimg::Win32_attr().wait_event,INFINITE);