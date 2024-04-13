  bool Config::remove_notifier(const Notifier * n) 
  {
    Vector<Notifier *>::iterator i   = notifier_list.begin();
    Vector<Notifier *>::iterator end = notifier_list.end();

    while (i != end && *i != n)
      ++i;

    if (i == end) {
    
      return false;
    
    } else {

      delete *i;
      notifier_list.erase(i);
      return true;

    }
  }