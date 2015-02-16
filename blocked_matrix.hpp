#pragma once

#include "dynamic_matrix.hpp"

template <typename T>
using blocked_matrix = dynamic_matrix<fixed_size_matrix<T>>;
