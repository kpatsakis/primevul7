    explicit ExceptionScope(Isolate* isolate) :
      // Scope currently can only be used for regular exceptions, not
      // failures like OOM or termination exception.
      isolate_(isolate),
      pending_exception_(isolate_->pending_exception()->ToObjectUnchecked()),
      catcher_(isolate_->catcher())
    { }