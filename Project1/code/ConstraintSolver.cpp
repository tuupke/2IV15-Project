#include "Particle.h"
#include "Constraint.h"
#include "include/gfx/vec2.h"
#include "linearSolver.cpp"


#define iVector Vec2f

using namespace std;

std::vector< std::vector< float > > multiply(std::vector< std::vector< float > > A,
                                             std::vector< std::vector< float > > B) {
    std::vector< std::vector< float > > Result = std::vector< std::vector< float > >(A.size(),
                                                                                     vector< float >(B[0].size()));

    for (int i = 0; i < A.size(); i++) {
        for (int j = 0; j < B[0].size(); j++) {
            int res = 0;

            for (int k = 0; k < A.size(); k++) {
                res += A[k][i] * B[j][k];
            }

            Result[i][j] = res;
        }
    }

    return Result;
}

std::vector< float > multiply(std::vector< float > A, float b) {
    std::vector< float > Return = std::vector< float >(A.size());

    for (int i = 0; i < A.size(); i++) {
        Return[i] = A[i] * b;
    }

    return Return;
}

std::vector< float > multiply(std::vector< std::vector< float > > A, std::vector< float > B) {
    std::vector< float > Return = std::vector< float >(B.size());
    for (int i = 0; i < B.size(); i++) {
        int result = 0;

        for (int j = 0; j < B.size(); j++) {
            result += A[j][i] * B[j];
        }

        Return[i] = result;
    }

    return Return;
}

std::vector< float > subtract(std::vector< float > A, std::vector< float > B) {
    std::vector< float > Return = vector< float >(A.size());
    for (int i = 0; i < A.size(); i++) {
        Return[i] = A[i] - B[i];
    }

    return Return;
}


void solve(std::vector< Particle * > particles, std::vector< Constraint * > constraints, float Ks, float Kd) {

    if (constraints.empty())
        return;

    int dimensions = 2;

    // Calculate the size of the arrays
    int innerSize = dimensions * particles.size();

    // Initialize all required arrays;
    std::vector< float > qD = vector< float >(innerSize);
    std::vector< float > Q = vector< float >(innerSize);
    std::vector< std::vector< float > > M = std::vector< std::vector< float > >(innerSize,
                                                                                std::vector< float >(innerSize, 0));
    std::vector< std::vector< float > > W = std::vector< std::vector< float > >(innerSize,
                                                                                std::vector< float >(innerSize, 0));

    for (int i = 0; i < particles.size() * dimensions; i += dimensions) {
        Particle *p = particles[i / dimensions];

        for (int d = 0; d < dimensions; d++) {
            M[i + d][i + d] = p->m_Mass;
            W[i + d][i + d] = 1 / p->m_Mass;
            Q[i + d] = p->m_ForceVector[d];
            qD[i + d] = p->m_Velocity[d];
        }
    }

    int cSize = constraints.size();

    vector< float > C = vector< float >();
    vector< float > CD = vector< float >();
    std::vector< std::vector< float > > J = std::vector< std::vector< float > >(cSize,
                                                                                std::vector< float >(innerSize, 0));
    std::vector< std::vector< float > > Jt = std::vector< std::vector< float > >(innerSize,
                                                                                 std::vector< float >(cSize, 0));
    std::vector< std::vector< float > > JD = std::vector< std::vector< float > >(cSize,
                                                                                 std::vector< float >(innerSize, 0));

    for (int i = 0; i < constraints.size(); i++) {
        Constraint *c = constraints[i];
        C[i] = c->calcC();
        CD[i] = c->calcCD();
        vector< iVector > j = c->j();
        vector< iVector > jD = c->jD();
        vector< int > pIDs = c->getParticleIDs();

        for (int h = 0; h < pIDs.size(); h++) {

            int particle = pIDs[h] * dimensions;
            for (int d = 0; d < dimensions; d++) {
                JD[i][particle] = jD[h][d];
                J[i][particle] = j[h][d];
                Jt[particle][i] = j[h][d];
            }
        }
    }

    std::vector< std::vector< float > > JW = std::vector< std::vector< float > >(cSize,
                                                                                 std::vector< float >(innerSize, 0));
    std::vector< std::vector< float > > JWJt = std::vector< std::vector< float > >(cSize,
                                                                                   std::vector< float >(innerSize, 0));

    JW = multiply(J, W);

    JWJt = multiply(JW, Jt);

    vector< float > JDqD = vector< float >(qD.size());
    vector< float > JWQ = vector< float >(Q.size());
    JDqD = multiply(multiply(JD, qD), -1);
    JWQ = multiply(JW, Q);

    vector< float > KsC = multiply(C, Ks);
    vector< float > KdCd = multiply(CD, Kd);

    vector< float > JWJtL = subtract(subtract(subtract(JDqD, JWQ), KsC), KdCd);

    double JWJtLD[JWJtL.size()];
    std::copy(JWJtL.begin(), JWJtL.end(), JWJtLD);

    implicitMatrix *iJWJT = new implicitMatrix(&JWJt);
    double l[constraints.size()];// = double[constraints.size()];


    int numberOfSteps = 100;

    ConjGrad(constraints.size(), iJWJT, l, JWJtLD, 1.0f / 1000.0f, &numberOfSteps);

    std::vector< float > lambda(l, l + sizeof l / sizeof l[0]);

    vector< float > Qh = multiply(Jt, lambda);
    for (int i = 0; i < particles.size(); i++) {
        Particle *p = particles[i];
        int index = dimensions * i;
        for (int d = 0; d < dimensions; d++) {
            p->m_ForceVector += Qh[index + d];
        }
    }
}



