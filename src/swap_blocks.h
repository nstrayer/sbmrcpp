#ifndef __SWAP_BLOCK_INCLUDED__
#define __SWAP_BLOCK_INCLUDED__

#include "Node_Container.h"
#include "vector_helpers.h"

void swap_block(Node* child_node,
                Node* new_block,
                Node_Container& blocks,
                const bool remove_empty = true) {
  Node* old_block = child_node->get_parent();

  child_node->set_parent(new_block);

  new_block->add_child(child_node);

  old_block->remove_child(child_node);

  // If the old block is now empty and we're removing empty blocks, delete it
  if (remove_empty & (old_block->num_children() == 0)) {
    auto& blocks_of_type = blocks.get_nodes_of_type(old_block->type_index);

    const bool delete_successful =
        delete_from_vector(blocks_of_type, old_block);

    if (!delete_successful)
      stop("Tried to delete a node that doesn't exist");
  }
}

#endif
