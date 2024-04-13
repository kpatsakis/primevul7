Index WasmBinaryBuilder::getAbsoluteLocalIndex(Index index) {
  // Wasm binaries put each let at the bottom of the index space, which may be
  // good for binary size as often the uses of the let variables are close to
  // the let itself. However, in Binaryen IR we just have a simple flat index
  // space of absolute values, which we add to as we parse, and we depend on
  // later optimizations to reorder locals for size.
  //
  // For example, if we have $x, then we add a let with $y, the binary would map
  // 0 => y, 1 => x, while in Binaryen IR $x always stays at 0, and $y is added
  // at 1.
  //
  // Compute the relative index in the let we were added. We start by looking at
  // the last let added, and if we belong to it, we are already relative to it.
  // We will continue relativizing as we go down, til we find our let.
  int64_t relative = index;
  for (auto i = int64_t(letStack.size()) - 1; i >= 0; i--) {
    auto& info = letStack[i];
    int64_t currNum = info.num;
    // There were |currNum| let items added in this let. Check if we were one of
    // them.
    if (relative < currNum) {
      return info.absoluteStart + relative;
    }
    relative -= currNum;
  }
  // We were not a let, but a normal var from the beginning. In that case, after
  // we subtracted the let items, we have the proper absolute index.
  return relative;
}