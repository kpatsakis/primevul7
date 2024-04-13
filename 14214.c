void WasmBinaryBuilder::visitLet(Block* curr) {
  // A let is lowered into a block that contains the value, and we allocate
  // locals as needed, which works as we remove non-nullability.

  startControlFlow(curr);
  // Get the output type.
  curr->type = getType();
  // Get the new local types. First, get the absolute index from which we will
  // start to allocate them.
  requireFunctionContext("let");
  Index absoluteStart = currFunction->vars.size();
  readVars();
  Index numNewVars = currFunction->vars.size() - absoluteStart;
  // Assign the values into locals.
  Builder builder(wasm);
  for (Index i = 0; i < numNewVars; i++) {
    auto* value = popNonVoidExpression();
    curr->list.push_back(builder.makeLocalSet(absoluteStart + i, value));
  }
  // Read the body, with adjusted local indexes.
  letStack.emplace_back(LetData{numNewVars, absoluteStart});
  curr->list.push_back(getBlockOrSingleton(curr->type));
  letStack.pop_back();
  curr->finalize(curr->type);
}