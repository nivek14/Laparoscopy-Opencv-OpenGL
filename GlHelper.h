/*
 * COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
 *
 * 
 * For the purposes of this copyright and license, "Contributing Authors"
 * is defined as the following set of individuals:
 *
 *    Carlos Augusto Dietrich (cadietrich@gmail.com)
 *
 * This library is supplied "AS IS".  The Contributing Authors disclaim 
 * all warranties, expressed or implied, including, without limitation, 
 * the warranties of merchantability and of fitness for any purpose. 
 * The Contributing Authors assume no liability for direct, indirect, 
 * incidental, special, exemplary, or consequential damages, which may 
 * result from the use of the this library, even if advised of the 
 * possibility of such damage.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject
 * to the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented.
 *
 * 2. Altered versions must be plainly marked as such and must not be 
 *    misrepresented as being the original source.
 *
 * 3. This Copyright notice may not be removed or altered from any source 
 *    or altered source distribution.
 *
 * The Contributing Authors specifically permit, without fee, and 
 * encourage the use of this source code as a component in commercial 
 * products. If you use this source code in a product, acknowledgment 
 * is not required but would be appreciated.
 *
 * 
 * "Software is a process, it's never finished, it's always evolving. 
 * That's its nature. We know our software sucks. But it's shipping! 
 * Next time we'll do better, but even then it will be shitty. 
 * The only software that's perfect is one you're dreaming about. 
 * Real software crashes, loses data, is hard to learn and hard to use. 
 * But it's a process. We'll make it less shitty. Just watch!"
 */

#ifndef GL_HELPER_INCLUDED
#define GL_HELPER_INCLUDED

// DEPRECATED: (05-Nov-2015) 
#define GL_HELPER_MATRIX_ACCESS(M, i, j) (*(M + i + (j<<2)))

#include <boost/shared_ptr.hpp>

#include <VertexPool.h>
#include <MyMath.h>

/**
*/
template <typename T>
inline void glColor3T(T r, T g, T b);

template <>
inline void glColor3T(float r, float g, float b)
{
    glColor3f(r, g, b);
}

template <>
inline void glColor3T(double r, double g, double b)
{
    glColor3d(r, g, b);
}

/**
*/
template <typename T>
inline void glNormal3T(T x, T y, T z);

template <>
inline void glNormal3T(float x, float y, float z)
{
    glNormal3f(x, y, z);
}

template <>
inline void glNormal3T(double x, double y, double z)
{
    glNormal3d(x, y, z);
}

/**
*/
template <typename T>
inline void glTexCoord2T(T s, T t);

template <>
inline void glTexCoord2T(float s, float t)
{
    glTexCoord2f(s, t);
}

template <>
inline void glTexCoord2T(double s, double t)
{
    glTexCoord2d(s, t);
}

/**
*/
template <typename T>
inline void glTexCoord4T(T s, T t, T p, T q);

template <>
inline void glTexCoord4T(float s, float t, float p, float q)
{
    glTexCoord4f(s, t, p, q);
}

template <>
inline void glTexCoord4T(double s, double t, double p, double q)
{
    glTexCoord4d(s, t, p, q);
}

/**
*/
template <typename T>
inline void glVertex2T(T x, T y);

template <>
inline void glVertex2T(float x, float y)
{
    glVertex2f(x, y);
}

template <>
inline void glVertex2T(double x, double y)
{
    glVertex2d(x, y);
}

/**
*/
template <typename T>
inline void glVertex2Tv(const T* p);

template <>
inline void glVertex2Tv(const float* p)
{
    glVertex2fv(p);
}

template <>
inline void glVertex2Tv(const double* p)
{
    glVertex2dv(p);
}

/**
*/
template <typename T>
inline void glVertex3T(T x, T y, T z);

template <>
inline void glVertex3T(float x, float y, float z)
{
    glVertex3f(x, y, z);
}

template <>
inline void glVertex3T(double x, double y, double z)
{
    glVertex3d(x, y, z);
}

/**
*/
template <typename T>
inline void glVertex3Tv(const T *vertex);

template <>
inline void glVertex3Tv(const float *vertex)
{
    glVertex3fv(vertex);
}

template <>
inline void glVertex3Tv(const double *vertex)
{
    glVertex3dv(vertex);
}

/**
*/
template <typename T>
inline void glVertex4T(T x, T y, T z, T w);

template <>
inline void glVertex4T(float x, float y, float z, float w)
{
    glVertex4f(x, y, z, w);
}

template <>
inline void glVertex4T(double x, double y, double z, double w)
{
    glVertex4d(x, y, z, w);
}

/**
*/
template <typename T>
inline void glColorPointerT(GLint size, GLsizei stride, const T* pointer);

template <>
inline void glColorPointerT(GLint size, GLsizei stride, const float* pointer)
{
    glColorPointer(size, GL_FLOAT, stride, pointer);
}

template <>
inline void glColorPointerT(GLint size, GLsizei stride, const double* pointer)
{
    glColorPointer(size, GL_DOUBLE, stride, pointer);
}

/**
*/
template <typename T>
inline void glNormalPointerT(GLsizei stride, const T* pointer);

template <>
inline void glNormalPointerT(GLsizei stride, const float* pointer)
{
    glNormalPointer(GL_FLOAT, stride, pointer);
}

template <>
inline void glNormalPointerT(GLsizei stride, const double* pointer)
{
    glNormalPointer(GL_DOUBLE, stride, pointer);
}

/**
*/
template <typename T>
inline void glTexCoordPointerT(GLint size, GLsizei stride, const T* pointer);

template <>
inline void glTexCoordPointerT(GLint size, GLsizei stride, const float* pointer)
{
    glTexCoordPointer(size, GL_FLOAT, stride, pointer);
}

template <>
inline void glTexCoordPointerT(GLint size, GLsizei stride, const double* pointer)
{
    glTexCoordPointer(size, GL_DOUBLE, stride, pointer);
}

/**
*/
template <typename T>
inline void glVertexPointerT(GLint size, GLsizei stride, const T* pointer);

template <>
inline void glVertexPointerT(GLint size, GLsizei stride, const float* pointer)
{
    glVertexPointer(size, GL_FLOAT, stride, pointer);
}

template <>
inline void glVertexPointerT(GLint size, GLsizei stride, const double* pointer)
{
    glVertexPointer(size, GL_DOUBLE, stride, pointer);
}

/**
*/
template <typename T>
inline void glGetTv(GLenum pname, T *params);

template <>
inline void glGetTv(GLenum pname, float *params)
{
    glGetFloatv(pname, params);
}

template <>
inline void glGetTv(GLenum pname, double *params)
{
    glGetDoublev(pname, params);
}

/**
*/
template <typename T>
inline void glLoadMatrixT(const T *m);

template <>
inline void glLoadMatrixT(const float *m)
{
    glLoadMatrixf(m);
}

template <>
inline void glLoadMatrixT(const double *m)
{
    glLoadMatrixd(m);
}

namespace GlHelper
{
    /**
    */
	bool IsGlOk();

    /**
    */
    void ShowMessage(std::string message, int size, int x, int y);

    //void DrawLine(float *originCoord, float *destinationCoord, int widthMax);

    // XY PLANE
    void DrawCircle(boost::shared_ptr<CVertexPool> mesh, CVertexPool::ValueType x, CVertexPool::ValueType y, CVertexPool::ValueType radius, my::int32 segmentCount);

    // XY PLANE, TEXTURE COORDS [0, 1]
    void DrawDisk(boost::shared_ptr<CVertexPool> mesh, CVertexPool::ValueType x, CVertexPool::ValueType y, CVertexPool::ValueType radius, my::int32 segmentCount);

    /**
    */
    bool DrawSphere(CVertexPool::ValueType *center, CVertexPool::ValueType radius, int slices, int stacks);
    /**
    */
    bool DrawSphere(boost::shared_ptr<CVertexPool>& mesh, CVertexPool::ValueType *center, CVertexPool::ValueType radius, int slices, int stacks);

    /**
    */
    void DrawText(double screenX, double screenY, double sizeInPixels, double rotation, void* font, std::string characterArray);

    // GLUT_BITMAP_TIMES_ROMAN_10
    // GLUT_BITMAP_TIMES_ROMAN_24
    // GLUT_BITMAP_HELVETICA_10
    // GLUT_BITMAP_HELVETICA_12
    // GLUT_BITMAP_HELVETICA_18
    // TRICKY: (28-Oct-2020) THE OBJECT COORDINATES PRESENTED BY glRasterPos ARE TREATED JUST LIKE THOSE OF A glVertex COMMAND. THEY ARE TRANSFORMED BY THE CURRENT MODELVIEW AND PROJECTION MATRICES AND PASSED TO THE CLIPPING STAGE.
    void DrawString(void* font, GLdouble x, GLdouble y, GLdouble z, const std::string& string);

    /**
    */
    template <typename T>
	bool DrawCylinder(const T *originCoord, const T *destinationCoord, T bottomRadius, T topRadius, int slices);

    // DEPRECATED: (05-Nov-2015) 
    template <typename T>
    inline void TransformVertex4(const T *transformMatrix, const T *inputVertex, T *outputVertex)
    {
        outputVertex[0] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 3)*inputVertex[3];
        outputVertex[1] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 3)*inputVertex[3];
        outputVertex[2] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 3)*inputVertex[3];
        outputVertex[3] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 3)*inputVertex[3];
    }
    
    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool GetViewProjectionMatrix(const T *viewMatrix, const T *projectionMatrix, T *viewProjectionMatrix);

    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool GetViewProjectionMatrixInverse(const T *viewMatrix, const T *projectionMatrix, T *viewProjectionInverseMatrix);
    
    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool ProjectVertex(T objx, T objy, T objz, const T *viewMatrix, const T *projectionMatrix, const int *viewport, T *winx, T *winy, T *winz);

    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool ProjectVertex(const T *objectCoordinate, const T *viewMatrix, const T *projectionMatrix, const int *viewport, T *windowCoordinate);
    
    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool UnProjectVertex(T winx, T winy, T winz, const T *viewMatrix, const T *projectionMatrix, const int *viewport, T *objx, T *objy, T *objz);
    
    // DEPRECATED: (05-Nov-2015) 
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    template <typename T>
    bool UnProjectVertex(const T *windowCoordinate, const T *viewMatrix, const T *projectionMatrix, const int *viewport, T *objectCoordinate);
    
    // DEPRECATED: (05-Nov-2015) 
    template <typename T>
    bool UnProject(int i, int j, T& x, T& y, T& z);
    
    // DEPRECATED: (05-Nov-2015) 
    template <typename T>
    bool Translate(T *matrix, T x, T y, T z);
    
    // DEPRECATED: (05-Nov-2015) 
    template <typename T>
	bool LookAt(T eyeX, T eyeY, T eyeZ, T lookAtX, T lookAtY, T lookAtZ, T upX, T upY, T upZ, T *matrix);
    // DEPRECATED: (05-Nov-2015) 
    template <typename T>
	bool LookAt2(T eyeX, T eyeY, T eyeZ, T lookAtX, T lookAtY, T lookAtZ, T upX, T upY, T upZ, T *matrix);
    
    /**
    */
    bool Draw(const CVertexPool& mesh);
    
    /**
    */
    bool Draw(const boost::shared_ptr<CVertexPool>& mesh);

    /**
    */
    struct LIGHT_POINT {
        LIGHT_POINT()
        {
            Create();
        }

        LIGHT_POINT(const LIGHT_POINT& lightPoint)
        {
            Copy(lightPoint);
        }

        void operator=(const LIGHT_POINT& lightPoint)
        {
            Copy(lightPoint);
        }

        void Create()
        {
            // DEFAULT PARAMETERS FOR SEVERAL APPLICATIONS

            MyMath::Assign(0.0f, 0.0f, 300.0f, 0.0f, m_lightPosition);

            MyMath::Assign(0.3f, 0.3f, 0.3f, 1.0f, m_ambientLight);
            MyMath::Assign(0.7f, 0.7f, 0.7f, 1.0f, m_diffuseLight);

            m_specularContribution = 0.15f;
        }

        void Copy(const LIGHT_POINT& lightPoint)
        {
            // DEFAULT PARAMETERS FOR SEVERAL APPLICATIONS

            MyMath::Assign4(lightPoint.m_lightPosition, m_lightPosition);

            MyMath::Assign4(lightPoint.m_ambientLight, m_ambientLight);
            MyMath::Assign4(lightPoint.m_diffuseLight, m_diffuseLight);

            m_specularContribution = lightPoint.m_specularContribution;
        }

        float m_lightPosition[4];
        float m_ambientLight[4];
        float m_diffuseLight[4];
        float m_specularContribution;
    };

    /**
    */
    void EnableLight(const LIGHT_POINT& lightPoint = LIGHT_POINT());

    /**
    */
    void DisableLight();

    // (BEGIN OF) TESTING: (13-Jun-2016) 
    struct PUSH_MATRIX_HELPER {
        // PUSH TRANSFORM (PROJECTION AND MODELVIEW)
        PUSH_MATRIX_HELPER(bool loadIdentity = false);
        // POP TRANSFORM (PROJECTION AND MODELVIEW)
        ~PUSH_MATRIX_HELPER();
    };
    // (END OF) TESTING: (13-Jun-2016) 

    // (BEGIN OF) TESTING: (19-Jul-2016) 
    struct PUSH_COLOR_HELPER {
        // PUSH TRANSFORM (PROJECTION AND MODELVIEW)
        PUSH_COLOR_HELPER();
        // POP TRANSFORM (PROJECTION AND MODELVIEW)
        ~PUSH_COLOR_HELPER();

        float m_currentColor[4];
    };
    // (END OF) TESTING: (19-Jul-2016) 

    /**
    */
    my::CVector3<double> GetOpticalAxis();

    /**
    */
    void ExportFrontBufferAsPpm(std::string ppmFileName, int windowWidth, int windowHeight);
};

#endif // GL_HELPER_INCLUDED

