///////////////////////////////////////////////////////////////////////////////
//
//  (C) 2001-2013 Force Dimension
//  All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef CMathsH
#define CMathsH
//---------------------------------------------------------------------------
#include "CMatrix3d.h"
#include <math.h>
//---------------------------------------------------------------------------

//===========================================================================
/*!
	Compute the cosine of an angle defined in degrees.

	\param		a_angleDeg  Angle in degrees.
	\return		Return the cosine of angle.
*/
//===========================================================================
inline double cCosDeg(double a_angleDeg)
{
	return (cos(a_angleDeg * CONST_DEG2RAD));
}


//===========================================================================
/*!
	Compute the sine of an angle defined in degrees.

	\param		a_angleDeg  Angle in degrees.
	\return		Return the sine of angle.
*/
//===========================================================================
inline double cSinDeg(double a_angleDeg)
{
	return (sin(a_angleDeg * CONST_DEG2RAD));
}


//===========================================================================
/*!
	Compute the tangent of an angle defined in degrees.

	\param		a_angleDeg  Angle in degrees.
	\return		Return the tangent of angle.
*/
//===========================================================================
inline double cTanDeg(double a_angleDeg)
{
	return (tan(a_angleDeg * CONST_DEG2RAD));
}


//===========================================================================
/*!
	Return the cosine of an angle defined in radians.

	\param		a_angleRad  Angle in radians.
	\return		Return the cosine of angle.
*/
//===========================================================================
inline double cCosRad(double a_angleRad)
{
	return (cos(a_angleRad));
}


//===========================================================================
/*!
	Return the sine of an angle defined in radians.

	\param		a_angleRad  Angle in radians.
	\return		Return the sine of angle.
*/
//===========================================================================
inline double cSinRad(double iValue)
{
	return (sin(iValue));
}


//===========================================================================
/*!
	Return the tangent of an angle defined in radians.

	\param		a_angleRad  Angle in radians.
	\return		Return the tangent of angle.
*/
//===========================================================================
inline double cTanRad(double iValue)
{
	return (tan(iValue));
}


//===========================================================================
/*!
	Convert an angle from degrees to radians.

	\param		a_angleDeg  Angle in degrees.
	\return		Return angle in radians.
*/
//===========================================================================
inline double cDegToRad(double a_angleDeg)
{
	return (a_angleDeg * CONST_DEG2RAD);
}


//===========================================================================
/*!
	Convert an angle from radians to degrees

	\param		a_angleRad  Angle in radians.
	\return		Return angle in degrees.
*/
//===========================================================================
inline double cRadToDeg(double a_angleRad)
{
	return (a_angleRad * CONST_RAD2DEG);
}


//===========================================================================
/*!
	Compute the addition between two vectors. Result = Vector1 + Vector2.

	\param    a_vector1  First vector.
	\param    a_vector2  Second vector.
	\return   Return the addition of both vectors.
*/
//===========================================================================
inline cVector3d cAdd(const cVector3d& a_vector1, const cVector3d& a_vector2)
{
	cVector3d result;
	a_vector1.addr(a_vector2, result);
	return (result);
}


//===========================================================================
/*!
	Compute the subtraction between two vectors. Result = Vector1 - Vector2.

	\param    a_vector1  First vector.
	\param    a_vector2  Second vector.
	\return   Return result of subtraction.
*/
//===========================================================================
inline cVector3d cSub(const cVector3d& a_vector1, const cVector3d& a_vector2)
{
	cVector3d result;
	a_vector1.subr(a_vector2, result);
	return (result);
}


//===========================================================================
/*!
	Multiply a vector by a scalar.

	\param		a_value   Scalar.
	\param		a_vector  Input vector.
	\return		Returns result of multiplication.
*/
//===========================================================================
inline cVector3d cMul(const double& a_value, const cVector3d& a_vector)
{
	cVector3d result;
	a_vector.mulr(a_value, result);
	return (result);
}


//===========================================================================
/*!
	Divide a vector by a scalar.

	\param		a_value   Scalar.
	\param		a_vector  Input vector.
	\return		Returns result of division.
*/
//===========================================================================
inline cVector3d cDiv(const double& a_value, const cVector3d& a_vector)
{
	cVector3d result;
	a_vector.divr(a_value, result);
	return (result);
}


//===========================================================================
/*!
	Compute the cross product between two 3D vectors.

	\param		a_vector1  First vector.
	\param		a_vector2  Second vector.
	\return		Returns the cross product between both vectors.
*/
//===========================================================================
inline cVector3d cCross(const cVector3d& a_vector1, const cVector3d& a_vector2)
{
	cVector3d result;
	a_vector1.crossr(a_vector2, result);
	return (result);
}


//===========================================================================
/*!
	Compute the dot product between two vectors.

	\param		a_vector1  First vector.
	\param		a_vector2  Second vector.
	\return		Returns the dot product between between both vectors.
*/
//===========================================================================
inline double cDot(const cVector3d& a_vector1, const cVector3d& a_vector2)
{
	return(a_vector1.dot(a_vector2));
}


//===========================================================================
/*!
	Compute the normalized vector (length = 1) of an input vector.

	\param		a_vector  Input vector.
	\return		Returns the normalized vector.
*/
//===========================================================================
inline cVector3d cNormalize(const cVector3d& a_vector)
{
	cVector3d result;
	a_vector.normalizer(result);
	return (result);
}


//===========================================================================
/*!
	Compute the distance between two points.

	\param		a_point1  First point.
	\param		a_point2  Second point.
	\return		Return distance between both points
*/
//===========================================================================
inline double cDistance(const cVector3d& a_point1, const cVector3d& a_point2)
{
	return ( a_point1.distance(a_point2) );
}


//===========================================================================
/*!
	Return the Identity Matrix

	\return   Return the identity matrix.
*/
//===========================================================================
inline cMatrix3d cIdentity3d(void)
{
	cMatrix3d result;
	result.identity();
	return (result);
}


//===========================================================================
/*!
	Compute the multiplication between two matrices.

	\param    a_matrix1  First matrix.
	\param    a_matrix2  Second matrix.
	\return   Returns multiplication of /e matrix1 * /e matrix2.
*/
//===========================================================================
inline cMatrix3d cMul(const cMatrix3d& a_matrix1, const cMatrix3d& a_matrix2)
{
	cMatrix3d result;
	a_matrix1.mulr(a_matrix2, result);
	return (result);
}


//===========================================================================
/*!
	Compute the multiplication of a matrix and a vector.

	\param		a_matrix  Input matrix.
	\param		a_vector  Input vector.
	\return		Returns the multimplication of the matrix and vector.
*/
//===========================================================================
inline cVector3d cMul(const cMatrix3d& a_matrix, const cVector3d& a_vector)
{
	cVector3d result;
	a_matrix.mulr(a_vector, result);
	return(result);
}


//===========================================================================
/*!
	Compute the transpose of a matrix

	\param		a_matrix  Input matrix.
	\return		Returns the transpose of the input matrix.
*/
//===========================================================================
inline cMatrix3d cTrans(const cMatrix3d& a_matrix)
{
	cMatrix3d result;
	a_matrix.transr(result);
	return(result);
}


//===========================================================================
/*!
	Compute the inverse of a matrix.

	\param		a_matrix  Input matrix.
	\return		Returns the inverse of the input matrix.
*/
//===========================================================================
inline cMatrix3d cInv(const cMatrix3d& a_matrix)
{
	cMatrix3d result;
	a_matrix.invertr(result);
	return(result);
}


//===========================================================================
/*!
	Compute a rotation matrix given a rotation \e axis and an \e angle.

	\param		a_axis  Axis of rotation.
	\param		a_angleRad  Rotation angle in Radian.
	\return		Returns a rotation matrix.
*/
//===========================================================================
inline cMatrix3d cRotMatrix(const cVector3d& a_axis, const double& a_angleRad)
{
	cMatrix3d result;
	result.set(a_axis, a_angleRad);
	return (result);
}


//===========================================================================
/*!
	Compute the rotation of a matrix around an \e axis and an \e angle.

	\param		a_matrix  Input matrix.
	\param		a_axis  Axis of rotation.
	\param		a_angleRad  Rotation angle in Radian.
	\return		Returns input matrix after rotation.
*/
//===========================================================================
inline cMatrix3d cRotate(const cMatrix3d a_matrix, const cVector3d& a_axis,
						 const double& a_angleRad)
{
	cMatrix3d result;
	a_matrix.rotater(a_axis, a_angleRad, result);
	return (result);
}


//===========================================================================
/*!
	Compute the projection of a point on a plane. the plane is expressed
    by a set of three point.

	\param		a_point  Point to project on plane.
	\param		a_planePoint0  Point 0 on plane.
    \param		a_planePoint1  Point 1 on plane.
    \param		a_planePoint2  Point 2 on plane.
	\return		Returns the projection of \e a_point on plane.
*/
//===========================================================================
inline cVector3d cProjectPointOnPlane(const cVector3d& a_point,
  const cVector3d& a_planePoint0, const cVector3d& a_planePoint1,
  const cVector3d& a_planePoint2)
{
    // create two vectors from the three input points lying in the projection
    // plane.
    cVector3d v01, v02;
    a_planePoint1.subr(a_planePoint0, v01);
    a_planePoint2.subr(a_planePoint0, v02);

    // compute the normal vector of the plane
    cVector3d n;
    v01.crossr(v02, n);
    n.normalize();

    // compute a projection matrix
    cMatrix3d projectionMatrix;
    projectionMatrix.set( (n.y * n.y) + (n.z * n.z), -(n.x * n.y), -(n.x * n.z),
                         -(n.y * n.x), (n.x * n.x) + (n.z * n.z), -(n.y * n.z),
                         -(n.z * n.x), -(n.z * n.y), (n.x * n.x) + (n.y * n.y) );

    // project point on plane and return projected point.
    cVector3d point;
    a_point.subr(a_planePoint0, point);
    projectionMatrix.mul(point);
    point.add(a_planePoint0);

    // return result
    return (point);
}


//===========================================================================
/*!
	Compute the projection of a point on a line. the line is expressed
    by a point located on the line and a direction vector.

	\param		a_point  Point to project on the line.
	\param		a_pointOnLine  Point located on the line
    \param      a_directionOfLine  Vector expressing the direction of the line
	\return		Returns the projection of \e a_point on the line.
*/
//===========================================================================
inline cVector3d cProjectPointOnLine(const cVector3d& a_point,
        const cVector3d& a_pointOnLine, const cVector3d& a_directionOfLine)
{
    cVector3d point, result;

    double lengthDirSq = a_directionOfLine.lengthsq();
    a_point.subr(a_pointOnLine, point);

    a_directionOfLine.mulr( (point.dot(a_directionOfLine) / (lengthDirSq)),
                            result);

    result.add(a_pointOnLine);

    return (result);
}


//===========================================================================
/*!
	Project a vector V0 onto a second vector V1.

	\param		a_vector0  Vector 0.
	\param		a_vector1  Vector 1.
	\return		Returns the projection of \e a_point on the line.
*/
//===========================================================================
inline cVector3d cProject(const cVector3d& a_vector0, const cVector3d& a_vector1)
{
    cVector3d point, result;
    double lengthSq = a_vector1.lengthsq();
    a_vector1.mulr( (a_vector0.dot(a_vector1) / (lengthSq)), result);

    return (result);
}


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
