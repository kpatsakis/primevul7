inline bool Context::has_out_of_memory() {
  return native_context()->out_of_memory()->IsTrue();
}