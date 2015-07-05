#include "VectorField.h"
#include "FieldToolbox.h"
#include <stdio.h>
#include <math.h>

#define IX(i, j) ((i)+(N+2)*(j))
#define CREATE_DIM1 (new Vec2f[(a_NumCells+2)*(a_NumCells+2)])
#define CREATE_DIM2 (new Vec2f[(CopyField->m_NumCells+2)*(CopyField->m_NumCells+2)])
#define CREATE_DIM  (new Vec2f[(m_NumCells+2)*(m_NumCells+2)])
#define IX_DIM(i, j) ((i)+(m_NumCells+2)*(j))
#define ITER_DIM    for(int i = 1; i <= m_NumCells; i++){ for(int j = 1; j <= m_NumCells; j++){
#define ENDITER_DIM }}


VectorField::VectorField(int a_NumCells, float a_Viscosity, float a_Dt)
        : m_NumCells(a_NumCells), m_Field(CREATE_DIM1), m_Viscosity(a_Viscosity), m_Dt(a_Dt) {
    for (int i = 0; i < (m_NumCells + 2) * (m_NumCells + 2); i++)
        m_Field[i][0] = m_Field[i][1] = 0.f;
}

VectorField::VectorField(VectorField *CopyField)
        : m_NumCells(CopyField->m_NumCells), m_Field(CREATE_DIM2), m_Viscosity(CopyField->m_Viscosity),
          m_Dt(CopyField->m_Dt) {
    for (int i = 0; i < (m_NumCells + 2) * (m_NumCells + 2); i++) {
        m_Field[i][0] = (*CopyField)[i][0];
        m_Field[i][1] = (*CopyField)[i][1];
    }
}

VectorField::~VectorField(void) {
    delete m_Field;
}

void set_bnd(int b, VectorField *vect, int lr) {
    int N = vect->m_NumCells;

    for (int i = 1; i <= N; i++) {
        vect->m_Field[IX(0, i)][lr] = b == 1 ? -vect->m_Field[IX(1, i)][lr] : vect->m_Field[IX(1, i)][lr];
        vect->m_Field[IX(N + 1, i)][lr] = b == 1 ? -vect->m_Field[IX(N, i)][lr] : vect->m_Field[IX(N, i)][lr];
        vect->m_Field[IX(i, 0)][lr] = b == 2 ? -vect->m_Field[IX(i, 1)][lr] : vect->m_Field[IX(i, 1)][lr];
        vect->m_Field[IX(i, N + 1)][lr] = b == 2 ? -vect->m_Field[IX(i, N)][lr] : vect->m_Field[IX(i, N)][lr];
    }

    vect->m_Field[IX(0, 0)][lr] = 0.5f * (vect->m_Field[IX(1, 0)][lr] + vect->m_Field[IX(0, 1)][lr]);
    vect->m_Field[IX(0, N + 1)][lr] = 0.5f * (vect->m_Field[IX(1, N + 1)][lr] + vect->m_Field[IX(0, N)][lr]);
    vect->m_Field[IX(N + 1, 0)][lr] = 0.5f * (vect->m_Field[IX(N, 0)][lr] + vect->m_Field[IX(N + 1, 1)][lr]);
    vect->m_Field[IX(N + 1, N + 1)][lr] = 0.5f * (vect->m_Field[IX(N, N + 1)][lr] + vect->m_Field[IX(N + 1, N)][lr]);
}

void lin_solve(int b, VectorField *x, VectorField *x0, int lr, int lrx, float a, float c) {

    int N = x0->m_NumCells;

    for (int k = 0; k < 20; k++) {
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                x->m_Field[IX(i, j)][lr] = (x0->m_Field[IX(i, j)][lrx] + a *
                                                                         (x->m_Field[IX(i - 1, j)][lr] +
                                                                          x->m_Field[IX(i + 1, j)][lr] +
                                                                          x->m_Field[IX(i, j - 1)][lr] +
                                                                          x->m_Field[IX(i, j + 1)][lr])) / c;
            }
        }
        set_bnd(b, x, lr);
    }
}

// N, u, v, u0, v0
// int N, float * u, float * v, float * p, float * div
void project(VectorField *a_SrcField, VectorField *VelocityField) {
    int N = VelocityField->m_NumCells;

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            VelocityField->m_Field[IX(i, j)][1] =
                    -0.5f * (
                            a_SrcField->m_Field[IX(i + 1, j)][0]
                            - a_SrcField->m_Field[IX(i - 1, j)][0]
                            + a_SrcField->m_Field[IX(i, j + 1)][1]
                            - a_SrcField->m_Field[IX(i, j - 1)][1]
                    ) / N;
            VelocityField->m_Field[IX(i, j)][0] = 0;
        }
    }

    set_bnd(0, VelocityField, 1);
    set_bnd(0, VelocityField, 0);

    lin_solve(0, VelocityField, VelocityField, 0, 1, 1, 4);

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            a_SrcField->m_Field[IX(i, j)][0] -=
                    0.5f * N * (VelocityField->m_Field[IX(i + 1, j)][0] - VelocityField->m_Field[IX(i - 1, j)][0]);
            a_SrcField->m_Field[IX(i, j)][1] -=
                    0.5f * N * (VelocityField->m_Field[IX(i, j + 1)][0] - VelocityField->m_Field[IX(i, j - 1)][0]);
        }
    }

    set_bnd(1, a_SrcField, 0);
    set_bnd(2, a_SrcField, 1);
}

void advect(int b, VectorField *A, VectorField *B, VectorField *C, VectorField *D, int lrA, int lrB, int lrC, int lrD,
            float dt) {
    int N = A->m_NumCells;
    float dt0 = dt * N;

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            float x = i - dt0 * C->m_Field[IX(i, j)][lrC];
            float y = j - dt0 * D->m_Field[IX(i, j)][lrD];

            if (x < 0.5f) x = 0.5f;
            if (x > N + 0.5f) x = N + 0.5f;

            if (y < 0.5f) y = 0.5f;
            if (y > N + 0.5f) y = N + 0.5f;

            int i0 = (int) x;
            int i1 = i0 + 1;

            int j0 = (int) y;
            int j1 = j0 + 1;

            float s1 = x - i0;
            float s0 = 1 - s1;
            float t1 = y - j0;
            float t0 = 1 - t1;

            A->m_Field[IX(i, j)][lrA] = s0 * (t0 * B->m_Field[IX(i0, j0)][lrB] + t1 * B->m_Field[IX(i0, j1)][lrB]) +
                                        s1 * (t0 * B->m_Field[IX(i1, j0)][lrB] + t1 * B->m_Field[IX(i1, j1)][lrB]);
        }
    }
    set_bnd(b, A, lrA);
}

void vorticityConfinement(VectorField *slachtoffer) {
    int N = slachtoffer->m_NumCells;
    float *forces = new float[(N + 2) * (N + 2)]();
    int *sign = new int[(N + 2) * (N + 2)]();

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            float dudy = (slachtoffer->m_Field[IX(i, j + 1)][0] - slachtoffer->m_Field[IX(i, j - 1)][0]) / 2;
            float dvdx = (slachtoffer->m_Field[IX(i + 1, j)][1] - slachtoffer->m_Field[IX(i - 1, j)][1]) / 2;

            float toStore = dvdx - dudy;
            forces[IX(i, j)] = abs(toStore);

            sign[IX(i, j)] = toStore < 0 ? -1 : 1;

        }
    }

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {

            float dwdx = (forces[IX(i + 1, j)] - forces[IX(i - 1, j)]) * 0.5f;
            float dwdy = (forces[IX(i, j + 1)] - forces[IX(i, j - 1)]) * 0.5f;

            float length = sqrt(dwdx * dwdx + dwdy * dwdy);

            if(length != 0){
                dwdx /= length;
                dwdy /= length;
            }

            float v = sign[IX(i, j)] * forces[IX(i, j)];

            slachtoffer->m_Field[IX(i, j)][0] = dwdy * -v;
            slachtoffer->m_Field[IX(i, j)][1] = dwdx * v;
        }
    }

    delete forces;
    delete sign;
}

void
VectorField::TimeStep(VectorField *a_SrcField, VectorField *VelocityField, std::vector<RigidBody*> &bodies) {
    AddField(VelocityField);

    if (VorticityConfinement) {
	    vorticityConfinement(VelocityField);
    }

    int N = a_SrcField->m_NumCells;
    float a = a_SrcField->m_Dt * VelocityField->m_Viscosity * N * N;

    // SWAP()
    lin_solve(1, VelocityField, a_SrcField, 0, 0, a, 1 + 4 * a);
    lin_solve(2, VelocityField, a_SrcField, 1, 1, a, 1 + 4 * a);

    for(int i = 0; i < bodies.size(); i++){
        bodies[i]->act(a_SrcField, VelocityField);
    }

    project(VelocityField, a_SrcField);
    // SWAP()

    advect(1, a_SrcField, VelocityField, VelocityField, VelocityField, 0, 0, 0, 1, a_SrcField->m_Dt);
    advect(2, a_SrcField, VelocityField, VelocityField, VelocityField, 1, 1, 0, 1, a_SrcField->m_Dt);

    project(a_SrcField, VelocityField);

    for(int i = 0; i < bodies.size(); i++){
        bodies[i]->emptyBody(a_SrcField, VelocityField);
    }
}

void
VectorField::AddField(VectorField *a_SrcField) {
    for (int i = 0; i < ((m_NumCells + 2) * (m_NumCells + 2)); i++) {
        m_Field[i][0] += m_Dt * ((*a_SrcField)[i][0]);
        m_Field[i][1] += m_Dt * ((*a_SrcField)[i][1]);
    }
}
