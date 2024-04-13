  String figure_out_dir(ParmString dir, ParmString file)
  {
    String temp;
    int s = file.size() - 1;
    while (s != -1 && file[s] != '/') --s;
    if (need_dir(file)) {
      temp += dir;
      temp += '/';
    }
    if (s != -1) {
      temp.append(file, s);
    }
    return temp;
  }