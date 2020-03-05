#include <testthat.h>
#include "Edge_Container.h"

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Basic unipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name);

  // Fully connected network (Except a3, which is not connected to itself)
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a1", "a2", "a2"};
  const Rcpp::CharacterVector   edges_to{"a1", "a2", "a3", "a2", "a3"};

  test_that("Sizing is correct") {
    auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

    expect_true(edges.size() == 5);
  }


}

