#include "ScalarField.h"
#include "FieldToolbox.h"

#define CREATE_DIM1  (new float[(a_NumCells+2)*(a_NumCells+2)])
#define CREATE_DIM2  (new float[(CopyField->m_NumCells+2)*(CopyField->m_NumCells+2)])
#define CREATE_DIM   (new float[(m_NumCells+2)*(m_NumCells+2)])
#define IX_DIM(i, j) ((i)+(m_NumCells+2)*(j))
#define ITER_DIM     for(int i = 1; i <= m_NumCells; i++){ for(int j = 1; j <= m_NumCells; j++){
#define ENDITER_DIM  }}
#define IX(i,j) ((i)+(N+2)*(j))

ScalarField::ScalarField(int a_NumCells, float a_Viscosity, float a_Dt)
        : m_NumCells(a_NumCells), m_Field(CREATE_DIM1), m_Viscosity(a_Viscosity), m_Dt(a_Dt) {
    for (int i = 0; i < (m_NumCells + 2) * (m_NumCells + 2); i++)
        m_Field[i] = 0.f;
}

ScalarField::ScalarField(ScalarField *CopyField)
        : m_NumCells(CopyField->m_NumCells), m_Field(CREATE_DIM2), m_Viscosity(CopyField->m_Viscosity),
          m_Dt(CopyField->m_Dt) {
    for (int i = 0; i < (m_NumCells + 2) * (m_NumCells + 2); i++)
        m_Field[i] = (*CopyField)[i];
}

ScalarField::~ScalarField(void) {
    delete m_Field;
}

void set_bnd(int b, ScalarField *vect) {
    int N = vect->m_NumCells;

    for (int i = 1; i <= vect->m_NumCells; i++) {
        vect->m_Field[IX(0, i)] = b == 1 ? -vect->m_Field[IX(1, i)] : vect->m_Field[IX(1, i)];
        vect->m_Field[IX(N + 1, i)] = b == 1 ? -vect->m_Field[IX(N, i)] : vect->m_Field[IX(N, i)];
        vect->m_Field[IX(i, 0)] = b == 2 ? -vect->m_Field[IX(i, 1)] : vect->m_Field[IX(i, 1)];
        vect->m_Field[IX(i, N + 1)] = b == 2 ? -vect->m_Field[IX(i, N)] : vect->m_Field[IX(i, N)];
    }

    vect->m_Field[IX(0, 0)] = 0.5f * (vect->m_Field[IX(1, 0)] + vect->m_Field[IX(0, 1)]);
    vect->m_Field[IX(0, N + 1)] = 0.5f * (vect->m_Field[IX(1, N + 1)] + vect->m_Field[IX(0, N)]);
    vect->m_Field[IX(N + 1, 0)] = 0.5f * (vect->m_Field[IX(N, 0)] + vect->m_Field[IX(N + 1, 1)]);
    vect->m_Field[IX(N + 1, N + 1)] = 0.5f * (vect->m_Field[IX(N, N + 1)] + vect->m_Field[IX(N + 1, N)]);
}

void lin_solve(int b, ScalarField *x, ScalarField *x0, float a, float c) {

    int N = x->m_NumCells;

    for (int k = 0; k < 20; k++) {
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                x->m_Field[IX(i, j)] = (x0->m_Field[IX(i, j)] + a *
                                                                (x->m_Field[IX(i - 1, j)] + x->m_Field[IX(i + 1, j)] +
                                                                 x->m_Field[IX(i, j - 1)] +
                                                                 x->m_Field[IX(i, j + 1)])) / c;
            }
        }
        set_bnd(b, x);
    }
}

void advect(int b, ScalarField *A, ScalarField *B, VectorField *C, VectorField *D, int lrC, int lrD,
            float dt) {
    int N = A->m_NumCells;
    float dt0 = dt * N;

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            float x = i - dt0 * C->m_Field[IX(i, j)][lrC];
            float y = j - dt0 * D->m_Field[IX(i, j)][lrD];

            x = std::max(x, 0.5f);
            x = std::min(x, N + 0.5f);

            y = std::max(y, 0.5f);
            y = std::min(y, N + 0.5f);

            int i0 = (int) x;
            int i1 = i0 + 1;

            int j0 = (int) y;
            int j1 = j0 + 1;

            float s1 = x - i0;
            float s0 = 1 - s1;
            float t1 = y - j0;
            float t0 = 1 - t1;

            A->m_Field[IX(i, j)] = s0 * (t0 * B->m_Field[IX(i0, j0)] + t1 * B->m_Field[IX(i0, j1)]) +
                                   s1 * (t0 * B->m_Field[IX(i1, j0)] + t1 * B->m_Field[IX(i1, j1)]);
        }
    }
    set_bnd(b, A);
}

void//                                 x                        x0                      (u,v)
ScalarField::TimeStep(ScalarField *a_SrcField, ScalarField *a_PrevField, VectorField *VelocityField) {

    AddField(a_SrcField);
    float diff = 0.0f;

    int N = a_SrcField->m_NumCells;

    // These 2 are an advect step
    float a = a_SrcField->m_Dt * N * N * diff;
    lin_solve(0, a_PrevField, a_SrcField, a, 1 + 4 * a);

    advect(0, a_SrcField, a_PrevField, VelocityField, VelocityField, 0, 1, a_SrcField->m_Dt);
}

void
ScalarField::AddField(ScalarField *a_SrcField) {
    for (int i = 0; i < ((m_NumCells + 2) * (m_NumCells + 2)); i++) {
        m_Field[i] += m_Dt * ((*a_SrcField)[i]);
    }
}

