#include "ills.hpp"

using Eigen::MatrixXf;
using Eigen::VectorXf;



float illsEstimateSkew(std::vector<float>& sender_timestamps,
                       std::vector<float>& reciever_timestamps,
                       uint32_t& num_timestamps,
                       uint32_t& cols){
    
    uint32_t curr_set_size = num_timestamps - 1;
    float clock_skew_curr = 0;
    float clock_skew_old = 0;
    bool first_iter = true;
    
    /* Step 4: Use linear least squares method to fit a line until there is 
    at most 1 point in the valid set. */
    while(curr_set_size > 1){

        /* timestamps, leaving sender*/
        MatrixXf A(curr_set_size - 1, cols);
        /* timestamps, arriving at reciever*/
        MatrixXf b(curr_set_size - 1, 1);

        /* Step 1. Set all data point to the valid set*/
        for(uint32_t i = 0; i < curr_set_size-1; i++){
            A(i,0) = sender_timestamps.at(i+1) - sender_timestamps.at(0);
            A(i,1) = -1;
            b(i,0) = reciever_timestamps.at(i+1) - reciever_timestamps.at(0);
        }

        //std::cout << A << std::endl;
        //std::cout << b << std::endl;

        /* Step 2. Use linear least squares method to fit a line to the data 
        points in the valid set */
        clock_skew_curr = illsIterateClockSkew(A, b);
        
        if((clock_skew_old == clock_skew_curr) && !first_iter)
            break;
        first_iter = false;

        std::cout << "current skew = " << clock_skew_curr <<"\n";

        /* Step 3. Remove the points which are above the created line from the 
        valid set*/
        for(uint32_t x = sender_timestamps.size() - 1; x > 0; x--){
            /* */
            if(sender_timestamps.at(x)*clock_skew_curr < 
                reciever_timestamps.at(x)){
                sender_timestamps.erase(sender_timestamps.begin() + x);
                reciever_timestamps.erase(reciever_timestamps.begin() + x);
                curr_set_size--;
            }
        }

        clock_skew_old = clock_skew_curr;
    }

    /* Step 5: The last created line’s slope is the estimated clock skew. */
    return clock_skew_curr;
}

float illsIterateClockSkew(MatrixXf& A, MatrixXf& b){
    uint32_t cols = 2;
    uint32_t num_timestamps = 10; //rows
    MatrixXf X_result(2, 1);

    MatrixXf Eps(num_timestamps - 1, 1);

    // TODO: check correctness - skew calculates on first iteration
    X_result = (A.transpose()*A).inverse()*A.transpose()*b;

    return X_result.coeff(0,0);
}