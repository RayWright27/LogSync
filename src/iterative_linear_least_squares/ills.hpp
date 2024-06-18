#ifndef ILLS_H
#define ILLS_H

#include "../../external/eigen/Eigen/Dense"

#include <iostream>

using Eigen::MatrixXf;

float illsIterateClockSkew(MatrixXf& A, MatrixXf& b);

float illsEstimateSkew(std::vector<float>& sender_timestamps,
                       std::vector<float>& reciever_timestamps,
                       uint32_t& num_timestamps,
                       uint32_t& cols);

#endif //ILLS_H