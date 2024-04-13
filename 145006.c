  bool Config::replace_notifier(const Notifier * o, 
				      Notifier * n) 
  {
    Vector<Notifier *>::iterator i   = notifier_list.begin();
    Vector<Notifier *>::iterator end = notifier_list.end();

    while (i != end && *i != o)
      ++i;

    if (i == end) {
    
      return false;
    
    } else {

      delete *i;
      *i = n;
      return true;

    }
  }