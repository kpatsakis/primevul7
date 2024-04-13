Name WasmBinaryBuilder::getExceptionTargetName(int32_t offset) {
  BYN_TRACE("getExceptionTarget " << offset << std::endl);
  // We always start parsing a function by creating a block label and pushing it
  // in breakStack in getBlockOrSingleton, so if a 'delegate''s target is that
  // block, it does not mean it targets that block; it throws to the caller.
  if (breakStack.size() - 1 == size_t(offset)) {
    return DELEGATE_CALLER_TARGET;
  }
  size_t index = breakStack.size() - 1 - offset;
  if (index > breakStack.size()) {
    throwError("bad try index (high)");
  }
  BYN_TRACE("exception target " << breakStack[index].name << std::endl);
  auto& ret = breakStack[index];
  // if the delegate/rethrow is in literally unreachable code, then we will not
  // emit it anyhow, so do not note that the target has a reference to it
  if (!willBeIgnored) {
    exceptionTargetNames.insert(ret.name);
  }
  return ret.name;
}