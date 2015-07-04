#include "Particle.h"
#include "Constraint.h"
#include "../include/gfx/vec2.h"
#include "linearSolver.cpp"

#define iVector Vec2f

using namespace std;

std::vector< std::vector< float > > multiply(std::vector< std::vector< float > > A,
                                             std::vector< std::vector< float > > B) {
    std::vector< std::vector< float > > Result = std::vector< std::vector< float > >(A.size(),
                                                                                     vector< float >(B[0].size(), 0));

    for (int y = 0; y < A[0].size(); y++) {
        for (int x = 0; x < A.size(); x++) {
            float res = 0;

            for (int k = 0; k < A.size(); k++) {

                res += A[k][y] * B[x][k];
            }

            Result[x][y] = res;
        }
    }

    return Result;
}

std::vector< float > multiply(std::vector< float > A, float b) {
    std::vector< float > Return = std::vector< float >(A.size(), 0);

    for (int i = 0; i < A.size(); i++) {
        Return[i] = A[i] * b;
    }

    return Return;
}

std::vector< float > multiply(std::vector< float > A, int b) {
    std::vector< float > Return = std::vector< float >(A.size(), 0);

    for (int i = 0; i < A.size(); i++) {
        Return[i] = A[i] * b;
    }

    return Return;
}

std::vector< float > multiply(std::vector< std::vector< float > > A, std::vector< float > B) {
    std::vector< float > Return = std::vector< float >(A[0].size(), 0);
    for (int y = 0; y < A[0].size(); y++) {
        float result = 0;

        for (int j = 0; j < A.size(); j++) {
            result += A[j][y] * B[j];
        }

        Return[y] = result;
    }

    return Return;
}

std::vector< float > subtract(std::vector< float > A, std::vector< float > B) {
    std::vector< float > Return = vector< float >(A.size(), 0);
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

    float def = 0;

    std::vector< float > qD = vector< float >(innerSize, def);
    std::vector< float > Q = vector< float >(innerSize, def);
    std::vector< std::vector< float > > M = std::vector< std::vector< float > >(innerSize,
                                                                                std::vector< float >(innerSize, def));
    std::vector< std::vector< float > > W = std::vector< std::vector< float > >(innerSize,
                                                                                std::vector< float >(innerSize, def));

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
                                                                                std::vector< float >(innerSize, def));
    std::vector< std::vector< float > > Jt = std::vector< std::vector< float > >(innerSize,
                                                                                 std::vector< float >(cSize, def));
    std::vector< std::vector< float > > JD = std::vector< std::vector< float > >(cSize,
                                                                                 std::vector< float >(innerSize, def));
    for (int i = 0; i < constraints.size(); i++) {
        Constraint *c = constraints[i];
//        iVector vC = c->calcC();
//        iVector vCD = c->calcCD();

//        cout << "Constraint y-C: "<< vC[1] << " y-CD: " << vCD[1] << endl;
        C.push_back(c->calcC());
        CD.push_back(c->calcCD());
        vector< iVector > j = c->j();
        vector< iVector > jD = c->jD();
        vector< int > pIDs = c->getParticleIDs();

        for (int h = 0; h < pIDs.size(); h++) {
//            cout << "Constrained particle J: " << j[0][1] << " JD: " << jD[0][1] << endl;
            int particle = pIDs[h] * dimensions;
            for (int d = 0; d < dimensions; d++) {
                JD[i][particle + d] = jD[h][d];
                J[i][particle + d] = j[h][d];
                Jt[particle + d][i] = j[h][d];
            }
        }
    }

    std::vector< std::vector< float > > JW = std::vector< std::vector< float > >(cSize,
                                                                                 std::vector< float >(innerSize, def));
    std::vector< std::vector< float > > JWJt = std::vector< std::vector< float > >(cSize,
                                                                                   std::vector< float >(innerSize,
                                                                                                        def));

    JW = multiply(J, W);

    JWJt = multiply(JW, Jt);

    vector< float > JDqD = vector< float >(qD.size(), def);
    vector< float > JWQ = vector< float >(Q.size(), def);
    JDqD = multiply(multiply(JD, qD), -1);
    JWQ = multiply(JW, Q);

    vector< float > KsC = multiply(C, Ks);
    vector< float > KdCd = multiply(CD, Kd);

    vector< float > JWJtL = subtract(subtract(subtract(JDqD, JWQ), KsC), KdCd);

    double JWJtLD[JWJtL.size()];
    std::copy(JWJtL.begin(), JWJtL.end(), JWJtLD);

    implicitMatrix *iJWJT = new implicitMatrix(&JWJt);
    double l[constraints.size()];

    int numberOfSteps = 1000;

    ConjGrad(constraints.size(), iJWJT, l, JWJtLD, 1.0f / 1000.0f, &numberOfSteps);

    std::vector< float > lambda(l, l + sizeof l / sizeof l[0]);

    vector< float > Qh = multiply(J, lambda);
    for (int i = 0; i < particles.size(); i++) {
        Particle *p = particles[i];
        int index = dimensions * i;
        for (int d = 0; d < dimensions; d++) {

            p->m_ForceVector[d] += Qh[index + d];

        }
    }
}
