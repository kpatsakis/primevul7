void WasmBinaryBuilder::visitGlobalGet(GlobalGet* curr) {
  BYN_TRACE("zz node: GlobalGet " << pos << std::endl);
  auto index = getU32LEB();
  if (index < globalImports.size()) {
    auto* import = globalImports[index];
    curr->name = import->name;
    curr->type = import->type;
  } else {
    Index adjustedIndex = index - globalImports.size();
    if (adjustedIndex >= globals.size()) {
      throwError("invalid global index");
    }
    auto& glob = globals[adjustedIndex];
    curr->name = glob->name;
    curr->type = glob->type;
  }
  globalRefs[index].push_back(curr); // we don't know the final name yet
}