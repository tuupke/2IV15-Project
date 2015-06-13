#pragma once
class VectorField;

class ScalarField
{
public:
	ScalarField(int a_NumCells, float a_Viscosity, float a_Dt);
	ScalarField(ScalarField *CopyField);
	virtual ~ScalarField(void);

	void	TimeStep(ScalarField *a_SrcField, VectorField *VelocityField);
	void	AddField(ScalarField *a_SrcField);

	float	&operator[](int i)       { return m_Field[i]; }

	int		m_NumCells;
	float	*m_Field;
	float	m_Viscosity;
	float	m_Dt;
};
