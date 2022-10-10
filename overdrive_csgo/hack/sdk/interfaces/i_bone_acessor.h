class matrix3x4_t;

class CBoneAccessor
{
public:
	inline CBoneAccessor( )
	{
		m_pAnimating = 0;
		m_pBones = 0;
		m_ReadableBones = m_WritableBones = 0;
	}

	const int* m_pAnimating;

	matrix3x4_t* m_pBones;

	int m_ReadableBones;        
	int m_WritableBones;
};