void WasmBinaryBuilder::visitCallIndirect(CallIndirect* curr) {
  BYN_TRACE("zz node: CallIndirect\n");
  auto index = getU32LEB();
  curr->heapType = getTypeByIndex(index);
  Index tableIdx = getU32LEB();
  // TODO: Handle error cases where `heapType` is not a signature?
  auto num = curr->heapType.getSignature().params.size();
  curr->operands.resize(num);
  curr->target = popNonVoidExpression();
  for (size_t i = 0; i < num; i++) {
    curr->operands[num - i - 1] = popNonVoidExpression();
  }
  // Defer setting the table name for later, when we know it.
  tableRefs[tableIdx].push_back(curr);
  curr->finalize();
}