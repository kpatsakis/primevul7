void WasmBinaryBuilder::readFunctions() {
  BYN_TRACE("== readFunctions\n");
  size_t total = getU32LEB();
  if (total != functionTypes.size() - functionImports.size()) {
    throwError("invalid function section size, must equal types");
  }
  for (size_t i = 0; i < total; i++) {
    BYN_TRACE("read one at " << pos << std::endl);
    auto sizePos = pos;
    size_t size = getU32LEB();
    if (size == 0) {
      throwError("empty function size");
    }
    endOfFunction = pos + size;

    auto* func = new Function;
    func->name = Name::fromInt(i);
    func->type = getTypeByFunctionIndex(functionImports.size() + i);
    currFunction = func;

    if (DWARF) {
      func->funcLocation = BinaryLocations::FunctionLocations{
        BinaryLocation(sizePos - codeSectionLocation),
        BinaryLocation(pos - codeSectionLocation),
        BinaryLocation(pos - codeSectionLocation + size)};
    }

    readNextDebugLocation();

    BYN_TRACE("reading " << i << std::endl);

    readVars();

    std::swap(func->prologLocation, debugLocation);
    {
      // process the function body
      BYN_TRACE("processing function: " << i << std::endl);
      nextLabel = 0;
      debugLocation.clear();
      willBeIgnored = false;
      // process body
      assert(breakStack.empty());
      assert(breakTargetNames.empty());
      assert(exceptionTargetNames.empty());
      assert(expressionStack.empty());
      assert(controlFlowStack.empty());
      assert(letStack.empty());
      assert(depth == 0);
      // Even if we are skipping function bodies we need to not skip the start
      // function. That contains important code for wasm-emscripten-finalize in
      // the form of pthread-related segment initializations. As this is just
      // one function, it doesn't add significant time, so the optimization of
      // skipping bodies is still very useful.
      auto currFunctionIndex = functionImports.size() + functions.size();
      bool isStart = startIndex == currFunctionIndex;
      if (!skipFunctionBodies || isStart) {
        func->body = getBlockOrSingleton(func->getResults());
      } else {
        // When skipping the function body we need to put something valid in
        // their place so we validate. An unreachable is always acceptable
        // there.
        func->body = Builder(wasm).makeUnreachable();

        // Skip reading the contents.
        pos = endOfFunction;
      }
      assert(depth == 0);
      assert(breakStack.empty());
      assert(breakTargetNames.empty());
      assert(exceptionTargetNames.empty());
      if (!expressionStack.empty()) {
        throwError("stack not empty on function exit");
      }
      assert(controlFlowStack.empty());
      assert(letStack.empty());
      if (pos != endOfFunction) {
        throwError("binary offset at function exit not at expected location");
      }
    }

    if (!wasm.features.hasGCNNLocals()) {
      TypeUpdating::handleNonDefaultableLocals(func, wasm);
    }

    std::swap(func->epilogLocation, debugLocation);
    currFunction = nullptr;
    debugLocation.clear();
    functions.push_back(func);
  }
  BYN_TRACE(" end function bodies\n");
}