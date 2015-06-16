#include "ScalarField.h"
#include "FieldToolbox.h"

#define CREATE_DIM1  (new float[(a_NumCells+2)*(a_NumCells+2)])
#define CREATE_DIM2  (new float[(CopyField->m_NumCells+2)*(CopyField->m_NumCells+2)])
#define CREATE_DIM   (new float[(m_NumCells+2)*(m_NumCells+2)])
#define IX_DIM(i, j) ((i)+(m_NumCells+2)*(j))
#define ITER_DIM     for(int i = 1; i <= m_NumCells; i++){ for(int j = 1; j <= m_NumCells; j++){
#define ENDITER_DIM  }}

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

void lin_solve(int b, VectorField *x, VectorField *x0, int lr, int lrx, float a, float c) {

    int N = x->m_NumCells;

    for (int k = 0; k < 20; k++) {
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                x->m_Field[IX(i, j)][lr] = (x0->m_Field[IX(i, j)][lrx] + a *
                                                                         (x->m_Field[IX(i - 1, j)][lr] + x->m_Field[IX(i + 1, j)][lr] +
                                                                          x->m_Field[IX(i, j - 1)][lr] +
                                                                          x->m_Field[IX(i, j + 1)][lr])) / c;
            }
        }
        set_bnd(b, x, lr);
    }
}

void
ScalarField::TimeStep(ScalarField *a_SrcField, VectorField *VelocityField) {

    AddField(a_SrcField);
    float diff = 0.0f;

    int N = a_SrcField->m_NumCells;

    float a = a_SrcField->m_Dt * N * N * diff;
    lin_solve(0, )

    // SWAP()
//    diffuse();
//    void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
//    {
//        float a=dt*diff*N*N;
//        lin_solve ( N, 0, x, x0, a, 1+4*a );
//
    // SWAP()
//    advect();

    /*
     * dens_step ( N, dens, dens_prev, u, v, diff, dt );
     * dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
    add_source ( N, x, x0, dt );
	SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
	SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
     */
//    Advection(VelocityField);
/*
  	add_source ( N, x, x0, dt );
	SWAP ( x0, x );
 	diffuse ( N, 0, x, x0, diff, dt );
	SWAP ( x0, x );
 	advect ( N, 0, x, x0, u, v, dt );
 */


    /*****************************************/
    /************* DELETE THIS ***************/
    /*static float change = .001f;
    change += .001f;
    float schange = 0.005f*sin(change);
    schange += 1.f;

    ITER_DIM
        m_Field[IX_DIM(i,j)] *=	schange;
    ENDITER_DIM*/
    /************* DELETE THIS ***************/
    /*****************************************/
}

void
ScalarField::AddField(ScalarField *a_SrcField) {
    for (int i = 0; i < ((m_NumCells + 2) * (m_NumCells + 2)); i++) {
        m_Field[i] += m_Dt * ((*a_SrcField)[i]);
    }
}

