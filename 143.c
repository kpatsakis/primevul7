  void CallCallbacksAndClear(
      CallbackList* callbacks, QuotaStatusCode status,
      int64 usage, int64 quota) {
    for (CallbackList::iterator iter = callbacks->begin();
         iter != callbacks->end(); ++iter) {
      (*iter)->Run(status, usage, quota);
      delete *iter;
    }
    callbacks->clear();
  }
