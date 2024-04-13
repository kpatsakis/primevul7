  bool Config::add_notifier(Notifier * n) 
  {
    Vector<Notifier *>::iterator i   = notifier_list.begin();
    Vector<Notifier *>::iterator end = notifier_list.end();

    while (i != end && *i != n)
      ++i;

    if (i != end) {
    
      return false;
    
    } else {

      notifier_list.push_back(n);
      return true;

    }
  }