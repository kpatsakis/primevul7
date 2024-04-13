void WasmBinaryBuilder::visitTableGet(TableGet* curr) {
  BYN_TRACE("zz node: TableGet\n");
  Index tableIdx = getU32LEB();
  if (tableIdx >= tables.size()) {
    throwError("bad table index");
  }
  curr->index = popNonVoidExpression();
  curr->type = tables[tableIdx]->type;
  curr->finalize();
  // Defer setting the table name for later, when we know it.
  tableRefs[tableIdx].push_back(curr);
}