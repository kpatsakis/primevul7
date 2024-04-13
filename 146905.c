    ~ExceptionScope() {
      isolate_->set_catcher(catcher_);
      isolate_->set_pending_exception(*pending_exception_);
    }