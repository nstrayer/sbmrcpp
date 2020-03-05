/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

// All test files should include the <testthat.h>
// header file.
#include <testthat.h>
#include "Node_Container.h"

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Basic loading unipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};


  test_that("Sizing is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name);

    expect_true(nodes.size() == 3);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name);

    expect_true(nodes.get_nodes_of_type(0).start_index == 0);
    expect_true(nodes.get_nodes_of_type(0).size == 3);
  }


}


context("Basic loading bipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "b2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "b"};
  const Rcpp::CharacterVector types_name{"a", "b"};

  test_that("Sizing is correct") {
    expect_true(Node_Container(nodes_id, nodes_type, types_name).size() == 4);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name);

    expect_true(nodes.get_nodes_of_type(0).start_index == 0);
    expect_true(nodes.get_nodes_of_type(0).size == 2);
    expect_true(nodes.get_nodes_of_type(1).start_index == 2);
    expect_true(nodes.get_nodes_of_type(1).size == 2);
  }
}


context("Basic loading tripartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "c1", "c2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "c", "c"};
  const Rcpp::CharacterVector types_name{"a", "b", "c"};

  test_that("Sizing is correct") {
    expect_true(Node_Container(nodes_id, nodes_type, types_name).size() == 5);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name);

    expect_true(nodes.get_nodes_of_type(0).start_index == 0);
    expect_true(nodes.get_nodes_of_type(0).size == 2);
    expect_true(nodes.get_nodes_of_type(1).start_index == 2);
    expect_true(nodes.get_nodes_of_type(1).size == 1);
    expect_true(nodes.get_nodes_of_type(2).start_index == 3);
    expect_true(nodes.get_nodes_of_type(2).size == 2);
  }


}
