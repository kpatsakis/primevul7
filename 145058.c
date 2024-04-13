  NotifierEnumeration * Config::notifiers() const 
  {
    return new NotifierEnumeration(notifier_list);
  }