void WasmBinaryBuilder::visitRefEq(RefEq* curr) {
  BYN_TRACE("zz node: RefEq\n");
  curr->right = popNonVoidExpression();
  curr->left = popNonVoidExpression();
  curr->finalize();
}