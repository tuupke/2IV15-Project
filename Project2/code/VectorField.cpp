#include "VectorField.h"
#include "FieldToolbox.h"

#define CREATE_DIM1 (new Vec2f[(a_NumCells+2)*(a_NumCells+2)])
#define CREATE_DIM2 (new Vec2f[(CopyField->m_NumCells+2)*(CopyField->m_NumCells+2)])
#define CREATE_DIM  (new Vec2f[(m_NumCells+2)*(m_NumCells+2)])
#define IX_DIM(i,j) ((i)+(m_NumCells+2)*(j))
#define ITER_DIM    for(int i = 1; i <= m_NumCells; i++){ for(int j = 1; j <= m_NumCells; j++){
#define ENDITER_DIM }}

VectorField::VectorField(int a_NumCells, float a_Viscosity, float a_Dt)
: m_NumCells(a_NumCells), m_Field(CREATE_DIM1), m_Viscosity(a_Viscosity), m_Dt(a_Dt)
{
	for(int i = 0; i < (m_NumCells+2)*(m_NumCells+2); i++) 
		m_Field[i][0] = m_Field[i][1] = 0.f;
}

VectorField::VectorField(VectorField *CopyField)
: m_NumCells(CopyField->m_NumCells), m_Field(CREATE_DIM2), m_Viscosity(CopyField->m_Viscosity), m_Dt(CopyField->m_Dt)
{
	for(int i = 0; i < (m_NumCells+2)*(m_NumCells+2); i++){
		m_Field[i][0] = (*CopyField)[i][0];
		m_Field[i][1] = (*CopyField)[i][1];
	}
}

VectorField::~VectorField(void)
{
	delete m_Field;
}

void 
VectorField::TimeStep(VectorField *a_SrcField, VectorField *VelocityField)
{
	AddField(a_SrcField);
	//Advection(VelocityField);	// Add this function
	//VorticityConfinement();	// Add this function
	//Projection();				// Add this function

	/*****************************************/
	/************* DELETE THIS ***************/
	static float change = .001f;
	change += .001f;
	float schange = 0.005f*sin(change);
	schange += 1.f;
	
	ITER_DIM
		m_Field[IX_DIM(i,j)][0] *=	schange;
		m_Field[IX_DIM(i,j)][1] *=	schange;
	ENDITER_DIM
	/************* DELETE THIS ***************/
	/*****************************************/
}

void
VectorField::AddField(VectorField *a_SrcField)
{
	for(int i=0; i<((m_NumCells+2)*(m_NumCells+2)); i++) {
		m_Field[i][0] += m_Dt*((*a_SrcField)[i][0]);
		m_Field[i][1] += m_Dt*((*a_SrcField)[i][1]);
	}
}

