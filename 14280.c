void WasmBinaryBuilder::visitGlobalSet(GlobalSet* curr) {
  BYN_TRACE("zz node: GlobalSet\n");
  auto index = getU32LEB();
  if (index < globalImports.size()) {
    auto* import = globalImports[index];
    curr->name = import->name;
  } else {
    Index adjustedIndex = index - globalImports.size();
    if (adjustedIndex >= globals.size()) {
      throwError("invalid global index");
    }
    curr->name = globals[adjustedIndex]->name;
  }
  curr->value = popNonVoidExpression();
  globalRefs[index].push_back(curr); // we don't know the final name yet
  curr->finalize();
}