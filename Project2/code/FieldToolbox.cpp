#include "FieldToolbox.h"

#define IX_DIM(i, j) ((i)+(m_NumCells+2)*(j)) 
#define ITER_DIM     for(int i = 1; i <= m_NumCells; i++){ for(int j = 1; j <= m_NumCells; j++){
#define ENDITER_DIM  }}

FieldToolbox::FieldToolbox(void)
{
}

FieldToolbox::~FieldToolbox(void)
{
}

void
FieldToolbox::LinearSolver(float a, float c, ScalarField *SolutionField)
{
	int m_NumCells = SolutionField->m_NumCells;

	float *oldField = SolutionField->m_Field;
	SolutionField->m_Field = new float[(m_NumCells+2)*(m_NumCells+2)]; 
	for(int i = 0; i < (m_NumCells+2)*(m_NumCells+2); i++) 
		(*SolutionField)[i] = oldField[i];//0.f;

	for(int kk = 0; kk < 20; kk++)
	{
		ITER_DIM
			 (*SolutionField)[IX_DIM(i,j)] =	(oldField[IX_DIM(i,j)] +
			 ((*SolutionField)[IX_DIM(i-1,j)] + 
			  (*SolutionField)[IX_DIM(i+1,j)] + 
			  (*SolutionField)[IX_DIM(i,j-1)] + 
			  (*SolutionField)[IX_DIM(i,j+1)])*a)/c;
		ENDITER_DIM
		//SolutionField->SetBoundaries();
	}

	delete oldField;
}

void
FieldToolbox::LinearSolver(float a, float c, VectorField *SolutionField)
{
	int m_NumCells = SolutionField->m_NumCells;
	
	Vec2f *oldField = SolutionField->m_Field;
	SolutionField->m_Field = new Vec2f[(m_NumCells+2)*(m_NumCells+2)];
	for(int in = 0; in < (m_NumCells+2)*(m_NumCells+2); in++) {
		(*SolutionField)[in][0] = 0.f;
		(*SolutionField)[in][1] = 0.f;
	}

	for(int kk = 0; kk < 20; kk++)
	{
		ITER_DIM
			(*SolutionField)[IX_DIM(i,j)][0] =	(oldField[IX_DIM(i,j)][0] +
			((*SolutionField)[IX_DIM(i-1,j)][0] + 
			 (*SolutionField)[IX_DIM(i+1,j)][0] + 
			 (*SolutionField)[IX_DIM(i,j-1)][0] + 
			 (*SolutionField)[IX_DIM(i,j+1)][0])*a)/c;
			(*SolutionField)[IX_DIM(i,j)][1] =	(oldField[IX_DIM(i,j)][1] +
			((*SolutionField)[IX_DIM(i-1,j)][1] + 
			 (*SolutionField)[IX_DIM(i+1,j)][1] + 
			 (*SolutionField)[IX_DIM(i,j-1)][1] + 
			 (*SolutionField)[IX_DIM(i,j+1)][1])*a)/c;
		ENDITER_DIM
		//SolutionField->SetBoundaries();
	}

	delete oldField;
}
