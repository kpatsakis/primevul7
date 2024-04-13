void ciEnv::validate_compile_task_dependencies(ciMethod* target) {
  if (failing())  return;  // no need for further checks

  Dependencies::DepType result = dependencies()->validate_dependencies(_task);
  if (result != Dependencies::end_marker) {
    if (result == Dependencies::call_site_target_value) {
      _inc_decompile_count_on_failure = false;
      record_failure("call site target change");
    } else if (Dependencies::is_klass_type(result)) {
      record_failure("concurrent class loading");
    } else {
      record_failure("invalid non-klass dependency");
    }
  }
}