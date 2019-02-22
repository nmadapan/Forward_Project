///////////////////////////////////////////////////////////////////////////////
//
//  (C) 2001-2013 Force Dimension
//  All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef CMatrix3dH
#define CMatrix3dH
//---------------------------------------------------------------------------
#include "CConstants.h"
#include "CVector3d.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \struct   cMatrix3d
      \brief    cMatrix3d represents a 3x3 matrix. Each cell of the matrix
                is composed of a double.
*/
//===========================================================================
struct cMatrix3d
{
  public:
	// MEMBERS:
	//! array of doubles describing the matrix.
	double m[3][3];

	// CONSTRUCTOR & DESTRUCTOR:
	//-----------------------------------------------------------------------
	/*!
		Constructor of cMatrix3d.
	*/
	//-----------------------------------------------------------------------
	cMatrix3d(){};


	//-----------------------------------------------------------------------
	/*!
		Initialize a matrix with a scalar  which is copied to each cell of 
		the matrix.

		\param  a_value  Value.
	*/
	//-----------------------------------------------------------------------
	inline void set(const double& a_value)
	{
		m[0][0] = a_value;  m[0][1] = a_value;  m[0][2] = a_value;
		m[1][0] = a_value;  m[1][1] = a_value;  m[1][2] = a_value;
		m[2][0] = a_value;  m[2][1] = a_value;  m[2][2] = a_value;
	}


	//-----------------------------------------------------------------------
	/*!
		Initialize a matrix bypassing as parameter values for each cell.

		\param  a_m00  Matrix Component [0,0]
		\param  a_m01  Matrix Component [0,1]
		\param  a_m02  Matrix Component [0,2]
		\param  a_m10  Matrix Component [1,0]
		\param  a_m11  Matrix Component [1,1]
		\param  a_m12  Matrix Component [1,2]
		\param  a_m20  Matrix Component [2,0]
		\param  a_m21  Matrix Component [2,1]
		\param  a_m22  Matrix Component [2,2]
	*/
	//-----------------------------------------------------------------------
	inline void set(const double& a_m00, const double& a_m01, const double& a_m02,
					const double& a_m10, const double& a_m11, const double& a_m12,
					const double& a_m20, const double& a_m21, const double& a_m22)
	{
		m[0][0] = a_m00;  m[0][1] = a_m01;  m[0][2] = a_m02;
		m[1][0] = a_m10;  m[1][1] = a_m11;  m[1][2] = a_m12;
		m[2][0] = a_m20;  m[2][1] = a_m21;  m[2][2] = a_m22;
	}


	//-----------------------------------------------------------------------
	/*!
		Initialize a matrix by passing as parameter 3 column vectors. M = (V0,V1,V2).

		\param  a_vectCol0  Vector Column 0.
		\param  a_vectCol1  Vector Column 1.
		\param  a_vectCol2  Vector Column 2.
	*/
	//-----------------------------------------------------------------------
	inline void setCol(const cVector3d& a_vectCol0, const cVector3d& a_vectCol1,
					   const cVector3d& a_vectCol2)
	{
		m[0][0] = a_vectCol0.x;  m[0][1] = a_vectCol1.x;  m[0][2] = a_vectCol2.x;
		m[1][0] = a_vectCol0.y;  m[1][1] = a_vectCol1.y;  m[1][2] = a_vectCol2.y;
		m[2][0] = a_vectCol0.z;  m[2][1] = a_vectCol1.z;  m[2][2] = a_vectCol2.z;
	}


	//-----------------------------------------------------------------------
	/*!
		Set column 0 of matrix with vector passed as parameter.

		\param  a_vectCol  Vector Column 0.
	*/
	//-----------------------------------------------------------------------
	inline void setCol0(const cVector3d& a_vectCol)
	{
		m[0][0] = a_vectCol.x;  m[1][0] = a_vectCol.y;  m[2][0] = a_vectCol.z;
	}


	//-----------------------------------------------------------------------
	/*!
		Set column 1 of matrix with vector passed as parameter.

		\param  a_vectCol  Vector Column 1.
	*/
	//-----------------------------------------------------------------------
	inline void setCol1(const cVector3d& a_vectCol)
	{
		m[0][1] = a_vectCol.x;  m[1][1] = a_vectCol.y;  m[2][1] = a_vectCol.z;
	}


	//-----------------------------------------------------------------------
	/*!
		Set column 2 of matrix with vector passed as parameter.

		\param  a_vectCol  Vector Column 2.
	*/
	//-----------------------------------------------------------------------
	inline void setCol2(const cVector3d& a_vectCol)
	{
		m[0][2] = a_vectCol.x;  m[1][2] = a_vectCol.y;  m[2][2] = a_vectCol.z;
	}


	//-----------------------------------------------------------------------
	/*!
		Read column vector 0 of matrix.

		\return  Return vector 0 of current matrix.
	*/
	//-----------------------------------------------------------------------
	inline cVector3d getCol0() const
	{
		cVector3d result;
		result.x = m[0][0];   result.y = m[1][0];     result.z = m[2][0];
		return (result);
	}


	//-----------------------------------------------------------------------
	/*!
		Read column vector 1 of matrix.

		\return  Return vector 1 of current matrix.
	*/
	//-----------------------------------------------------------------------
	inline cVector3d getCol1() const
	{
		cVector3d result;
		result.x = m[0][1];   result.y = m[1][1];     result.z = m[2][1];
		return (result);
	}


	//-----------------------------------------------------------------------
	/*!
		Read column vector 2 of matrix.

		\return  Return vector 2 of current matrix.
	*/
	//-----------------------------------------------------------------------
	inline cVector3d getCol2() const
	{
		cVector3d result;
		result.x = m[0][2];   result.y = m[1][2];     result.z = m[2][2];
		return (result);
	}


	//-----------------------------------------------------------------------
	/*!
		Copy current matrix values to an external matrix passed as parameter.

		\param    a_destination  Destination matrix.
	*/
	//-----------------------------------------------------------------------
	inline void copyto(cMatrix3d& a_destination) const
	{
		a_destination.m[0][0] = m[0][0];  a_destination.m[0][1] = m[0][1];  a_destination.m[0][2] = m[0][2];
		a_destination.m[1][0] = m[1][0];  a_destination.m[1][1] = m[1][1];  a_destination.m[1][2] = m[1][2];
		a_destination.m[2][0] = m[2][0];  a_destination.m[2][1] = m[2][1];  a_destination.m[2][2] = m[2][2];
	}


	//-----------------------------------------------------------------------
	/*!
		Copy values from an external matrix passed as parameter to current 
		matrix.

		\param    a_source  Source matrix.
	*/
	//-----------------------------------------------------------------------
	inline void copyfrom(const cMatrix3d& a_source)
	{
		m[0][0] = a_source.m[0][0];  m[0][1] = a_source.m[0][1];  m[0][2] = a_source.m[0][2];
		m[1][0] = a_source.m[1][0];  m[1][1] = a_source.m[1][1];  m[1][2] = a_source.m[1][2];
		m[2][0] = a_source.m[2][0];  m[2][1] = a_source.m[2][1];  m[2][2] = a_source.m[2][2];
	}


	//-----------------------------------------------------------------------
	/*!
		Set the identity matrix.
	*/
	//-----------------------------------------------------------------------
	inline void identity()
	{
		m[0][0] = 1.0;  m[0][1] = 0.0;  m[0][2] = 0.0;
		m[1][0] = 0.0;  m[1][1] = 1.0;  m[1][2] = 0.0;
		m[2][0] = 0.0;  m[2][1] = 0.0;  m[2][2] = 1.0;
	}


	//-----------------------------------------------------------------------
	/*!
		Multiply current matrix with an external matrix. M = M * a_matrix.
		Result is stored in current matrix.

		\param    a_matrix  Matrix with which multiplication is performed.
	*/
	//-----------------------------------------------------------------------
	inline void mul(const cMatrix3d& a_matrix)
	{
		// compute multiplication between both matrices
		double m00 = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] + m[0][2] * a_matrix.m[2][0];
		double m01 = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] + m[0][2] * a_matrix.m[2][1];
		double m02 = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] + m[0][2] * a_matrix.m[2][2];
		double m10 = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] + m[1][2] * a_matrix.m[2][0];
		double m11 = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] + m[1][2] * a_matrix.m[2][1];
		double m12 = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] + m[1][2] * a_matrix.m[2][2];
		double m20 = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] + m[2][2] * a_matrix.m[2][0];
		double m21 = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] + m[2][2] * a_matrix.m[2][1];
		double m22 = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] + m[2][2] * a_matrix.m[2][2];

		// return values to current matrix
		m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;
		m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;
		m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;
	}


	//-----------------------------------------------------------------------
	/*!
		Multiply current matrix with an external matrix. a_result = M * a_matrix.
		Result is stored in a_result matrix.

		\param  a_matrix  Matrix with which multiplication is performed.
		\param  a_result  Result matrix.
	*/
	//-----------------------------------------------------------------------
	inline void mulr(const cMatrix3d& a_matrix, cMatrix3d& a_result) const
	{
		// compute multiplication between both matrices
		a_result.m[0][0] = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] + m[0][2] * a_matrix.m[2][0];
		a_result.m[0][1] = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] + m[0][2] * a_matrix.m[2][1];
		a_result.m[0][2] = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] + m[0][2] * a_matrix.m[2][2];
		a_result.m[1][0] = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] + m[1][2] * a_matrix.m[2][0];
		a_result.m[1][1] = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] + m[1][2] * a_matrix.m[2][1];
		a_result.m[1][2] = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] + m[1][2] * a_matrix.m[2][2];
		a_result.m[2][0] = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] + m[2][2] * a_matrix.m[2][0];
		a_result.m[2][1] = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] + m[2][2] * a_matrix.m[2][1];
		a_result.m[2][2] = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] + m[2][2] * a_matrix.m[2][2];
	}


	//-----------------------------------------------------------------------
	/*!
		Multiply current matrix with an external vector passed as parameter. 
		a_vector = M * a_vector. Result is stored in same vector.

		\param  a_vector  Vector with which multiplication is performed.
						  Result is stored is same vector.
	*/
	//-----------------------------------------------------------------------
	inline void mul(cVector3d& a_vector)
	{
		// compute multiplication
		double x = m[0][0] * a_vector.x + m[0][1] * a_vector.y + m[0][2] * a_vector.z;
		double y = m[1][0] * a_vector.x + m[1][1] * a_vector.y + m[1][2] * a_vector.z;
		double z = m[2][0] * a_vector.x + m[2][1] * a_vector.y + m[2][2] * a_vector.z;

		// store result
		a_vector.x = x;
		a_vector.y = y;
		a_vector.z = z;
	}


	//-----------------------------------------------------------------------
	/*!
		Multiply current matrix with a vector. a_result = M * a_vector.
		Result is stored in result vector a_result.
																`
		\param  a_vector  Vector with which multiplication is performed.
		\param  a_result  Result of multiplication is stored here.
	*/
	//-----------------------------------------------------------------------
	inline void mulr(const cVector3d& a_vector, cVector3d& a_result) const
	{
		// compute multiplication
		a_result.x = m[0][0] * a_vector.x + m[0][1] * a_vector.y + m[0][2] * a_vector.z;
		a_result.y = m[1][0] * a_vector.x + m[1][1] * a_vector.y + m[1][2] * a_vector.z;
		a_result.z = m[2][0] * a_vector.x + m[2][1] * a_vector.y + m[2][2] * a_vector.z;
	}


	//-----------------------------------------------------------------------
	/*!
		Compute the determinant of  current matrix.

		\return Return the determinant of current matrix.
	*/
	//-----------------------------------------------------------------------
	inline double det() const
	{
		return (+ m[0][0] * m[1][1] * m[2][2]
				+ m[0][1] * m[1][2] * m[2][0]
				+ m[0][2] * m[1][0] * m[2][1]
				- m[2][0] * m[1][1] * m[0][2]
				- m[2][1] * m[1][2] * m[0][0]
				- m[2][2] * m[1][0] * m[0][1]);
	}


	//-----------------------------------------------------------------------
	/*!
		Compute the transpose of current matrix. 
		Result is stored in current matrix.
	*/
	//-----------------------------------------------------------------------
	inline void trans()
	{
		double t;

		t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t;
		t = m[0][2]; m[0][2] = m[2][0]; m[2][0] = t;
		t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
	}


	//-----------------------------------------------------------------------
	/*!
		Compute the transpose of current matrix.
		Result is stored in result matrix.

		\param  a_result  Result is stored here.
	*/
	//-----------------------------------------------------------------------
	inline void transr(cMatrix3d& a_result) const
	{
		a_result.m[0][0] = m[0][0];
		a_result.m[0][1] = m[1][0];
		a_result.m[0][2] = m[2][0];

		a_result.m[1][0] = m[0][1];
		a_result.m[1][1] = m[1][1];
		a_result.m[1][2] = m[2][1];

		a_result.m[2][0] = m[0][2];
		a_result.m[2][1] = m[1][2];
		a_result.m[2][2] = m[2][2];
	}


	//-----------------------------------------------------------------------
	/*!
		Compute the inverse of current matrix.
		If the operation succeeds, result is stored in current matrix.

		\return Returns \b true if matrix was inverted successfully,
				otherwize return \b false.
	*/
	//-----------------------------------------------------------------------
	bool invert()
	{
		// compute determinant
		double det = (+ m[0][0] * m[1][1] * m[2][2]
				        + m[0][1] * m[1][2] * m[2][0]
				    	+ m[0][2] * m[1][0] * m[2][1]
					    - m[2][0] * m[1][1] * m[0][2]
					    - m[2][1] * m[1][2] * m[0][0]
					    - m[2][2] * m[1][0] * m[0][1]);

		// check if determinant null
		if ((det < CONST_TINY) && (det > -CONST_TINY))
		{
			// determinant null, matrix invertion could not be performed
			return (false);
		}
		else
		{
			// compute inverted matrix
			double m00 =  (m[1][1] * m[2][2] - m[2][1]*m[1][2]) / det;
			double m01 = -(m[0][1] * m[2][2] - m[2][1]*m[0][2]) / det;
			double m02 =  (m[0][1] * m[1][2] - m[1][1]*m[0][2]) / det;

			double m10 = -(m[1][0] * m[2][2] - m[2][0]*m[1][2]) / det;
			double m11 =  (m[0][0] * m[2][2] - m[2][0]*m[0][2]) / det;
			double m12 = -(m[0][0] * m[1][2] - m[1][0]*m[0][2]) / det;

			double m20 =  (m[1][0] * m[2][1] - m[2][0]*m[1][1]) / det;
			double m21 = -(m[0][0] * m[2][1] - m[2][0]*m[0][1]) / det;
			double m22 =  (m[0][0] * m[1][1] - m[1][0]*m[0][1]) / det;

			// return values to current matrix
			m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;
			m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;
			m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;

			// return success
			return (true);
		}
	}


	//-----------------------------------------------------------------------
	/*!
		Compute the inverse of current matrix.
		If the operation succeeds, result is stored in result matrix passed
		as parameter.

		\param  a_result  Result is stored here.
		\return Return \b true if matrix was inverted successfully,
				otherwize return \b false.
	*/
	//-----------------------------------------------------------------------
	bool invertr(cMatrix3d& a_result) const
	{
		// compute determinant
		double det = (+ m[0][0] * m[1][1] * m[2][2]
					  + m[0][1] * m[1][2] * m[2][0]
					  + m[0][2] * m[1][0] * m[2][1]
					  - m[2][0] * m[1][1] * m[0][2]
					  - m[2][1] * m[1][2] * m[0][0]
					  - m[2][2] * m[1][0] * m[0][1]);

		// check if determinant null.
		if ((det < CONST_TINY) && (det > -CONST_TINY))
		{
			// determinant null, matrix invertion can not be performed
			return (false);
		}
		else
		{
			// compute inverted matrix
			a_result.m[0][0] =  (m[1][1] * m[2][2] - m[2][1]*m[1][2]) / det;
			a_result.m[0][1] = -(m[0][1] * m[2][2] - m[2][1]*m[0][2]) / det;
			a_result.m[0][2] =  (m[0][1] * m[1][2] - m[1][1]*m[0][2]) / det;

			a_result.m[1][0] = -(m[1][0] * m[2][2] - m[2][0]*m[1][2]) / det;
			a_result.m[1][1] =  (m[0][0] * m[2][2] - m[2][0]*m[0][2]) / det;
			a_result.m[1][2] = -(m[0][0] * m[1][2] - m[1][0]*m[0][2]) / det;

			a_result.m[2][0] =  (m[1][0] * m[2][1] - m[2][0]*m[1][1]) / det;
			a_result.m[2][1] = -(m[0][0] * m[2][1] - m[2][0]*m[0][1]) / det;
			a_result.m[2][2] =  (m[0][0] * m[1][1] - m[1][0]*m[0][1]) / det;

			// return success
			return (true);
		}
	}


	//-----------------------------------------------------------------------
	/*!
		Build a rotation matrix defined by a rotation axis and rotation
		angle given in radian. These values are passed as parameters.
		Result is stored in current matrix.
		
		\param  a_axis  Axis of rotation.
		\param  a_angleRad  Rotation angle in Radian.
		\return Return \b true if operation succeded. Otherwize 
				return \b false.
	*/
	//-----------------------------------------------------------------------
	inline bool set(const cVector3d& a_axis, const double& a_angleRad)
	{
		// compute length of axis vector
		double length = a_axis.length();

		// check length of axis vector
		if (length < CONST_TINY)
		{
			// rotation matrix could not be computed because axis vector is not defined
			return (false);
		}

		// normalize axis vector
		double x = a_axis.x / length;
		double y = a_axis.y / length;
		double z = a_axis.z / length;

		// compute rotation matrix
		double c = cos(a_angleRad);
		double s = sin(a_angleRad);
		double v = 1-c;

		m[0][0] = x*x*v+c;     m[0][1] = x*y*v-z*s;  m[0][2] = x*z*v+y*s;
		m[1][0] = x*y*v+z*s;   m[1][1] = y*y*v+c;    m[1][2] = y*z*v-x*s;
		m[2][0] = x*z*v-y*s;   m[2][1] = y*z*v+x*s;  m[2][2] = z*z*v+c;

		// return success
		return (true);
	}


	//-----------------------------------------------------------------------
	/*!
		Rotate current matrix arround an axis an angle defined as parameters.

		\param  a_axis  Axis of rotation.
		\param  a_angleRad  Rotation angle in Radian.
		\return Return \b true if operation succeded. Otherwize 
				return \b false.
	*/
	//-----------------------------------------------------------------------
	inline bool rotate(const cVector3d& a_axis, const double& a_angleRad)
	{
		// compute length of axis vector
		double length = a_axis.length();

		// check length of axis vector
		if (length < CONST_TINY)
		{
			// rotation matrix could not be computed because axis vector is not defined
			return (false);
		}

		// normalize axis vector
		double x = a_axis.x / length;
		double y = a_axis.y / length;
		double z = a_axis.z / length;

		// compute rotation matrix
		double c = cos(a_angleRad);
		double s = sin(a_angleRad);
		double v = 1-c;

		double m00 = x*x*v+c;     double m01 = x*y*v-z*s;  double m02 = x*z*v+y*s;
		double m10 = x*y*v+z*s;   double m11 = y*y*v+c;    double m12 = y*z*v-x*s;
		double m20 = x*z*v-y*s;   double m21 = y*z*v+x*s;  double m22 = z*z*v+c;

		// compute multiplication between both matrices
		double tm00 = m00 * m[0][0] + m01 * m[1][0] + m02 * m[2][0];
		double tm01 = m00 * m[0][1] + m01 * m[1][1] + m02 * m[2][1];
		double tm02 = m00 * m[0][2] + m01 * m[1][2] + m02 * m[2][2];
		double tm10 = m10 * m[0][0] + m11 * m[1][0] + m12 * m[2][0];
		double tm11 = m10 * m[0][1] + m11 * m[1][1] + m12 * m[2][1];
		double tm12 = m10 * m[0][2] + m11 * m[1][2] + m12 * m[2][2];
		double tm20 = m20 * m[0][0] + m21 * m[1][0] + m22 * m[2][0];
		double tm21 = m20 * m[0][1] + m21 * m[1][1] + m22 * m[2][1];
		double tm22 = m20 * m[0][2] + m21 * m[1][2] + m22 * m[2][2];

		// store new values to current matrix
		m[0][0] = tm00;  m[0][1] = tm01;  m[0][2] = tm02;
		m[1][0] = tm10;  m[1][1] = tm11;  m[1][2] = tm12;
		m[2][0] = tm20;  m[2][1] = tm21;  m[2][2] = tm22;

		// return success
		return (true);
	}


	//-----------------------------------------------------------------------
	/*!
		Rotate current matrix arround an axis an angle defined as parameters.
		Result is stored in result matrix.

		\param  a_axis  Axis of rotation.
		\param  a_angleRad  Rotation angle in Radian.
		\param  a_result  Result is stored here.
		\return Return \b true if operation succeded. Otherwize 
				return \b false.
	*/
	//-----------------------------------------------------------------------
	inline bool rotater(const cVector3d& a_axis, const double& a_angleRad, cMatrix3d& a_result) const
	{
		// compute length of axis vector
		double length = a_axis.length();

		// check length of axis vector
		if (length < CONST_TINY)
		{
			// rotation matrix could not be computed because axis vector is not defined
			return (false);
		}

		// normalize axis vector
		double x = a_axis.x / length;
		double y = a_axis.y / length;
		double z = a_axis.z / length;

		// compute rotation matrix
		double c = cos(a_angleRad);
		double s = sin(a_angleRad);
		double v = 1-c;

		double m00 = x*x*v+c;     double m01 = x*y*v-z*s;  double m02 = x*z*v+y*s;
		double m10 = x*y*v+z*s;   double m11 = y*y*v+c;    double m12 = y*z*v-x*s;
		double m20 = x*z*v-y*s;   double m21 = y*z*v+x*s;  double m22 = z*z*v+c;

		// compute multiplication between both matrices
		a_result.m[0][0] = m00 * m[0][0] + m01 * m[1][0] + m02 * m[2][0];
		a_result.m[0][1] = m00 * m[0][1] + m01 * m[1][1] + m02 * m[2][1];
		a_result.m[0][2] = m00 * m[0][2] + m01 * m[1][2] + m02 * m[2][2];
		a_result.m[1][0] = m10 * m[0][0] + m11 * m[1][0] + m12 * m[2][0];
		a_result.m[1][1] = m10 * m[0][1] + m11 * m[1][1] + m12 * m[2][1];
		a_result.m[1][2] = m10 * m[0][2] + m11 * m[1][2] + m12 * m[2][2];
		a_result.m[2][0] = m20 * m[0][0] + m21 * m[1][0] + m22 * m[2][0];
		a_result.m[2][1] = m20 * m[0][1] + m21 * m[1][1] + m22 * m[2][1];
		a_result.m[2][2] = m20 * m[0][2] + m21 * m[1][2] + m22 * m[2][2];

		// return success
		return (true);
	}
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
