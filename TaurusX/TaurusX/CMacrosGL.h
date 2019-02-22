///////////////////////////////////////////////////////////////////////////////
//
//  (C) 2001-2013 Force Dimension
//  All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef CMacrosGLH
#define CMacrosGLH
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(WIN64)
#include "windows.h"
#endif
#include "CVector3d.h"
#include "CMatrix3d.h"
#ifdef MACOSX
#include "GLUT/glut.h"
#else
#include "GL/gl.h"
#endif
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \struct   cMatrixGL
      \brief    To express position or translation.
                On the OpenGL side 4x4 matrices are required to perform all
                geometrical transformations. cMatrixGL provides a structure
                which encapsulates all the necessary
                functionalities to generate 4x4 OpenGL transformation matrices
                by passing 3D position vectors and rotation matrices.
                DCSGL also provides OpenGL calls to push, multiply and pop
                matrices off the OpenGL stack. OpenGL Matrices are COLUMN major.
*/
//===========================================================================
struct cMatrixGL
{
  private:
	//! array of type \e double.
	double  m[4][4];

  public:
    //-----------------------------------------------------------------------
	/*!
		  Returns a pointer to the matrix array in memory.

	      \return   Returns pointer of type \e double.
	*/
    //-----------------------------------------------------------------------
	const double* pMatrix() const { return m[0]; }


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL translation matrix from a position vector passed as
          parameter.

          \param    a_pos   Input vector.
	*/
    //-----------------------------------------------------------------------
	inline void set(const cVector3d& a_pos)
	{
        m[0][0] = 1.0;      m[0][1] = 0.0;       m[0][2] = 0.0;       m[0][3] = 0.0;
        m[1][0] = 0.0;      m[1][1] = 1.0;       m[1][2] = 0.0;       m[1][3] = 0.0;
        m[2][0] = 0.0;      m[2][1] = 0.0;       m[2][2] = 1.0;       m[2][3] = 0.0;
        m[3][0] = a_pos.x;  m[3][1] = a_pos.y;   m[3][2] = a_pos.z;   m[3][3] = 1.0;
    }


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL rotation matrix from a rotation 3x3 Matrix passed as
          parameter.

	      \param    a_rot  Rotation Matrix.
	*/
    //-----------------------------------------------------------------------
	void set(const cMatrix3d& a_rot)
	{
        m[0][0] = a_rot.m[0][0];  m[0][1] = a_rot.m[1][0];  m[0][2] = a_rot.m[2][0];  m[0][3] = 0.0;
        m[1][0] = a_rot.m[0][1];  m[1][1] = a_rot.m[1][1];  m[1][2] = a_rot.m[2][1];  m[1][3] = 0.0;
        m[2][0] = a_rot.m[0][2];  m[2][1] = a_rot.m[1][2];  m[2][2] = a_rot.m[2][2];  m[2][3] = 0.0;
        m[3][0] = 0.0;            m[3][1] = 0.0;            m[3][2] = 0.0;            m[3][3] = 1.0;
	}


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL translation matrix from Vector giving translation.
		  Do not use directly.

	      \param    a_pos   Translation Vector.
	      \param    a_rot   Rotation Matrix.
	*/
    //-----------------------------------------------------------------------
	void set(const cVector3d& a_pos, const cMatrix3d& a_rot)
	{
        m[0][0] = a_rot.m[0][0];  m[0][1] = a_rot.m[1][0];  m[0][2] = a_rot.m[2][0];  m[0][3] = 0.0;
        m[1][0] = a_rot.m[0][1];  m[1][1] = a_rot.m[1][1];  m[1][2] = a_rot.m[2][1];  m[1][3] = 0.0;
        m[2][0] = a_rot.m[0][2];  m[2][1] = a_rot.m[1][2];  m[2][2] = a_rot.m[2][2];  m[2][3] = 0.0;
        m[3][0] = a_pos.x;        m[3][1] = a_pos.y;        m[3][2] = a_pos.z;        m[3][3] = 1.0;
	}


    //-----------------------------------------------------------------------
    /*!
        Copy current matrix to an external matrix passed as parameter.

        \param    a_destination  Destination matrix.
    */
    //-----------------------------------------------------------------------
    inline void copyto(cMatrixGL& a_destination) const
    {
        a_destination.m[0][0] = m[0][0];  a_destination.m[0][1] = m[0][1];
        a_destination.m[0][2] = m[0][2];  a_destination.m[0][3] = m[0][3];
        a_destination.m[1][0] = m[1][0];  a_destination.m[1][1] = m[1][1];
        a_destination.m[1][2] = m[1][2];  a_destination.m[1][3] = m[1][3];
        a_destination.m[2][0] = m[2][0];  a_destination.m[2][1] = m[2][1];
        a_destination.m[2][2] = m[2][2];  a_destination.m[2][3] = m[2][3];
        a_destination.m[3][0] = m[3][0];  a_destination.m[3][1] = m[3][1];
        a_destination.m[3][2] = m[3][2];  a_destination.m[3][3] = m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Copy values from an external matrix passed as parameter to current
        matrix.

        \param    a_source  Source matrix.
    */
    //-----------------------------------------------------------------------
    inline void copyfrom(const cMatrixGL& a_source)
    {
        m[0][0] = a_source.m[0][0];  m[0][1] = a_source.m[0][1];
        m[0][2] = a_source.m[0][2];  m[0][3] = a_source.m[0][3];
        m[1][0] = a_source.m[1][0];  m[1][1] = a_source.m[1][1];
        m[1][2] = a_source.m[1][2];  m[1][3] = a_source.m[1][3];
        m[2][0] = a_source.m[2][0];  m[2][1] = a_source.m[2][1];
        m[2][2] = a_source.m[2][2];  m[2][3] = a_source.m[2][3];
        m[3][0] = a_source.m[3][0];  m[3][1] = a_source.m[3][1];
        m[3][2] = a_source.m[3][2];  m[3][3] = a_source.m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Set the identity matrix.
    */
    //-----------------------------------------------------------------------
    inline void identity()
    {
        m[0][0] = 1.0;  m[0][1] = 0.0;  m[0][2] = 0.0;  m[0][3] = 0.0;
        m[1][0] = 0.0;  m[1][1] = 1.0;  m[1][2] = 0.0;  m[1][3] = 0.0;
        m[2][0] = 0.0;  m[2][1] = 0.0;  m[2][2] = 1.0;  m[2][3] = 0.0;
        m[3][0] = 0.0;  m[3][1] = 0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current matrix with an external matrix passed as parameter.
        Result is stored in current matrix.

        \param    a_matrix  Matrix with which multiplication is performed.
    */
    //-----------------------------------------------------------------------
    inline void mul(const cMatrixGL& a_matrix)
    {
        // compute multiplication between both matrices
        double m00 = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] +
                     m[0][2] * a_matrix.m[2][0] + m[0][3] * a_matrix.m[3][0];
        double m01 = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] +
                     m[0][2] * a_matrix.m[2][1] + m[0][3] * a_matrix.m[3][1];
        double m02 = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] +
                     m[0][2] * a_matrix.m[2][2] + m[0][3] * a_matrix.m[3][2];
        double m03 = m[0][0] * a_matrix.m[0][3] + m[0][1] * a_matrix.m[1][3] +
                     m[0][2] * a_matrix.m[2][3] + m[0][3] * a_matrix.m[3][3];

        double m10 = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] +
                     m[1][2] * a_matrix.m[2][0] + m[1][3] * a_matrix.m[3][0];
        double m11 = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] +
                     m[1][2] * a_matrix.m[2][1] + m[1][3] * a_matrix.m[3][1];
        double m12 = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] +
                     m[1][2] * a_matrix.m[2][2] + m[1][3] * a_matrix.m[3][2];
        double m13 = m[1][0] * a_matrix.m[0][3] + m[1][1] * a_matrix.m[1][3] +
                     m[1][2] * a_matrix.m[2][3] + m[1][3] * a_matrix.m[3][3];

        double m20 = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] +
                     m[2][2] * a_matrix.m[2][0] + m[2][3] * a_matrix.m[3][0];
        double m21 = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] +
                     m[2][2] * a_matrix.m[2][1] + m[2][3] * a_matrix.m[3][1];
        double m22 = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] +
                     m[2][2] * a_matrix.m[2][2] + m[2][3] * a_matrix.m[3][2];
        double m23 = m[2][0] * a_matrix.m[0][3] + m[2][1] * a_matrix.m[1][3] +
                     m[2][2] * a_matrix.m[2][3] + m[2][3] * a_matrix.m[3][3];

        double m30 = m[3][0] * a_matrix.m[0][0] + m[3][1] * a_matrix.m[1][0] +
                     m[3][2] * a_matrix.m[2][0] + m[3][3] * a_matrix.m[3][0];
        double m31 = m[3][0] * a_matrix.m[0][1] + m[3][1] * a_matrix.m[1][1] +
                     m[3][2] * a_matrix.m[2][1] + m[3][3] * a_matrix.m[3][1];
        double m32 = m[3][0] * a_matrix.m[0][2] + m[3][1] * a_matrix.m[1][2] +
                     m[3][2] * a_matrix.m[2][2] + m[3][3] * a_matrix.m[3][2];
        double m33 = m[3][0] * a_matrix.m[0][3] + m[3][1] * a_matrix.m[1][3] +
                     m[3][2] * a_matrix.m[2][3] + m[3][3] * a_matrix.m[3][3];

        // return values to current matrix
        m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;  m[0][3] = m03;
        m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;  m[1][3] = m13;
        m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;  m[2][3] = m23;
        m[3][0] = m30;  m[3][1] = m31;  m[3][2] = m32;  m[3][3] = m33;
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current matrix with an external matrix passed as parameter.
        Result is stored in result matrix.

        \param      a_matrix  Matrix with which multiplication is performed.
        \param      a_result  Here is where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void mulr(const cMatrixGL& a_matrix, cMatrixGL& a_result)
    {
        // compute multiplication between both matrices
        a_result.m[0][0] = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] +
                           m[0][2] * a_matrix.m[2][0] + m[0][3] * a_matrix.m[3][0];
        a_result.m[0][1] = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] +
                           m[0][2] * a_matrix.m[2][1] + m[0][3] * a_matrix.m[3][1];
        a_result.m[0][2] = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] +
                           m[0][2] * a_matrix.m[2][2] + m[0][3] * a_matrix.m[3][2];
        a_result.m[0][3] = m[0][0] * a_matrix.m[0][3] + m[0][1] * a_matrix.m[1][3] +
                           m[0][2] * a_matrix.m[2][3] + m[0][3] * a_matrix.m[3][3];

        a_result.m[1][0] = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] +
                           m[1][2] * a_matrix.m[2][0] + m[1][3] * a_matrix.m[3][0];
        a_result.m[1][1] = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] +
                           m[1][2] * a_matrix.m[2][1] + m[1][3] * a_matrix.m[3][1];
        a_result.m[1][2] = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] +
                           m[1][2] * a_matrix.m[2][2] + m[1][3] * a_matrix.m[3][2];
        a_result.m[1][3] = m[1][0] * a_matrix.m[0][3] + m[1][1] * a_matrix.m[1][3] +
                           m[1][2] * a_matrix.m[2][3] + m[1][3] * a_matrix.m[3][3];

        a_result.m[2][0] = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] +
                           m[2][2] * a_matrix.m[2][0] + m[2][3] * a_matrix.m[3][0];
        a_result.m[2][1] = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] +
                           m[2][2] * a_matrix.m[2][1] + m[2][3] * a_matrix.m[3][1];
        a_result.m[2][2] = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] +
                           m[2][2] * a_matrix.m[2][2] + m[2][3] * a_matrix.m[3][2];
        a_result.m[2][3] = m[2][0] * a_matrix.m[0][3] + m[2][1] * a_matrix.m[1][3] +
                           m[2][2] * a_matrix.m[2][3] + m[2][3] * a_matrix.m[3][3];

        a_result.m[3][0] = m[3][0] * a_matrix.m[0][0] + m[3][1] * a_matrix.m[1][0] +
                           m[3][2] * a_matrix.m[2][0] + m[3][3] * a_matrix.m[3][0];
        a_result.m[3][1] = m[3][0] * a_matrix.m[0][1] + m[3][1] * a_matrix.m[1][1] +
                           m[3][2] * a_matrix.m[2][1] + m[3][3] * a_matrix.m[3][1];
        a_result.m[3][2] = m[3][0] * a_matrix.m[0][2] + m[3][1] * a_matrix.m[1][2] +
                           m[3][2] * a_matrix.m[2][2] + m[3][3] * a_matrix.m[3][2];
        a_result.m[3][3] = m[3][0] * a_matrix.m[0][3] + m[3][1] * a_matrix.m[1][3] +
                           m[3][2] * a_matrix.m[2][3] + m[3][3] * a_matrix.m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Compute the transpose of the current matrix.
        Result is stored in current matrix.
    */
    //-----------------------------------------------------------------------
    inline void trans()
    {
        double t;

        t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t;
        t = m[0][2]; m[0][2] = m[2][0]; m[2][0] = t;
        t = m[0][3]; m[0][3] = m[3][0]; m[3][0] = t;
        t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
        t = m[1][3]; m[1][3] = m[3][1]; m[3][1] = t;
        t = m[2][3]; m[2][3] = m[3][2]; m[3][2] = t;
    }


    //-----------------------------------------------------------------------
    /*!
        Compute the transpose of the current matrix.
        Result is stored in result matrix.

        \param      a_result  Result is stored here.
    */
    //-----------------------------------------------------------------------
    inline void transr(cMatrixGL& a_result)
    {
        a_result.m[0][0] = m[0][0];
        a_result.m[0][1] = m[1][0];
        a_result.m[0][2] = m[2][0];
        a_result.m[0][3] = m[3][0];

        a_result.m[1][0] = m[0][1];
        a_result.m[1][1] = m[1][1];
        a_result.m[1][2] = m[2][1];
        a_result.m[1][3] = m[3][1];

        a_result.m[2][0] = m[0][2];
        a_result.m[2][1] = m[1][2];
        a_result.m[2][2] = m[2][2];
        a_result.m[2][3] = m[3][2];

        a_result.m[3][0] = m[0][3];
        a_result.m[3][1] = m[1][3];
        a_result.m[3][2] = m[2][3];
        a_result.m[3][3] = m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Push current OpenGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPush()
    {
        glPushMatrix();
    }


    //-----------------------------------------------------------------------
    /*!
        Load current OpenGL matrix with this cMatrixGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixLoad()
    {
        glLoadMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current OpenGL matrix with this cMatrixGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixMultiply()
    {
        glMultMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Push current OpenGL on the stack and multiply with this cMatrixGL
        matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPushMultiply()
    {
        glPushMatrix();
        glMultMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Pop current OpenGL matrix off the stack.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPop()
    {
        glPopMatrix();
    }
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
