  void CheckCompleted() {
    if (--waiting_callbacks_ <= 0) {
      DispatchCallbacks();
      DCHECK(callbacks_.empty());
      DCHECK(unlimited_callbacks_.empty());

      UsageAndQuotaDispatcherTaskMap& dispatcher_map =
          manager()->usage_and_quota_dispatchers_;
      DCHECK(dispatcher_map.find(std::make_pair(host_, type_)) !=
             dispatcher_map.end());
      dispatcher_map.erase(std::make_pair(host_, type_));
      CallCompleted();
    }
  }
