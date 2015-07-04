//
// Created by Mart Pluijmaekers on 23/05/15.
//

#ifndef CODE_MULTIPLY_H
#define CODE_MULTIPLY_H


class Multiply {

    std::vector<std::vector<float>> multiply (std::vector<std::vector<float>> A, std::vector<std::vector<float>> B){
        std::vector<std::vector<float>> Result = std::vector<std::vector<float>>(A.size(), vector<float>(B[0].size()));

        for(int i = 0; i < A.size(); i++){
            for(int j = 0; j < B[0].size(); j++){
                int res = 0;

                for(int k = 0; k < A.size(); k++){
                    res += A[k][i] * B[j][k];
                }

                Result[i][j] = res;
            }
        }

        return Result;
    }

    std::vector<float> multiply(std::vector<float> A, float b){
        std::vector<float> Return = std::vector<float>(A.size());

        for(int i = 0; i < A.size(); i++){
            Return[i] = A[i] * b;
        }

        return Return;
    }

    std::vector<float> multiply(std::vector<std::vector<float>> A, std::vector<float> B){
        std::vector<float Return = std::vector<float>(B.size());
        for(int i = 0; i < B.size(); i++){
            int result = 0;

            for(int j = 0; j < B.size(); j++){
                result += A[j][i] * B[j];
            }

            Return[i] = result;
        }

        return Return;
    }

};


#endif //CODE_MULTIPLY_H
