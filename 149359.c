void ciEnv::report_failure(const char* reason) {
  EventCompilationFailure event;
  if (event.should_commit()) {
    event.set_compileId(compile_id());
    event.set_failureMessage(reason);
    event.commit();
  }
}