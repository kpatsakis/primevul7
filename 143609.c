
    static int screen_width() {
      DEVMODE mode;
      mode.dmSize = sizeof(DEVMODE);
      mode.dmDriverExtra = 0;
      EnumDisplaySettings(0,ENUM_CURRENT_SETTINGS,&mode);
      return (int)mode.dmPelsWidth;