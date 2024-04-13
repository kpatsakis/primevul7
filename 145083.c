  PathBrowser::~PathBrowser() 
  {
    delete els;
    if (dir_handle) closedir((DIR *)dir_handle);
  }