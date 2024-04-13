bool ConstantFolding::ReduceDivToReciprocalMul(GraphDef* optimized_graph,
                                               NodeDef* node) {
  // Strength reduce floating point division by a constant Div(x, const) to
  // multiplication by the reciprocal Mul(x, Reciprocal(const)). This in turn
  // will be constant folded to Mul(x, 1.0/const).
  if (node->input_size() >= 2 &&
      (IsDiv(*node) || IsRealDiv(*node) || IsXdivy(*node))) {
    const string& const_input = node->input(1);
    const NodeDef* denom = node_map_->GetNode(const_input);
    CHECK(denom != nullptr);
    if (!IsReallyConstant(*denom)) {
      return false;
    }
    if (node->attr().count("T") == 0) {
      return false;
    }
    DataType type = node->attr().at("T").type();
    // Skip integer division.
    if (IsDiv(*node) &&
        !(DataTypeIsFloating(type) || DataTypeIsComplex(type))) {
      return false;
    }
    // Insert new reciprocal op and change node from Div to Mul.
    NodeDef* reciprocal_node = optimized_graph->add_node();
    reciprocal_node->set_name(OptimizedNodeName(*node, "_recip"));
    reciprocal_node->set_op("Reciprocal");
    reciprocal_node->set_device(node->device());
    reciprocal_node->add_input(const_input);
    (*reciprocal_node->mutable_attr())["T"].set_type(type);

    // Re-wire inputs and outputs.
    if (IsXdivy(*node)) {
      node->set_op("MulNoNan");
      node->set_input(1, node->input(0));
      node->set_input(0, reciprocal_node->name());
    } else {
      node->set_op("Mul");
      node->set_input(1, reciprocal_node->name());
    }
    node_map_->AddNode(reciprocal_node->name(), reciprocal_node);
    node_map_->UpdateOutput(node->name(), const_input, reciprocal_node->name());

    return true;
  }

  return false;
}