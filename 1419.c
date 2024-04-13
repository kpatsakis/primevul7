bool ConstantFolding::PartialConstPropThroughIdentityN(NodeDef* node) {
  // Partial constant propagation through IdentityN.
  if (!(IsIdentityN(*node) || IsIdentityNSingleInput(*node)) ||
      !HasRegularInputs(*node))
    return false;

  std::vector<int> inputs_to_forward;
  for (int input_idx = 0; input_idx < node->input_size(); ++input_idx) {
    const string& input = node->input(input_idx);
    if (IsControlInput(input)) {
      return false;
    }
    const NodeDef* input_node = node_map_->GetNode(NodeName(input));
    if (input_node == nullptr) {
      LOG(ERROR) << "Bad input: " << input;
      return false;
    }
    // Forward constant inputs to outputs and add a control dependency on
    // the IdentityN node.
    if (IsReallyConstant(*input_node)) {
      inputs_to_forward.push_back(input_idx);
    }
  }
  return ForwardInputs(node, inputs_to_forward);
}