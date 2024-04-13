Status ConstantFolding::AddQuantizedMatMulMinMaxOutConstNodes(
    NodeDef* node, GraphDef* optimized_graph) {
  auto add_quantized_out = [this, node, optimized_graph](
                               const string& out_const_name, int index) {
    NodeDef* out_node = optimized_graph->add_node();
    graph_modified_ = true;
    Tensor value(DT_FLOAT, TensorShape({}));
    const bool is_min = index == 1;
    const DataType type_attr = node->attr().at("dtype").type();

    value.flat<float>()(0) = is_min ? QuantizedTypeMinAsFloat(type_attr)
                                    : QuantizedTypeMaxAsFloat(type_attr);
    TF_RETURN_IF_ERROR(
        CreateNodeDef(out_const_name, TensorValue(&value), out_node));
    node_map_->AddNode(out_const_name, out_node);
    out_node->set_device(node->device());
    // Copy all inputs from node.
    out_node->mutable_input()->CopyFrom(node->input());
    for (const string& input : out_node->input()) {
      node_map_->AddOutput(NodeName(input), out_const_name);
    }

    // Update output nodes consuming node:index to new const node.
    string old_input = absl::StrCat(node->name(), ":", index);
    int old_node_count = 0;
    // We make a copy since the set might change.
    auto outputs = node_map_->GetOutputs(node->name());
    for (const auto& output : outputs) {
      for (int i = 0; i < output->input_size(); ++i) {
        if (output->input(i) == old_input) {
          output->set_input(i, out_const_name);
          node_map_->AddOutput(out_const_name, output->name());
        } else if (NodeName(output->input(i)) == node->name()) {
          ++old_node_count;
        }
      }
      if (old_node_count == 0) {
        node_map_->RemoveOutput(node->name(), output->name());
      }
    }

    return Status::OK();
  };
  const string min_out_const_name =
      OptimizedNodeName(*node, "-quantized_matmul_min_out");
  const string max_out_const_name =
      OptimizedNodeName(*node, "-quantized_matmul_max_out");
  if (node_map_->GetNode(min_out_const_name) == nullptr &&
      node_map_->GetNode(max_out_const_name) == nullptr) {
    TF_RETURN_IF_ERROR(add_quantized_out(min_out_const_name, 1));
    TF_RETURN_IF_ERROR(add_quantized_out(max_out_const_name, 2));
  } else {
    return errors::Internal(absl::Substitute(
        "Can't create Const for QuantizedMatMul min_out/max_out of "
        "node '$0' because of node name conflict",
        node->name()));
  }
  return Status::OK();
}