void WasmBinaryBuilder::visitTryOrTryInBlock(Expression*& out) {
  BYN_TRACE("zz node: Try\n");
  auto* curr = allocator.alloc<Try>();
  startControlFlow(curr);
  // For simplicity of implementation, like if scopes, we create a hidden block
  // within each try-body and catch-body, and let branches target those inner
  // blocks instead.
  curr->type = getType();
  curr->body = getBlockOrSingleton(curr->type);

  Builder builder(wasm);
  // A nameless label shared by all catch body blocks
  Name catchLabel = getNextLabel();
  breakStack.push_back({catchLabel, curr->type});

  auto readCatchBody = [&](Type tagType) {
    auto start = expressionStack.size();
    if (tagType != Type::none) {
      pushExpression(builder.makePop(tagType));
    }
    processExpressions();
    size_t end = expressionStack.size();
    if (start > end) {
      throwError("block cannot pop from outside");
    }
    if (end - start == 1) {
      curr->catchBodies.push_back(popExpression());
    } else {
      auto* block = allocator.alloc<Block>();
      pushBlockElements(block, curr->type, start);
      block->finalize(curr->type);
      curr->catchBodies.push_back(block);
    }
  };

  while (lastSeparator == BinaryConsts::Catch ||
         lastSeparator == BinaryConsts::CatchAll) {
    if (lastSeparator == BinaryConsts::Catch) {
      auto index = getU32LEB();
      if (index >= wasm.tags.size()) {
        throwError("bad tag index");
      }
      auto* tag = wasm.tags[index].get();
      curr->catchTags.push_back(tag->name);
      readCatchBody(tag->sig.params);

    } else { // catch_all
      if (curr->hasCatchAll()) {
        throwError("there should be at most one 'catch_all' clause per try");
      }
      readCatchBody(Type::none);
    }
  }
  breakStack.pop_back();

  if (lastSeparator == BinaryConsts::Delegate) {
    curr->delegateTarget = getExceptionTargetName(getU32LEB());
  }

  // For simplicity, we ensure that try's labels can only be targeted by
  // delegates and rethrows, and delegates/rethrows can only target try's
  // labels. (If they target blocks or loops, it is a validation failure.)
  // Because we create an inner block within each try and catch body, if any
  // delegate/rethrow targets those inner blocks, we should make them target the
  // try's label instead.
  curr->name = getNextLabel();
  if (auto* block = curr->body->dynCast<Block>()) {
    if (block->name.is()) {
      if (exceptionTargetNames.find(block->name) !=
          exceptionTargetNames.end()) {
        BranchUtils::replaceExceptionTargets(block, block->name, curr->name);
        exceptionTargetNames.erase(block->name);
      }
    }
  }
  if (exceptionTargetNames.find(catchLabel) != exceptionTargetNames.end()) {
    for (auto* catchBody : curr->catchBodies) {
      BranchUtils::replaceExceptionTargets(catchBody, catchLabel, curr->name);
    }
    exceptionTargetNames.erase(catchLabel);
  }

  // If catch bodies contained stacky code, 'pop's can be nested within a block.
  // Fix that up.
  EHUtils::handleBlockNestedPop(curr, currFunction, wasm);
  curr->finalize(curr->type);

  // For simplicity, we create an inner block within the catch body too, but the
  // one within the 'catch' *must* be omitted when we write out the binary back
  // later, because the 'catch' instruction pushes a value onto the stack and
  // the inner block does not support block input parameters without multivalue
  // support.
  // try
  //   ...
  // catch $e   ;; Pushes value(s) onto the stack
  //   block  ;; Inner block. Should be deleted when writing binary!
  //     use the pushed value
  //   end
  // end
  //
  // But when input binary code is like
  // try
  //   ...
  // catch $e
  //   br 0
  // end
  //
  // 'br 0' accidentally happens to target the inner block, creating code like
  // this in Binaryen IR, making the inner block not deletable, resulting in a
  // validation error:
  // (try
  //   ...
  //   (catch $e
  //     (block $label0 ;; Cannot be deleted, because there's a branch to this
  //       ...
  //       (br $label0)
  //     )
  //   )
  // )
  //
  // When this happens, we fix this by creating a block that wraps the whole
  // try-catch, and making the branches target that block instead, like this:
  // (block $label  ;; New enclosing block, new target for the branch
  //   (try
  //     ...
  //     (catch $e
  //       (block   ;; Now this can be deleted when writing binary
  //         ...
  //         (br $label)
  //       )
  //     )
  //   )
  // )
  if (breakTargetNames.find(catchLabel) == breakTargetNames.end()) {
    out = curr;
  } else {
    // Create a new block that encloses the whole try-catch
    auto* block = builder.makeBlock(catchLabel, curr);
    out = block;
  }
  breakTargetNames.erase(catchLabel);
}