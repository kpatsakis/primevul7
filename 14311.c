void WasmBinaryBuilder::visitTableSet(TableSet* curr) {
  BYN_TRACE("zz node: TableSet\n");
  Index tableIdx = getU32LEB();
  if (tableIdx >= tables.size()) {
    throwError("bad table index");
  }
  curr->value = popNonVoidExpression();
  curr->index = popNonVoidExpression();
  curr->finalize();
  // Defer setting the table name for later, when we know it.
  tableRefs[tableIdx].push_back(curr);
}