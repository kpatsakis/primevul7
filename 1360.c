Status ConstantFolding::MaterializeOutputValues(
    NodeDef* node, const GraphProperties& properties) {
  const std::vector<OpInfo::TensorProperties>& output =
      properties.GetOutputProperties(node->name());
  if (output.size() != 1 || !output[0].has_value() ||
      !IsFoldable(*node, &properties)) {
    return Status::OK();
  }

  // If this is a trivial Identity node with a constant input, just route the
  // input around it.
  if (IsIdentity(*node)) {
    NodeDef* input = node_map_->GetNode(node->input(0));
    if (IsReallyConstant(*input)) {
      std::vector<int> inputs_to_forward;
      std::iota(inputs_to_forward.begin(), inputs_to_forward.end(), 0);
      graph_modified_ = ForwardInputs(node, inputs_to_forward);
      return Status::OK();
    }
  }
  // Repurpose the existing node to be the constant.
  // Device placement is preserved.
  TensorProto value_copy = output[0].value();
  return ReplaceOperationWithConstantTensor(output[0].dtype(), &value_copy,
                                            node, graph_);
}