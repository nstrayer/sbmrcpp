#include <testthat.h>
#include <vector>
#include "vector_helpers.h"
#include "Node.h"



context("Efficient deletion of elements in integer vector") {

  // Setup a basic vector
  std::vector<int> count_vec{0,1,2,3,4};

  test_that("Sizing reflects deletion") {
    // Before deleting we have 5 elements in vector
    expect_true(count_vec.size() == 5);
    // 2nd element of vector should be number 2
    expect_true(count_vec.at(2) == 2);


    // Delete the element "2"
    // {0,1,2,3,4} Swap 2 with end of vector
    // {0,1,4,3,2} Remove last element of vector
    // {0,1,4,3,_}
    delete_from_vector(count_vec, 2);

    // Should have one less element
    expect_true(count_vec.size() == 4);
    // Second element should now be 4
    expect_true(count_vec.at(2) == 4);
  }
}

context("Efficient deletion of elements in Node vector") {

  //  Setup a vector of unique node pointers
  std::vector<std::unique_ptr<Node>> node_vec;
  node_vec.emplace_back(new Node(0, 0, 2));
  node_vec.emplace_back(new Node(1, 0, 2));
  node_vec.emplace_back(new Node(2, 1, 2));
  node_vec.emplace_back(new Node(3, 1, 2));

  test_that("Sizing reflects deletion") {
    // Before deleting we have 5 elements in vector
    expect_true(node_vec.size() == 4);
    // Node in second slot should have an index of 1
    expect_true(node_vec.at(1)->index == 1);

    Node * second_node_raw_ptr = node_vec.at(1).get();

    // Remove that node in second slow
    delete_from_vector(node_vec, second_node_raw_ptr);

    // Should have one less element
    expect_true(node_vec.size() == 3);
    // Second node index should now be 3
    expect_true(node_vec.at(1)->index == 3);
  }
}

context("Random selection of an element in nested vectors") {

  std::vector<std::vector<int>> vec_of_vecs;

  // Make a vector of three different vectors each with three integers
  vec_of_vecs.push_back({0,1,2});
  vec_of_vecs.push_back({3,4,5});
  vec_of_vecs.push_back({6,7,8});

  test_that("Sizing is proper") {
    expect_true(vec_of_vecs.size() == 3);
    expect_true(vec_of_vecs[0].size() == 3);
    expect_true(vec_of_vecs[1].size() == 3);
    expect_true(vec_of_vecs[2].size() == 3);
  }

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  const int num_samples = 1000;

  int num_times_4 = 0;

  for (int i = 0; i < num_samples; i++) {
    const int sampled_int = get_random_element(vec_of_vecs, random_engine);

    if(sampled_int == 4) num_times_4++;
  }

  const double proportion_4 = double(num_times_4)/double(num_samples);
  const double thresh = 0.01;
  const double true_prop = 1.0/9.0;
  // Hope that four is chosen 1/9th of the time
  expect_true(proportion_4 > true_prop - thresh);
  expect_true(proportion_4 < true_prop + thresh);

}


context("Random selection of an element in flat vectors") {

  std::vector<int> vec{0,1,2,3,4,5,6,7,8};

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  const int num_samples = 1000;

  int num_times_4 = 0;

  for (int i = 0; i < num_samples; i++) {
    const int sampled_int = get_random_element(vec, random_engine);

    if(sampled_int == 4) num_times_4++;
  }

  const double proportion_4 = double(num_times_4)/double(num_samples);
  const double thresh = 0.01;
  const double true_prop = 1.0/9.0;
  // Hope that four is chosen 1/9th of the time
  expect_true(proportion_4 > true_prop - thresh);
  expect_true(proportion_4 < true_prop + thresh);

}

