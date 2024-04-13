void WasmBinaryBuilder::visitRefFunc(RefFunc* curr) {
  BYN_TRACE("zz node: RefFunc\n");
  Index index = getU32LEB();
  // We don't know function names yet, so record this use to be updated later.
  // Note that we do not need to check that 'index' is in bounds, as that will
  // be verified in the next line. (Also, note that functionRefs[index] may
  // write to an odd place in the functionRefs map if index is invalid, but that
  // is harmless.)
  functionRefs[index].push_back(curr);
  // To support typed function refs, we give the reference not just a general
  // funcref, but a specific subtype with the actual signature.
  curr->finalize(Type(getTypeByFunctionIndex(index), NonNullable));
}