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

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>

#include <Logger.h>
#include <MyMath.h>
#include <FileHelper.h>

#include "PinholeCamera2.h"

#define GL_HELPER_MATRIX_ACCESS(M, i, j) (*(M + i + (j<<2)))

/**
 */
CPinholeCamera2::CPinholeCamera2()
{
    Create();
}

/**
 */
CPinholeCamera2::CPinholeCamera2(const CPinholeCamera2 &pinholeCamera)
{
    Copy(pinholeCamera);
}

/**
 */
CPinholeCamera2& CPinholeCamera2::operator=(const CPinholeCamera2 &pinholeCamera)
{
    Copy(pinholeCamera);
    
    return *this;
}

// ORTHOGONAL
bool CPinholeCamera2::Create(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight)
{
    m_projectionType = ORTHOGONAL_PROJECTION;
    
    m_cameraType = TRACKBALL_CAMERA;

    m_leftClippingPlane = leftClippingPlane;
    m_rightClippingPlane = rightClippingPlane;
    m_bottomClippingPlane = bottomClippingPlane;
    m_topClippingPlane = topClippingPlane;
    
    m_fieldOfView = 0;
    
    m_nearPlaneDepth = nearPlaneDepth;
    m_farPlaneDepth = farPlaneDepth;
    
    m_viewport[0] = 0;
    m_viewport[1] = 0;

    if (viewportWidth < 1)
    {
        LOG_ERROR();

        viewportWidth = 1;
    }

    m_viewport[2] = viewportWidth;

    if (viewportHeight < 1)
    {
        LOG_ERROR();

        viewportHeight = 1;
    }

    m_viewport[3] = viewportHeight;

    m_pitchRotation = 0;
    m_yawRotation = 0;
    m_rollRotation = 0;

    m_frontDisplacement = 0;
    m_sideDisplacement = 0;
    m_upDisplacement = 0;
    
    LoadIdentity4x4(m_viewMatrix);

    UpdateProjectionMatrix();
    
    // TODO: (08-May-2019) RE-EVALUATE
    //UpdateImagePlaneMatrix();
    
    return true;
}

// PERSPECTIVE
bool CPinholeCamera2::Create(double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight)
{
    m_projectionType = PERSPECTIVE_PROJECTION;
    
    m_cameraType = TRACKBALL_CAMERA;

    //m_leftClippingPlane = ?;
    //m_rightClippingPlane = ?;
    //m_bottomClippingPlane = ?;
    //m_topClippingPlane = ?;

    m_fieldOfView = fieldOfView;
    
    m_nearPlaneDepth = nearPlaneDepth;
    m_farPlaneDepth = farPlaneDepth;
    
    m_viewport[0] = 0;
    m_viewport[1] = 0;

    if (viewportWidth < 1)
    {
        LOG_ERROR();

        viewportWidth = 1;
    }

    m_viewport[2] = viewportWidth;

    if (viewportHeight < 1)
    {
        LOG_ERROR();

        viewportHeight = 1;
    }

    m_viewport[3] = viewportHeight;

    m_pitchRotation = 0;
    m_yawRotation = 0;
    m_rollRotation = 0;

    m_frontDisplacement = 0;
    m_sideDisplacement = 0;
    m_upDisplacement = 0;

    LoadIdentity4x4(m_viewMatrix);

    UpdateProjectionMatrix();
    
    // TODO: (08-May-2019) RE-EVALUATE
    //UpdateImagePlaneMatrix();
    
    return true;
}

bool CPinholeCamera2::Create(my::CVector3<double> opticalCenter, my::CVector3<double> referencePoint, my::CVector3<double> upVector, double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight)
{
    const double opticalCenterPointer[3] = { opticalCenter.x(), opticalCenter.y(), opticalCenter.z() },
        referencePointPointer[3] = { referencePoint.x(), referencePoint.y(), referencePoint.z() },
        upVectorPointer[3] = { upVector.x(), upVector.y(), upVector.z() };

    return Create(opticalCenterPointer, referencePointPointer, upVectorPointer, fieldOfView, nearPlaneDepth, farPlaneDepth, viewportWidth, viewportHeight);
}

bool CPinholeCamera2::Create(const double *opticalCenter, const double *referencePoint, const double *upVector, double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight)
{
    m_projectionType = PERSPECTIVE_PROJECTION;
    
    m_fieldOfView = fieldOfView;
    
    m_nearPlaneDepth = nearPlaneDepth;
    m_farPlaneDepth = farPlaneDepth;
    
    m_viewport[0] = 0;
    m_viewport[1] = 0;
    m_viewport[2] = viewportWidth;
    m_viewport[3] = viewportHeight;
    
    UpdateViewMatrix(opticalCenter, referencePoint, upVector);
    
    UpdateProjectionMatrix();
    
    //UpdateImagePlaneMatrix();
    
    return true;
}

/**
*/
CPinholeCamera2::PROJECTION_TYPE CPinholeCamera2::GetProjectionType() const
{
    return m_projectionType;
}

/**
*/
void CPinholeCamera2::SetCameraType(CAMERA_TYPE type)
{
    m_cameraType = type;
}

/**
*/
CPinholeCamera2::CAMERA_TYPE CPinholeCamera2::GetCameraType() const
{
    return m_cameraType;
}

/**
*/
CPinholeCamera2::TRACKBALL_MODE CPinholeCamera2::GetTrackballMode() const
{
    return m_trackballMode;
}

/**
*/
void CPinholeCamera2::SetTrackballMode(TRACKBALL_MODE trackballMode)
{
    m_trackballMode = trackballMode;
}

/**
 */
double CPinholeCamera2::GetLeftClippingPlane() const
{
    return m_leftClippingPlane;
}

/**
 */
bool CPinholeCamera2::SetLeftClippingPlane(double leftClippingPlane)
{
    if (!UpdateProjection(leftClippingPlane, m_rightClippingPlane, m_bottomClippingPlane, m_topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetRightClippingPlane() const
{
    return m_rightClippingPlane;
}

/**
 */
bool CPinholeCamera2::SetRightClippingPlane(double rightClippingPlane)
{
    if (!UpdateProjection(m_leftClippingPlane, rightClippingPlane, m_bottomClippingPlane, m_topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetBottomClippingPlane() const
{
    return m_bottomClippingPlane;
}

/**
 */
bool CPinholeCamera2::SetBottomClippingPlane(double bottomClippingPlane)
{
    if (!UpdateProjection(m_leftClippingPlane, m_rightClippingPlane, bottomClippingPlane, m_topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetTopClippingPlane() const
{
    return m_topClippingPlane;
}

/**
 */
bool CPinholeCamera2::SetTopClippingPlane(double topClippingPlane)
{
    if (!UpdateProjection(m_leftClippingPlane, m_rightClippingPlane, m_bottomClippingPlane, topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetFieldOfView() const
{
    return m_fieldOfView;
}

/**
 */
bool CPinholeCamera2::SetFieldOfView(double fieldOfView)
{
    // BUG: (??-???-????) LOWER THRESHOLDS MAKE CAMERA UNSTABLE
    if (fieldOfView < 1.0)
        fieldOfView = 1.0;
    else if (fieldOfView > 179.0)
        fieldOfView = 179.0;
    
    if (!UpdateProjection(fieldOfView, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
bool CPinholeCamera2::SetFieldOfView(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane)
{
    if (!UpdateProjection(leftClippingPlane, rightClippingPlane, bottomClippingPlane, topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetNearPlaneDepth() const
{
    return m_nearPlaneDepth;
}

/**
 */
bool CPinholeCamera2::SetNearPlaneDepth(double depth)
{
    if (!UpdateProjection(m_fieldOfView, depth, m_farPlaneDepth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

/**
 */
double CPinholeCamera2::GetFarPlaneDepth() const
{
    return m_farPlaneDepth;
}

/**
 */
bool CPinholeCamera2::SetFarPlaneDepth(double depth)
{
    if (!UpdateProjection(m_fieldOfView, m_nearPlaneDepth, depth))
    {
        LOG_ERROR();
        
        return false;
    }
    
    return true;
}

int CPinholeCamera2::GetViewportWidth() const
{
    return m_viewport[2] - m_viewport[0];
}

int CPinholeCamera2::GetViewportHeight() const
{
    return m_viewport[3] - m_viewport[1];
}

// "YES", AROUND X AXIS
void CPinholeCamera2::Pitch(double angle)
{
    m_pitchRotation += angle;
}

// "NO", AROUND Y AXIS
void CPinholeCamera2::Yaw(double angle)
{
    m_yawRotation += angle;
}

// AROUND Z AXIS
void CPinholeCamera2::Roll(double angle)
{
    m_rollRotation += angle;
}

// TRICKY: (??-???-????) ALONG Z AXIS, WORLD COORDINATES.
void CPinholeCamera2::MoveFront(double displacement)
{
    m_frontDisplacement += displacement;
}

// TRICKY: (??-???-????) ALONG X AXIS, WORLD COORDINATES.
void CPinholeCamera2::MoveSide(double displacement)
{
    if (m_projectionType == PERSPECTIVE_PROJECTION)
        m_sideDisplacement += displacement;
    //// TESTING: (??-???-????)
    //else if (m_projectionType == ORTHOGONAL_PROJECTION)
    //    m_sideDisplacement += displacement * ((m_rightClippingPlane - m_leftClippingPlane) * 0.05);
}

// TRICKY: (??-???-????) ALONG Y AXIS, WORLD COORDINATES.
void CPinholeCamera2::MoveUp(double displacement)
{
    if (m_projectionType == PERSPECTIVE_PROJECTION)
        m_upDisplacement += displacement;
    //// TESTING: (??-???-????)
    //else if (m_projectionType == ORTHOGONAL_PROJECTION)
    //    m_upDisplacement += displacement * ((m_topClippingPlane - m_bottomClippingPlane) * 0.05);
}

//// ORTHOGONAL/PERSPECTIVE
//void CPinholeCamera2::Zoom(double factor)
//{
//    // TODO: (08-May-2019) SET AN APPROPRIATE THRESHOLD
//    double step = 0.05 * factor;
//
//    if (m_projectionType == PERSPECTIVE_PROJECTION)
//        MoveFront(step);
//    else if (m_projectionType == ORTHOGONAL_PROJECTION)
//    {
//        // TODO: (08-May-2019) SET AN APPROPRIATE THRESHOLD
//        if ((m_rightClippingPlane - m_leftClippingPlane - 2.0 * step) > 0.01)
//        {
//            m_leftClippingPlane += step;
//            m_rightClippingPlane -= step;
//            m_bottomClippingPlane += step;
//            m_topClippingPlane -= step;
//        }
//
//        UpdateProjectionMatrix();
//    }
//}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
void glTranslated(T x, T y, T z, T *m)
{
    // (BEGIN OF) TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
    //m[0] += m[3] * x;
    //m[1] += m[3] * y;
    //m[2] += m[3] * z;
    //m[4] += m[7] * x;
    //m[5] += m[7] * y;
    //m[6] += m[7] * z;
    //m[8] += m[11] * x;
    //m[9] += m[11] * y;
    //m[10] += m[11] * z;
    //m[12] += m[15] * x;
    //m[13] += m[15] * y;
    //m[14] += m[15] * z;
    m[12] += x;
    m[13] += y;
    m[14] += z;
    // (END OF) TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
void glRotated(T angle, T x, T y, T z, T *m)
{
    T c = MyMath::Cosine(MyMath::DegreesToRadians(angle)),
        s = MyMath::Sine(MyMath::DegreesToRadians(angle)),
        c1 = 1.0 - c;

    T m0 = m[0],
        m4 = m[4],
        m8 = m[8],
        m12 = m[12],
        m1 = m[1],
        m5 = m[5],
        m9 = m[9],
        m13 = m[13],
        m2 = m[2],
        m6 = m[6],
        m10 = m[10],
        m14 = m[14];

    // ROTATION MATRIX

    T r0 = x * x * c1 + c,
        r1 = x * y * c1 + z * s,
        r2 = x * z * c1 - y * s,
        r4 = x * y * c1 - z * s,
        r5 = y * y * c1 + c,
        r6 = y * z * c1 + x * s,
        r8 = x * z * c1 + y * s,
        r9 = y * z * c1 - x * s,
        r10 = z * z * c1 + c;

    m[0] = r0 * m0 + r4 * m1 + r8 * m2;
    m[1] = r1 * m0 + r5 * m1 + r9 * m2;
    m[2] = r2 * m0 + r6 * m1 + r10* m2;
    m[4] = r0 * m4 + r4 * m5 + r8 * m6;
    m[5] = r1 * m4 + r5 * m5 + r9 * m6;
    m[6] = r2 * m4 + r6 * m5 + r10* m6;
    m[8] = r0 * m8 + r4 * m9 + r8 * m10;
    m[9] = r1 * m8 + r5 * m9 + r9 * m10;
    m[10] = r2 * m8 + r6 * m9 + r10* m10;
    m[12] = r0 * m12 + r4 * m13 + r8 * m14;
    m[13] = r1 * m12 + r5 * m13 + r9 * m14;
    m[14] = r2 * m12 + r6 * m13 + r10* m14;
}

// TESTING: (10-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
void glMultMatrix(T *m, const T* n)
{
    T m0 = m[0],
        m4 = m[4],
        m8 = m[8],
        m12 = m[12],
        m1 = m[1],
        m5 = m[5],
        m9 = m[9],
        m13 = m[13],
        m2 = m[2],
        m6 = m[6],
        m10 = m[10],
        // (BEGIN OF) TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
        //m14 = m[14],
        //m3 = m[3],
        //m7 = m[7],
        //m11 = m[11],
        //m15 = m[15];
        m14 = m[14];
        // (END OF) TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS

    m[0] = m0 * n[0] + m4 * n[1] + m8 * n[2] + m12 * n[3];
    m[1] = m1 * n[0] + m5 * n[1] + m9 * n[2] + m13 * n[3];
    m[2] = m2 * n[0] + m6 * n[1] + m10 * n[2] + m14 * n[3];
    // TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
    //m[3] = m3 * n[0] + m7 * n[1] + m11 * n[2] + m15 * n[3];
    m[4] = m0 * n[4] + m4 * n[5] + m8 * n[6] + m12 * n[7];
    m[5] = m1 * n[4] + m5 * n[5] + m9 * n[6] + m13 * n[7];
    m[6] = m2 * n[4] + m6 * n[5] + m10 * n[6] + m14 * n[7];
    // TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
    //m[7] = m3 * n[4] + m7 * n[5] + m11 * n[6] + m15 * n[7];
    m[8] = m0 * n[8] + m4 * n[9] + m8 * n[10] + m12 * n[11];
    m[9] = m1 * n[8] + m5 * n[9] + m9 * n[10] + m13 * n[11];
    m[10] = m2 * n[8] + m6 * n[9] + m10 * n[10] + m14 * n[11];
    // TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
    //m[11] = m3 * n[8] + m7 * n[9] + m11 * n[10] + m15 * n[11];
    m[12] = m0 * n[12] + m4 * n[13] + m8 * n[14] + m12 * n[15];
    m[13] = m1 * n[12] + m5 * n[13] + m9 * n[14] + m13 * n[15];
    m[14] = m2 * n[12] + m6 * n[13] + m10 * n[14] + m14 * n[15];
    // TESTING: (10-May-2019) OPTIMIZED FOR COMMON MODEL/VIEW OPERATIONS
    //m[15] = m3 * n[12] + m7 * n[13] + m11 * n[14] + m15 * n[15];
}

/**
 */
void CPinholeCamera2::ApplyTransform()
{
    double transformMatrix[16] = { 
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    double m0 = m_viewMatrix[0],
        m4 = m_viewMatrix[4],
        m8 = m_viewMatrix[8],
        m1 = m_viewMatrix[1],
        m5 = m_viewMatrix[5],
        m9 = m_viewMatrix[9],
        m2 = m_viewMatrix[2],
        m6 = m_viewMatrix[6],
        m10 = m_viewMatrix[10];

    double dX = m_sideDisplacement * m0 + m_upDisplacement * m1 + m_frontDisplacement * m2,
        dY = m_sideDisplacement * m4 + m_upDisplacement * m5 + m_frontDisplacement * m6,
        dZ = m_sideDisplacement * m8 + m_upDisplacement * m9 + m_frontDisplacement * m10;

    if (m_cameraType == TRACKBALL_CAMERA)
    {
        if (m_trackballMode == AROUND_Z)
            glRotated(m_rollRotation, 0.0, 0.0, 1.0, transformMatrix);
        else
            glRotated(m_rollRotation, m2, m6, m10, transformMatrix);

        if (m_trackballMode == AROUND_Y)
            glRotated(m_yawRotation, 0.0, 1.0, 0.0, transformMatrix);
        else
            glRotated(m_yawRotation, m1, m5, m9, transformMatrix);

        glRotated(m_pitchRotation, m0, m4, m8, transformMatrix);

        glTranslated(dX, dY, dZ, transformMatrix);
    }
    else if (m_cameraType == FLYBY_CAMERA)
    {
        double m12 = m_viewMatrix[12],
            m13 = m_viewMatrix[13],
            m14 = m_viewMatrix[14],
            eyeX = m0 * m12 + m1 * m13 + m2 * m14,
            eyeY = m4 * m12 + m5 * m13 + m6 * m14,
            eyeZ = m8 * m12 + m9 * m13 + m10 * m14;

        glTranslated(eyeX + dX, eyeY + dY, eyeZ + dZ, transformMatrix);

        glRotated(m_yawRotation, 0.0, 1.0, 0.0, transformMatrix);

        glRotated(m_pitchRotation, m0, m4, m8, transformMatrix);

        glTranslated(-eyeX, -eyeY, -eyeZ, transformMatrix);
    }

    glMultMatrix(m_viewMatrix, transformMatrix);

    m_pitchRotation = 0.0;
    m_yawRotation = 0.0;
    m_rollRotation = 0.0;
    
    m_frontDisplacement = 0.0;
    m_sideDisplacement = 0.0;
    m_upDisplacement = 0.0;
}

// OpenGL ADAPTOR
const double *CPinholeCamera2::GetViewMatrix() const
{
    return m_viewMatrix;
}

void CPinholeCamera2::SetViewMatrix(const double* viewMatrix, bool transpose)
{
    if (transpose)
    {
        m_viewMatrix[0] = viewMatrix[0];
        m_viewMatrix[1] = viewMatrix[4];
        m_viewMatrix[2] = viewMatrix[8];
        m_viewMatrix[3] = viewMatrix[12];
        m_viewMatrix[4] = viewMatrix[1];
        m_viewMatrix[5] = viewMatrix[5];
        m_viewMatrix[6] = viewMatrix[9];
        m_viewMatrix[7] = viewMatrix[13];
        m_viewMatrix[8] = viewMatrix[2];
        m_viewMatrix[9] = viewMatrix[6];
        m_viewMatrix[10] = viewMatrix[10];
        m_viewMatrix[11] = viewMatrix[14];
        m_viewMatrix[12] = viewMatrix[3];
        m_viewMatrix[13] = viewMatrix[7];
        m_viewMatrix[14] = viewMatrix[11];
        m_viewMatrix[15] = viewMatrix[15];
    }
    else
        memcpy(m_viewMatrix, viewMatrix, 16 * sizeof(double));
}

///**
// */
//template <>
//void CPinholeCamera2::SetViewMatrix(const float *viewMatrix)
//{
//    HEALTH_CHECK(!viewMatrix, /*false*/);
//    
//    MyMath::Assign16(viewMatrix, m_viewMatrix);
//}
//
///**
// */
//template <>
//void CPinholeCamera2::SetViewMatrix(const double *viewMatrix)
//{
//    HEALTH_CHECK(!viewMatrix, /*false*/);
//    
//    MyMath::Assign16(viewMatrix, m_viewMatrix);
//}

// OpenGL ADAPTOR
const double *CPinholeCamera2::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void CPinholeCamera2::SetProjectionMatrix(const double* projectionMatrix, bool transpose)
{
    if (transpose)
    {
        m_projectionMatrix[0] = projectionMatrix[0];
        m_projectionMatrix[1] = projectionMatrix[4];
        m_projectionMatrix[2] = projectionMatrix[8];
        m_projectionMatrix[3] = projectionMatrix[12];
        m_projectionMatrix[4] = projectionMatrix[1];
        m_projectionMatrix[5] = projectionMatrix[5];
        m_projectionMatrix[6] = projectionMatrix[9];
        m_projectionMatrix[7] = projectionMatrix[13];
        m_projectionMatrix[8] = projectionMatrix[2];
        m_projectionMatrix[9] = projectionMatrix[6];
        m_projectionMatrix[10] = projectionMatrix[10];
        m_projectionMatrix[11] = projectionMatrix[14];
        m_projectionMatrix[12] = projectionMatrix[3];
        m_projectionMatrix[13] = projectionMatrix[7];
        m_projectionMatrix[14] = projectionMatrix[11];
        m_projectionMatrix[15] = projectionMatrix[15];
    }
    else
        memcpy(m_projectionMatrix, projectionMatrix, 16 * sizeof(double));
}

///**
// */
//template <>
//void CPinholeCamera2::SetProjectionMatrix(const float *projectionMatrix)
//{
//    HEALTH_CHECK(!projectionMatrix, /*false*/);
//    
//    MyMath::Assign16(projectionMatrix, m_projectionMatrix);
//}
//
///**
// */
//template <>
//void CPinholeCamera2::SetProjectionMatrix(const double *projectionMatrix)
//{
//    HEALTH_CHECK(!projectionMatrix, /*false*/);
//    
//    MyMath::Assign16(projectionMatrix, m_projectionMatrix);
//}
//
///**
// */
//template <>
//void CPinholeCamera2::SetProjectionMatrix(my::CMatrix4<double>& projectionMatrix)
//{
//    m_projectionMatrix[ 0] = projectionMatrix(0, 0);
//    m_projectionMatrix[ 1] = projectionMatrix(0, 1);
//    m_projectionMatrix[ 2] = projectionMatrix(0, 2);
//    m_projectionMatrix[ 3] = projectionMatrix(0, 3);
//    m_projectionMatrix[ 4] = projectionMatrix(1, 0);
//    m_projectionMatrix[ 5] = projectionMatrix(1, 1);
//    m_projectionMatrix[ 6] = projectionMatrix(1, 2);
//    m_projectionMatrix[ 7] = projectionMatrix(1, 3);
//    m_projectionMatrix[ 8] = projectionMatrix(2, 0);
//    m_projectionMatrix[ 9] = projectionMatrix(2, 1);
//    m_projectionMatrix[10] = projectionMatrix(2, 2);
//    m_projectionMatrix[11] = projectionMatrix(2, 3);
//    m_projectionMatrix[12] = projectionMatrix(3, 0);
//    m_projectionMatrix[13] = projectionMatrix(3, 1);
//    m_projectionMatrix[14] = projectionMatrix(3, 2);
//    m_projectionMatrix[15] = projectionMatrix(3, 3);
//}

// TODO: (08-May-2019) RE-EVALUATE
///**
//*/
//const double *CPinholeCamera2::GetViewProjectionMatrix()
//{
//    UpdateViewProjectionMatrix();
//
//    return m_viewProjectionMatrix;
//}

///**
// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
// */
//const double *CPinholeCamera2::GetViewProjectionInverseMatrix()
//{
//    double cache[4][8] = {{0}};
//    double m0 = 0,
//    m1 = 0,
//    m2 = 0,
//    m3 = 0,
//    s = 0;
//    double *r0 = 0,
//    *r1 = 0,
//    *r2 = 0,
//    *r3 = 0;
//    
//    UpdateViewProjectionMatrix();
//    
//    r0 = cache[0];
//    r1 = cache[1];
//    r2 = cache[2];
//    r3 = cache[3];
//    
//    r0[0] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 0, 0);
//    r0[1] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 0, 1);
//    r0[2] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 0, 2);
//    r0[3] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 0, 3);
//    r0[4] = 1;
//    r0[5] = 0;
//    r0[6] = 0;
//    r0[7] = 0;
//    
//    r1[0] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 1, 0);
//    r1[1] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 1, 1);
//    r1[2] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 1, 2);
//    r1[3] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 1, 3);
//    r1[5] = 1;
//    r1[4] = 0;
//    r1[6] = 0;
//    r1[7] = 0;
//    
//    r2[0] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 2, 0);
//    r2[1] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 2, 1);
//    r2[2] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 2, 2);
//    r2[3] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 2, 3);
//    r2[6] = 1;
//    r2[4] = 0;
//    r2[5] = 0;
//    r2[7] = 0;
//    
//    r3[0] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 3, 0);
//    r3[1] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 3, 1);
//    r3[2] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 3, 2);
//    r3[3] = GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrix, 3, 3);
//    r3[7] = 1;
//    r3[4] = 0;
//    r3[5] = 0;
//    r3[6] = 0;
//    
//    // Choose pivot - or die.
//    if (fabs(r3[0]) > fabs(r2[0]))
//        std::swap(r3, r2);
//    if (fabs(r2[0]) > fabs(r1[0]))
//        std::swap(r2, r1);
//    if (fabs(r1[0]) > fabs(r0[0]))
//        std::swap(r1, r0);
//    
//    if (r0[0] == 0)
//    {
//        LOG_ERROR();
//        
//        return 0;
//    }
//    
//    // Eliminate first variable;
//    m1 = r1[0]/r0[0];
//    m2 = r2[0]/r0[0];
//    m3 = r3[0]/r0[0];
//    
//    s = r0[1];
//    
//    r1[1] -= m1*s;
//    r2[1] -= m2*s;
//    r3[1] -= m3*s;
//    
//    s = r0[2];
//    
//    r1[2] -= m1*s;
//    r2[2] -= m2*s;
//    r3[2] -= m3*s;
//    
//    s = r0[3];
//    
//    r1[3] -= m1*s;
//    r2[3] -= m2*s;
//    r3[3] -= m3*s;
//    
//    s = r0[4];
//    
//    if (s != 0)
//    {
//        r1[4] -= m1*s;
//        r2[4] -= m2*s;
//        r3[4] -= m3*s;
//    }
//    
//    s = r0[5];
//    
//    if (s != 0)
//    {
//        r1[5] -= m1*s;
//        r2[5] -= m2*s;
//        r3[5] -= m3*s;
//    }
//    
//    s = r0[6];
//    
//    if (s != 0)
//    {
//        r1[6] -= m1*s;
//        r2[6] -= m2*s;
//        r3[6] -= m3*s;
//    }
//    
//    s = r0[7];
//    
//    if (s != 0)
//    {
//        r1[7] -= m1*s;
//        r2[7] -= m2*s;
//        r3[7] -= m3*s;
//    }
//    
//    // Choose pivot - or die.
//    if (fabs(r3[1]) > fabs(r2[1]))
//        std::swap(r3, r2);
//    
//    if (fabs(r2[1]) > fabs(r1[1]))
//        std::swap(r2, r1);
//    
//    if (r1[1] == 0)
//    {
//        LOG_ERROR();
//        
//        return 0;
//    }
//    
//    // Eliminate second variable.
//    m2 = r2[1]/r1[1];
//    m3 = r3[1]/r1[1];
//    
//    r2[2] -= m2*r1[2];
//    r3[2] -= m3*r1[2];
//    r2[3] -= m2*r1[3];
//    r3[3] -= m3*r1[3];
//    
//    s = r1[4];
//    
//    if (s != 0)
//    {
//        r2[4] -= m2*s;
//        r3[4] -= m3*s;
//    }
//    
//    s = r1[5];
//    
//    if (s != 0)
//    {
//        r2[5] -= m2*s;
//        r3[5] -= m3*s;
//    }
//    
//    s = r1[6];
//    
//    if (s != 0)
//    {
//        r2[6] -= m2*s;
//        r3[6] -= m3*s;
//    }
//    
//    s = r1[7];
//    
//    if (s != 0)
//    {
//        r2[7] -= m2*s;
//        r3[7] -= m3*s;
//    }
//    
//    // Choose pivot - or die.
//    if (fabs(r3[2]) > fabs(r2[2]))
//        std::swap(r3, r2);
//    
//    if (r2[2] == 0)
//    {
//        LOG_ERROR();
//        
//        return 0;
//    }
//    
//    // Eliminate third variable.
//    m3 = r3[2]/r2[2];
//    
//    r3[3] -= m3*r2[3];
//    r3[4] -= m3*r2[4];
//    r3[5] -= m3*r2[5];
//    r3[6] -= m3*r2[6];
//    r3[7] -= m3*r2[7];
//    
//    // Last check
//    if (r3[3] == 0)
//    {
//        LOG_ERROR();
//        
//        return 0;
//    }
//    
//    // Now back substitute row 3
//    s = 1.0/r3[3];
//    
//    r3[4] *= s;
//    r3[5] *= s;
//    r3[6] *= s;
//    r3[7] *= s;
//    
//    // Now back substitute row 2
//    m2 = r2[3];
//    
//    s = 1.0/r2[2];
//    
//    r2[4] = s*(r2[4] - r3[4]*m2);
//    r2[5] = s*(r2[5] - r3[5]*m2);
//    r2[6] = s*(r2[6] - r3[6]*m2);
//    r2[7] = s*(r2[7] - r3[7]*m2);
//    
//    m1 = r1[3];
//    
//    r1[4] -= r3[4]*m1;
//    r1[5] -= r3[5]*m1;
//    r1[6] -= r3[6]*m1;
//    r1[7] -= r3[7]*m1;
//    
//    m0 = r0[3];
//    
//    r0[4] -= r3[4]*m0;
//    r0[5] -= r3[5]*m0;
//    r0[6] -= r3[6]*m0;
//    r0[7] -= r3[7]*m0;
//    
//    // Nnow back substitute row 1
//    m1 = r1[2];
//    
//    s = 1.0/r1[1];
//    
//    r1[4] = s*(r1[4] - r2[4]*m1);
//    r1[5] = s*(r1[5] - r2[5]*m1);
//    r1[6] = s*(r1[6] - r2[6]*m1);
//    r1[7] = s*(r1[7] - r2[7]*m1);
//    
//    m0 = r0[2];
//    
//    r0[4] -= r2[4]*m0;
//    r0[5] -= r2[5]*m0;
//    r0[6] -= r2[6]*m0;
//    r0[7] -= r2[7]*m0;
//    
//    // Now back substitute row 0.
//    m0 = r0[1];
//    
//    s = 1.0/r0[0];
//    
//    r0[4] = s*(r0[4] - r1[4]*m0);
//    r0[5] = s*(r0[5] - r1[5]*m0);
//    r0[6] = s*(r0[6] - r1[6]*m0);
//    r0[7] = s*(r0[7] - r1[7]*m0);
//    
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 0, 0) = r0[4];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 0, 1) = r0[5];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 0, 2) = r0[6];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 0, 3) = r0[7];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 1, 0) = r1[4];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 1, 1) = r1[5];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 1, 2) = r1[6];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 1, 3) = r1[7];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 2, 0) = r2[4];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 2, 1) = r2[5];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 2, 2) = r2[6];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 2, 3) = r2[7];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 3, 0) = r3[4];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 3, 1) = r3[5];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 3, 2) = r3[6];
//    GL_HELPER_MATRIX_ACCESS(m_viewProjectionMatrixInverse, 3, 3) = r3[7];
//    
//    return m_viewProjectionMatrixInverse;
//}

/**
 */
const int *CPinholeCamera2::GetViewport() const
{
    return m_viewport;
}

///**
// */
//void CPinholeCamera2::SetViewport(const int *viewport)
//{
//    HEALTH_CHECK(!viewport, /*false*/);
//    
//    m_viewport[0] = viewport[0];
//    m_viewport[1] = viewport[1];
//    m_viewport[2] = viewport[2];
//    m_viewport[3] = viewport[3];
//    
//    UpdateProjectionMatrix();
//}

/**
 */
void CPinholeCamera2::SetViewport(int x, int y, int width, int height)
{
    m_viewport[0] = x;
    m_viewport[1] = y;

    if (width < 1)
    {
        LOG_ERROR();

        width = 1;
    }

    m_viewport[2] = width;

    if (height < 1)
    {
        LOG_ERROR();

        height = 1;
    }

    m_viewport[3] = height;
    
    UpdateProjectionMatrix();
}

// TODO: (08-May-2019) RE-EVALUATE
///**
// */
//const double *CPinholeCamera2::GetImagePlaneMatrix()
//{
//    if (m_projectionType == PERSPECTIVE_PROJECTION)
//    {
//        UpdateImagePlaneMatrix();
//        
//        return m_imagePlaneMatrix;
//    }
//    
//    return 0;
//}

// http://3dengine.org/Right-up-back_from_modelview
my::CVector3<double> CPinholeCamera2::GetOpticalCenter() const
{
    my::CVector3<double> opticalCenter;

    if (m_projectionType == PERSPECTIVE_PROJECTION)
    {
        opticalCenter.Set(-(m_viewMatrix[0] * m_viewMatrix[12] + m_viewMatrix[1] * m_viewMatrix[13] + m_viewMatrix[2] * m_viewMatrix[14]),
            -(m_viewMatrix[4] * m_viewMatrix[12] + m_viewMatrix[5] * m_viewMatrix[13] + m_viewMatrix[6] * m_viewMatrix[14]),
            -(m_viewMatrix[8] * m_viewMatrix[12] + m_viewMatrix[9] * m_viewMatrix[13] + m_viewMatrix[10] * m_viewMatrix[14]));
    }
    
    return opticalCenter;
}

// http://3dengine.org/Right-up-back_from_modelview
my::CVector3<double> CPinholeCamera2::GetOpticalAxis() const
{
    my::CVector3<double> opticalAxis;

    if (m_projectionType == PERSPECTIVE_PROJECTION)
    {
        opticalAxis.Set(-(m_viewMatrix[2]),
            -(m_viewMatrix[6]),
            -(m_viewMatrix[10]));
    }

    return opticalAxis;
}

// http://3dengine.org/Right-up-back_from_modelview
my::CVector3<double> CPinholeCamera2::GetUpAxis() const
{
    my::CVector3<double> upAxis;

    if (m_projectionType == PERSPECTIVE_PROJECTION)
    {
        upAxis.Set(m_viewMatrix[1],
            m_viewMatrix[5],
            m_viewMatrix[9]);
    }
    
    return upAxis;
}

// TODO: (08-May-2019) RE-EVALUATE
///**
// */
//double CPinholeCamera2::GetAspect() const
//{
//    return (double)(m_viewport[2] - m_viewport[0]) / (double)(m_viewport[3] - m_viewport[1]);
//}

bool CPinholeCamera2::ToFile(std::string cameraFileName) const
{
    my::CVector3<double> opticalCenter = GetOpticalCenter(),
        opticalAxis = GetOpticalAxis(),
        up = GetUpAxis();

    opticalAxis.Normalize();
    up.Normalize();

    double aspectRatio = (double)(m_viewport[2] - m_viewport[0]) / (m_viewport[3] - m_viewport[1]);

    rapidjson::StringBuffer jsonString;
    rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonString);

    jsonWriter.StartObject();

    // "viewport"

    jsonWriter.String("viewport");

    jsonWriter.StartArray();

    jsonWriter.Int(m_viewport[0]);
    jsonWriter.Int(m_viewport[1]);
    jsonWriter.Int(m_viewport[2]);
    jsonWriter.Int(m_viewport[3]);

    jsonWriter.EndArray();

    // "optical_center"

    jsonWriter.String("optical_center");

    jsonWriter.StartArray();

    jsonWriter.Double(opticalCenter.x());
    jsonWriter.Double(opticalCenter.y());
    jsonWriter.Double(opticalCenter.z());

    jsonWriter.EndArray();

    // "optical_axis"

    jsonWriter.String("optical_axis");

    jsonWriter.StartArray();

    jsonWriter.Double(opticalAxis.x());
    jsonWriter.Double(opticalAxis.y());
    jsonWriter.Double(opticalAxis.z());

    jsonWriter.EndArray();

    // "up_vector"

    jsonWriter.String("up_vector");

    jsonWriter.StartArray();

    jsonWriter.Double(up.x());
    jsonWriter.Double(up.y());
    jsonWriter.Double(up.z());

    jsonWriter.EndArray();

    // "field_of_view"

    jsonWriter.String("field_of_view");
    jsonWriter.Double(m_fieldOfView);

    // "near_clipping_plane"

    jsonWriter.String("near_clipping_plane");
    jsonWriter.Double(m_nearPlaneDepth);

    // "far_clipping_plane"

    jsonWriter.String("far_clipping_plane");
    jsonWriter.Double(m_farPlaneDepth);

    // "aspect_ratio"

    jsonWriter.String("aspect_ratio");
    jsonWriter.Double(aspectRatio);

    jsonWriter.EndObject();

    jsonString.Flush();

    std::ofstream jsonFileStream(cameraFileName);

    HEALTH_CHECK(!jsonFileStream.is_open(), false);

    jsonFileStream << jsonString.GetString();

    jsonFileStream.close();

    return true;
}

bool CPinholeCamera2::FromFile(std::string cameraFileName)
{
    std::string jsonString;

    my::file::GetFileAsString(cameraFileName, jsonString);

    HEALTH_CHECK(jsonString.empty(), false);

    rapidjson::Document document;

    HEALTH_CHECK(document.Parse<0>(jsonString.c_str()).HasParseError(), false);

    HEALTH_CHECK(!document.IsObject(), false);

    my::int32 viewport[4] = { 0 };

    double opticalCenter[3] = { 0 },
        opticalAxis[3] = { 0 },
        upVector[3] = { 0 };

    double fieldOfView = my::Null<double>(),
        nearClippingPlane = my::Null<double>(),
        farClippingPlane = my::Null<double>(),
        aspectRatio = my::Null<double>();

    // viewport

    HEALTH_CHECK(!document.HasMember("viewport"), false);

    const rapidjson::Value& viewportParameterArrayHandle = document["viewport"];

    HEALTH_CHECK(!viewportParameterArrayHandle.IsArray(), false);
    HEALTH_CHECK(viewportParameterArrayHandle.Size() != 4, false);

    for (rapidjson::SizeType viewportParameterIndex = 0; viewportParameterIndex < viewportParameterArrayHandle.Size(); ++viewportParameterIndex)
    {
        const rapidjson::Value& viewportParameterHandle = viewportParameterArrayHandle[viewportParameterIndex];

        HEALTH_CHECK(!viewportParameterHandle.IsInt(), false);

        viewport[viewportParameterIndex] = viewportParameterHandle.GetInt();
    }

    // optical_center

    HEALTH_CHECK(!document.HasMember("optical_center"), false);

    const rapidjson::Value& opticalCenterHandle = document["optical_center"];

    HEALTH_CHECK(!opticalCenterHandle.IsArray(), false);
    HEALTH_CHECK(opticalCenterHandle.Size() != 3, false);

    for (rapidjson::SizeType opticalCenterParameterIndex = 0; opticalCenterParameterIndex < opticalCenterHandle.Size(); ++opticalCenterParameterIndex)
    {
        const rapidjson::Value& opticalCenterParameterHandle = opticalCenterHandle[opticalCenterParameterIndex];

        HEALTH_CHECK(!opticalCenterParameterHandle.IsNumber(), false);

        opticalCenter[opticalCenterParameterIndex] = opticalCenterParameterHandle.GetDouble();
    }

    // optical_axis

    HEALTH_CHECK(!document.HasMember("optical_axis"), false);

    const rapidjson::Value& opticalAxisHandle = document["optical_axis"];

    HEALTH_CHECK(!opticalAxisHandle.IsArray(), false);
    HEALTH_CHECK(opticalAxisHandle.Size() != 3, false);

    for (rapidjson::SizeType opticalAxisParameterIndex = 0; opticalAxisParameterIndex < opticalAxisHandle.Size(); ++opticalAxisParameterIndex)
    {
        const rapidjson::Value& opticalAxisParameterHandle = opticalAxisHandle[opticalAxisParameterIndex];

        HEALTH_CHECK(!opticalAxisParameterHandle.IsNumber(), false);

        opticalAxis[opticalAxisParameterIndex] = opticalAxisParameterHandle.GetDouble();
    }

    // up

    HEALTH_CHECK(!document.HasMember("up_vector"), false);

    const rapidjson::Value& upVectorHandle = document["up_vector"];

    HEALTH_CHECK(!upVectorHandle.IsArray(), false);
    HEALTH_CHECK(upVectorHandle.Size() != 3, false);

    for (rapidjson::SizeType upVectorParameterIndex = 0; upVectorParameterIndex < upVectorHandle.Size(); ++upVectorParameterIndex)
    {
        const rapidjson::Value& upVectorParameterHandle = upVectorHandle[upVectorParameterIndex];

        HEALTH_CHECK(!upVectorParameterHandle.IsNumber(), false);

        upVector[upVectorParameterIndex] = upVectorParameterHandle.GetDouble();
    }

    // field_of_view

    HEALTH_CHECK(!document.HasMember("field_of_view"), false);

    const rapidjson::Value& fieldOfViewHandle = document["field_of_view"];

    HEALTH_CHECK(!fieldOfViewHandle.IsNumber(), false);

    fieldOfView = fieldOfViewHandle.GetDouble();

    // near_clipping_plane

    HEALTH_CHECK(!document.HasMember("near_clipping_plane"), false);

    const rapidjson::Value& nearClippingPlaneHandle = document["near_clipping_plane"];

    HEALTH_CHECK(!nearClippingPlaneHandle.IsNumber(), false);

    nearClippingPlane = nearClippingPlaneHandle.GetDouble();

    // far_clipping_plane

    HEALTH_CHECK(!document.HasMember("far_clipping_plane"), false);

    const rapidjson::Value& farClippingPlaneHandle = document["far_clipping_plane"];

    HEALTH_CHECK(!farClippingPlaneHandle.IsNumber(), false);

    farClippingPlane = farClippingPlaneHandle.GetDouble();

    // aspect_ratio

    HEALTH_CHECK(!document.HasMember("aspect_ratio"), false);

    const rapidjson::Value& aspectRatioHandle = document["aspect_ratio"];

    HEALTH_CHECK(!aspectRatioHandle.IsNumber(), false);

    aspectRatio = aspectRatioHandle.GetDouble();

    // ADAPTOR FOR gluLookAt
    double center[3] = { opticalCenter[0] + opticalAxis[0], opticalCenter[1] + opticalAxis[1], opticalCenter[2] + opticalAxis[2] };

    if (!Create(opticalCenter, center, upVector, fieldOfView, nearClippingPlane, farClippingPlane, viewport[2] - viewport[0], viewport[3] - viewport[1]))
    {
        LOG_ERROR();

        return false;
    }

    return true;
}

// TODO: (08-May-2019) RE-EVALUATE
///**
// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
// */
//bool CPinholeCamera2::ProjectVertex(const double *worldCoord, double *screenCoord) const
//{
//    double cache[8] = {0};
//    
//    // Modelview transform
//    cache[0] = m_viewMatrix[ 0]*worldCoord[0] + m_viewMatrix[ 4]*worldCoord[1] + m_viewMatrix[ 8]*worldCoord[2] + m_viewMatrix[12];
//    cache[1] = m_viewMatrix[ 1]*worldCoord[0] + m_viewMatrix[ 5]*worldCoord[1] + m_viewMatrix[ 9]*worldCoord[2] + m_viewMatrix[13];
//    cache[2] = m_viewMatrix[ 2]*worldCoord[0] + m_viewMatrix[ 6]*worldCoord[1] + m_viewMatrix[10]*worldCoord[2] + m_viewMatrix[14];
//    cache[3] = m_viewMatrix[ 3]*worldCoord[0] + m_viewMatrix[ 7]*worldCoord[1] + m_viewMatrix[11]*worldCoord[2] + m_viewMatrix[15];
//    
//    // Projection transform, the final row of projection matrix is
//    // always [0 0 -1 0] so we optimize for that.
//    cache[4] = m_projectionMatrix[ 0]*cache[0] + m_projectionMatrix[ 4]*cache[1] + m_projectionMatrix[ 8]*cache[2] + m_projectionMatrix[12]*cache[3];
//    cache[5] = m_projectionMatrix[ 1]*cache[0] + m_projectionMatrix[ 5]*cache[1] + m_projectionMatrix[ 9]*cache[2] + m_projectionMatrix[13]*cache[3];
//    cache[6] = m_projectionMatrix[ 2]*cache[0] + m_projectionMatrix[ 6]*cache[1] + m_projectionMatrix[10]*cache[2] + m_projectionMatrix[14]*cache[3];
//    cache[7] = -cache[2];
//    
//    // The result normalizes between -1 and 1
//    if (cache[7] == 0)
//        return false;
//    
//    cache[7] = 1/cache[7];
//    
//    // Perspective division
//    cache[4] *= cache[7];
//    cache[5] *= cache[7];
//    cache[6] *= cache[7];
//    
//    // Window coordinates
//    // Map x, y to range 0-1
//    screenCoord[0] = (cache[4]*0.5 + 0.5)*m_viewport[2] + m_viewport[0];
//    screenCoord[1] = (cache[5]*0.5 + 0.5)*m_viewport[3] + m_viewport[1];
//    // This is only correct when glDepthRange(0, 1)
//    screenCoord[2] = (1 + cache[6])*0.5;
//    
//    return true;
//}

// TRICKY: (07-Jun-2019) THE ORIGIN OF THE CAMERA IS AT THE LEFT-BOTTOM CORNER!
// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
my::CVector3<double> CPinholeCamera2::ProjectVertex(my::CVector3<double> worldCoord) const
{
    double cache[7] = { 0 };
    
    cache[0] = m_viewMatrix[0] * worldCoord.x() + m_viewMatrix[4] * worldCoord.y() + m_viewMatrix[8] * worldCoord.z() + m_viewMatrix[12];
    cache[1] = m_viewMatrix[1] * worldCoord.x() + m_viewMatrix[5] * worldCoord.y() + m_viewMatrix[9] * worldCoord.z() + m_viewMatrix[13];
    cache[2] = m_viewMatrix[2] * worldCoord.x() + m_viewMatrix[6] * worldCoord.y() + m_viewMatrix[10] * worldCoord.z() + m_viewMatrix[14];
    cache[3] = m_viewMatrix[3] * worldCoord.x() + m_viewMatrix[7] * worldCoord.y() + m_viewMatrix[11] * worldCoord.z() + m_viewMatrix[15];
    
    cache[4] = m_projectionMatrix[0] * cache[0] + m_projectionMatrix[4] * cache[1] + m_projectionMatrix[8] * cache[2] + m_projectionMatrix[12] * cache[3];
    cache[5] = m_projectionMatrix[1] * cache[0] + m_projectionMatrix[5] * cache[1] + m_projectionMatrix[9] * cache[2] + m_projectionMatrix[13] * cache[3];
    cache[6] = m_projectionMatrix[2] * cache[0] + m_projectionMatrix[6] * cache[1] + m_projectionMatrix[10] * cache[2] + m_projectionMatrix[14] * cache[3];
    
    my::CVector3<double> emptyObject(my::Null<double>(), my::Null<double>(), my::Null<double>());
    
    if (cache[2] == 0.0)
        return emptyObject;
    
    cache[2] = -1.0 / cache[2];
    
    cache[4] *= cache[2];
    cache[5] *= cache[2];
    cache[6] *= cache[2];
    
    my::CVector3<double> screenCoord(my::Null<double>(), my::Null<double>(), my::Null<double>());
    
    // [-1, 1] -> [0, WIDTH/HEIGHT]
    screenCoord.x() = (cache[4] * 0.5 + 0.5) * m_viewport[2] + m_viewport[0];
    screenCoord.y() = (cache[5] * 0.5 + 0.5) * m_viewport[3] + m_viewport[1];
    // [-1, 1] -> [0, 1]
    screenCoord.z() = (1.0 + cache[6]) * 0.5;
    
    return screenCoord;
}

// DEPRECATED: (05-Nov-2015) 
template <typename T>
inline void TransformVertex4(const T *transformMatrix, const T *inputVertex, T *outputVertex)
{
    outputVertex[0] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 0, 3)*inputVertex[3];
    outputVertex[1] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 1, 3)*inputVertex[3];
    outputVertex[2] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 2, 3)*inputVertex[3];
    outputVertex[3] = GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 0)*inputVertex[0] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 1)*inputVertex[1] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 2)*inputVertex[2] + GL_HELPER_MATRIX_ACCESS(transformMatrix, 3, 3)*inputVertex[3];
}

/**
 */
bool CPinholeCamera2::UnProjectVertex(const double *screenCoord, double* worldCoord)
{
    double viewProjectionMatrixInverse[16] = { 0 };
    
    if (!ComputeViewProjectionInverseMatrix(viewProjectionMatrixInverse))
    {
        LOG_ERROR();

        return false;
    }

    HEALTH_CHECK(!viewProjectionMatrixInverse, false);

    double inputVertex[4] = { 0 },
        outputVertex[4] = { 0 };

    inputVertex[0] = screenCoord[0];
    inputVertex[1] = screenCoord[1];
    inputVertex[2] = 2.0 * screenCoord[2] - 1.0;
    inputVertex[3] = 1.0;
    
    TransformVertex4(viewProjectionMatrixInverse, inputVertex, outputVertex);
    
    if (outputVertex[3] == 0)
    {
        LOG_ERROR();
        
        return false;
    }
    
    outputVertex[3] = 1.0 / outputVertex[3];
    
    worldCoord[0] = outputVertex[0] * outputVertex[3];
    worldCoord[1] = outputVertex[1] * outputVertex[3];
    worldCoord[2] = outputVertex[2] * outputVertex[3];
    
    return true;
}

///**
//*/
//my::CVector3<double> CPinholeCamera2::UnProjectVertex(my::CVector3<double> screenCoord)
//{
//    double screenCoordPointer[3] = { screenCoord.x(), screenCoord.y(), screenCoord.z() },
//        worldCoordPointer[3] = { 0 };
//
//    if (!UnProjectVertex(screenCoordPointer, worldCoordPointer))
//    {
//        LOG_ERROR();
//
//        return my::CVector3<double>();
//    }
//
//    return my::CVector3<double>(worldCoordPointer[0], worldCoordPointer[1], worldCoordPointer[2]);
//}

// TODO: (08-May-2019) RE-EVALUATE
//// DEBUG ONLY! (25-Sep-2015)
//bool CPinholeCamera2::SetViewProjectionMatrix(const my::CViewProjectionMatrix<double>& viewProjectionMatrix)
//{
//    HEALTH_CHECK(my::IsNull(viewProjectionMatrix.GetFieldOfView()), false);
//
//    SetFieldOfView(viewProjectionMatrix.GetFieldOfView());
//
//    SetNearPlaneDepth(viewProjectionMatrix.GetNearClippingPlane());
//    SetFarPlaneDepth(viewProjectionMatrix.GetFarClippingPlane());
//
//    my::CVector4<my::int32> viewport = viewProjectionMatrix.GetViewport();
//
//    int viewportPointer[4] = { viewport.x(), viewport.y(), viewport.z(), viewport.w() };
//
//    SetViewport(viewportPointer);
//
//    my::CMatrix4<double> viewMatrix = viewProjectionMatrix.GetViewMatrix();
//
//    SetViewMatrix(viewMatrix.Get());
//
//    my::CMatrix4<double> projectionMatrix = viewProjectionMatrix.GetProjectionMatrix();
//
//    SetProjectionMatrix(projectionMatrix.Get());
//
//    UpdateImagePlaneMatrix();
//
//    UpdateViewProjectionMatrix();
//
//    return true;
//}

// TODO: (08-May-2019) RE-EVALUATE
///**
// */
//std::string CPinholeCamera2::ToJson() const
//{
//    std::string jsonString;
//    
//    jsonString += "{";
//    
//    jsonString += JsonHelper::NumberToJson("field_of_view", &CPinholeCamera2::GetFieldOfView, (*this)) + ",";
//    jsonString += JsonHelper::NumberToJson("near_plane_depth", &CPinholeCamera2::GetNearPlaneDepth, (*this)) + ",";
//    jsonString += JsonHelper::NumberToJson("far_plane_depth", &CPinholeCamera2::GetFarPlaneDepth, (*this)) + ",";
//    jsonString += JsonHelper::ArrayToJson("viewport", &CPinholeCamera2::GetViewport, (*this), 4) + ",";
//    jsonString += JsonHelper::ArrayToJson("view_matrix", &CPinholeCamera2::GetViewMatrix, (*this), 16) + ",";
//    jsonString += JsonHelper::ArrayToJson("projection_matrix", &CPinholeCamera2::GetProjectionMatrix, (*this), 16) + ",";
//    
//    if (jsonString.back() == ',')
//        jsonString.pop_back();
//    
//    jsonString += "}";
//    
//    return jsonString;
//}

// TODO: (08-May-2019) RE-EVALUATE
///**
// */
//void CPinholeCamera2::LoadIdentity3x3(double *matrix) const
//{
//    *matrix++ = 1;
//    *matrix++ = 0;
//    *matrix++ = 0;
//    
//    *matrix++ = 0;
//    *matrix++ = 1;
//    *matrix++ = 0;
//    
//    *matrix++ = 0;
//    *matrix++ = 0;
//    *matrix   = 1;
//}

int CPinholeCamera2::GetIndex() const
{
    return m_index;
}

void CPinholeCamera2::SetIndex(int index)
{
    m_index = index;
}

/**
 */
void CPinholeCamera2::LoadIdentity4x4(double *matrix) const
{
    // ROW/COLUMN
    *matrix++ = 1;
    *matrix++ = 0;
    *matrix++ = 0;
    *matrix++ = 0;
    // ROW/COLUMN
    *matrix++ = 0;
    *matrix++ = 1;
    *matrix++ = 0;
    *matrix++ = 0;
    // ROW/COLUMN
    *matrix++ = 0;
    *matrix++ = 0;
    *matrix++ = 1;
    *matrix++ = 0;
    // ROW/COLUMN
    *matrix++ = 0;
    *matrix++ = 0;
    *matrix++ = 0;
    *matrix   = 1;
}

/**
 */
void CPinholeCamera2::UpdateViewMatrix(const double *opticalCenter, const double *referencePoint, const double *up)
{
    double z[3] = { 0 },
        x[3] = { 0 },
        y[3] = { 0 };

    // OPTICAL AXIS

    MyMath::Subtract3(opticalCenter, referencePoint, z);
    MyMath::Normalize3(z);

    MyMath::CrossProduct3(up, z, x);
    MyMath::Normalize3(x);

    MyMath::CrossProduct3(z, x, y);

    // x0 x1 x2 t0    0  4  8 12    00 01 02 03
    // y0 y1 y2 t1    1  5  9 13    10 11 12 13
    // z0 z1 z2 t2    2  6 10 14    20 21 22 23
    //  0  0  0  1    3  7 11 15    30 31 32 33

    double orientation[16] = {
        x[0], y[0], z[0], 0,
        x[1], y[1], z[1], 0,
        x[2], y[2], z[2], 0,
        0, 0, 0, 1
    };

    double translation[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -opticalCenter[0], -opticalCenter[1], -opticalCenter[2], 1
    };

    MyMath::Multiply4x4(orientation, translation, m_viewMatrix);
}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
bool glFrustum(T left, T right, T bottom, T top, T nearVal, T farVal, T *m)
{
    double w = right - left,
        h = top - bottom,
        d = farVal - nearVal;

    if (fabs(w) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    if (fabs(h) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    if (fabs(d) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    // 0  4  8 12    00 01 02 03
    // 1  5  9 13    10 11 12 13
    // 2  6 10 14    20 21 22 23
    // 3  7 11 15    30 31 32 33

    m[0] = 2.0 * nearVal / w;
    m[5] = 2.0 * nearVal / h;
    m[8] = (right + left) / w;
    m[9] = (top + bottom) / h;
    m[10] = -(farVal + nearVal) / d;
    m[11] = -1.0;
    m[14] = -(2.0 * farVal * nearVal) / d;
    m[15] = 0.0;

    return true;
}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
bool gluPerspective(T fovY, T aspect, T zNear, T zFar, T *m)
{
    // TANGENT OF HALF FOVY
    T tangent = tan(MyMath::DegreesToRadians(fovY / 2.0)),
        // HALF HEIGHT OF NEAR PLANE
        height = zNear * tangent,
        // HALF WIDTH OF NEAR PLANE
        width = height * aspect;

    if (!glFrustum(-width, width, -height, height, zNear, zFar, m))
    {
        LOG_ERROR();

        return false;
    }

    return true;
}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
bool glOrtho(T left, T right, T bottom, T top, T nearVal, T farVal, T *m)
{
    double w = right - left,
        h = top - bottom,
        d = farVal - nearVal;

    if (fabs(w) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    if (fabs(h) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    if (fabs(d) < 1.0e-7)
    {
        LOG_ERROR();

        return false;
    }

    // 0  4  8 12    00 01 02 03
    // 1  5  9 13    10 11 12 13
    // 2  6 10 14    20 21 22 23
    // 3  7 11 15    30 31 32 33

    m[0] = 2.0 / w;
    m[5] = 2.0 / h;
    m[10] = -2.0 / d;
    m[12] = -(right + left) / w;
    m[13] = -(top + bottom) / h;
    m[14] = -(farVal + nearVal) / d;

    return true;
}

// http://www.scratchapixel.com/lessons/3d-advanced-lessons/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix/
void CPinholeCamera2::UpdateProjectionMatrix()
{
    //// (BEGIN OF) TESTING: (05-Nov-2016) REPLACING OPENGL BY VIEWPROJECTIONMATRIX ON MATRIX OPERATIONS
    //my::CViewProjectionMatrix<double> viewProjectionMatrix;
    //
    //my::int32 viewportWidth = m_viewport[2] - m_viewport[0],
    //    viewportHeight = m_viewport[3] - m_viewport[1];
    //
    //double viewportAspect = (double)viewportWidth / viewportHeight;
    //
    //if (m_projectionType == PERSPECTIVE_PROJECTION)
    //    viewProjectionMatrix.SetProjectionMatrix(m_fieldOfView, viewportAspect, m_nearPlaneDepth, m_farPlaneDepth);
    //else if (m_projectionType == ORTHOGONAL_PROJECTION)
    //{
    //    double nearPlaneHeight = m_topClippingPlane - m_bottomClippingPlane,
    //        nearPlaneWidth = nearPlaneHeight * viewportAspect,
    //        nearPlaneHorizontalMidpoint = 0.5 * (m_leftClippingPlane + m_rightClippingPlane);
    //    
    //    viewProjectionMatrix.SetProjectionMatrix(nearPlaneHorizontalMidpoint - 0.5 * nearPlaneWidth, nearPlaneHorizontalMidpoint + 0.5 * nearPlaneWidth, m_bottomClippingPlane, m_topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth);
    //}
    //
    //my::CMatrix4<double> projectionMatrix = viewProjectionMatrix.GetProjectionMatrix();
    //
    //MyMath::Assign16(projectionMatrix.Get(), m_projectionMatrix);
    //// (END OF) TESTING: (05-Nov-2016) REPLACING OPENGL BY VIEWPROJECTIONMATRIX ON MATRIX OPERATIONS

    LoadIdentity4x4(m_projectionMatrix);
    
    int viewportWidth = m_viewport[2] - m_viewport[0],
        viewportHeight = m_viewport[3] - m_viewport[1];
    
    double viewportAspect = (double)viewportWidth / viewportHeight;
    
    if (m_projectionType == PERSPECTIVE_PROJECTION)
        gluPerspective(m_fieldOfView, viewportAspect, m_nearPlaneDepth, m_farPlaneDepth, m_projectionMatrix);
    else if (m_projectionType == ORTHOGONAL_PROJECTION)
    {
    	double nearPlaneHeight = m_topClippingPlane - m_bottomClippingPlane,
            nearPlaneWidth = nearPlaneHeight * viewportAspect,
            nearPlaneHorizontalMidpoint = 0.5 * (m_leftClippingPlane + m_rightClippingPlane);
    
        glOrtho(nearPlaneHorizontalMidpoint - 0.5 * nearPlaneWidth, nearPlaneHorizontalMidpoint + 0.5 * nearPlaneWidth, m_bottomClippingPlane, m_topClippingPlane, m_nearPlaneDepth, m_farPlaneDepth, m_projectionMatrix);
    }
}

//// TODO: (08-May-2019) RE-EVALUATE
///**
// */
//void CPinholeCamera2::UpdateImagePlaneMatrix()
//{
//    int windowWidth = 0,
//    windowHeight = 0;
//    double w = 0,
//    h = 0,
//    as = 0,
//    bs = 0;
//    
//    windowWidth = m_viewport[2] - m_viewport[0];
//    windowHeight = m_viewport[3] - m_viewport[1];
//    
//    if (windowWidth > windowHeight)
//    {
//        w = 2.0*m_nearPlaneDepth*MyMath::Tangent(m_fieldOfView*0.00872664625);
//        h = ((double)windowHeight/windowWidth)*w;
//    }
//    else
//    {
//        h = 2. *m_nearPlaneDepth*MyMath::Tangent(m_fieldOfView*0.00872664625);
//        w = ((double)windowWidth/windowHeight)*h;
//    }
//    
//    as = w/windowWidth;
//    
//    m_imagePlaneMatrix[0] = m_viewMatrix[0]*as;
//    m_imagePlaneMatrix[1] = m_viewMatrix[4]*as;
//    m_imagePlaneMatrix[2] = m_viewMatrix[8]*as;
//    
//    bs = h/windowHeight;
//    
//    m_imagePlaneMatrix[3] = -m_viewMatrix[1]*bs;
//    m_imagePlaneMatrix[4] = -m_viewMatrix[5]*bs;
//    m_imagePlaneMatrix[5] = -m_viewMatrix[9]*bs;
//    
//    m_imagePlaneMatrix[6] = m_viewMatrix[ 2]*m_nearPlaneDepth - 0.5*(m_imagePlaneMatrix[0]*windowWidth + m_imagePlaneMatrix[3]*windowHeight);
//    m_imagePlaneMatrix[7] = m_viewMatrix[ 6]*m_nearPlaneDepth - 0.5*(m_imagePlaneMatrix[1]*windowWidth + m_imagePlaneMatrix[4]*windowHeight);
//    m_imagePlaneMatrix[8] = m_viewMatrix[10]*m_nearPlaneDepth - 0.5*(m_imagePlaneMatrix[2]*windowWidth + m_imagePlaneMatrix[5]*windowHeight);
//}

///**
//*/
//void CPinholeCamera2::UpdateViewProjectionMatrix()
//{
//    m_viewProjectionMatrix[0] =
//        m_projectionMatrix[0] * m_viewMatrix[0] +
//        m_projectionMatrix[4] * m_viewMatrix[1] +
//        m_projectionMatrix[8] * m_viewMatrix[2] +
//        m_projectionMatrix[12] * m_viewMatrix[3];
//    m_viewProjectionMatrix[4] =
//        m_projectionMatrix[0] * m_viewMatrix[4] +
//        m_projectionMatrix[4] * m_viewMatrix[5] +
//        m_projectionMatrix[8] * m_viewMatrix[6] +
//        m_projectionMatrix[12] * m_viewMatrix[7];
//    m_viewProjectionMatrix[8] =
//        m_projectionMatrix[0] * m_viewMatrix[8] +
//        m_projectionMatrix[4] * m_viewMatrix[9] +
//        m_projectionMatrix[8] * m_viewMatrix[10] +
//        m_projectionMatrix[12] * m_viewMatrix[11];
//    m_viewProjectionMatrix[12] =
//        m_projectionMatrix[0] * m_viewMatrix[12] +
//        m_projectionMatrix[4] * m_viewMatrix[13] +
//        m_projectionMatrix[8] * m_viewMatrix[14] +
//        m_projectionMatrix[12] * m_viewMatrix[15];
//    m_viewProjectionMatrix[1] =
//        m_projectionMatrix[1] * m_viewMatrix[0] +
//        m_projectionMatrix[5] * m_viewMatrix[1] +
//        m_projectionMatrix[9] * m_viewMatrix[2] +
//        m_projectionMatrix[13] * m_viewMatrix[3];
//    m_viewProjectionMatrix[5] =
//        m_projectionMatrix[1] * m_viewMatrix[4] +
//        m_projectionMatrix[5] * m_viewMatrix[5] +
//        m_projectionMatrix[9] * m_viewMatrix[6] +
//        m_projectionMatrix[13] * m_viewMatrix[7];
//    m_viewProjectionMatrix[9] =
//        m_projectionMatrix[1] * m_viewMatrix[8] +
//        m_projectionMatrix[5] * m_viewMatrix[9] +
//        m_projectionMatrix[9] * m_viewMatrix[10] +
//        m_projectionMatrix[13] * m_viewMatrix[11];
//    m_viewProjectionMatrix[13] =
//        m_projectionMatrix[1] * m_viewMatrix[12] +
//        m_projectionMatrix[5] * m_viewMatrix[13] +
//        m_projectionMatrix[9] * m_viewMatrix[14] +
//        m_projectionMatrix[13] * m_viewMatrix[15];
//    m_viewProjectionMatrix[2] =
//        m_projectionMatrix[2] * m_viewMatrix[0] +
//        m_projectionMatrix[6] * m_viewMatrix[1] +
//        m_projectionMatrix[10] * m_viewMatrix[2] +
//        m_projectionMatrix[14] * m_viewMatrix[3];
//    m_viewProjectionMatrix[6] =
//        m_projectionMatrix[2] * m_viewMatrix[4] +
//        m_projectionMatrix[6] * m_viewMatrix[5] +
//        m_projectionMatrix[10] * m_viewMatrix[6] +
//        m_projectionMatrix[14] * m_viewMatrix[7];
//    m_viewProjectionMatrix[10] =
//        m_projectionMatrix[2] * m_viewMatrix[8] +
//        m_projectionMatrix[6] * m_viewMatrix[9] +
//        m_projectionMatrix[10] * m_viewMatrix[10] +
//        m_projectionMatrix[14] * m_viewMatrix[11];
//    m_viewProjectionMatrix[14] =
//        m_projectionMatrix[2] * m_viewMatrix[12] +
//        m_projectionMatrix[6] * m_viewMatrix[13] +
//        m_projectionMatrix[10] * m_viewMatrix[14] +
//        m_projectionMatrix[14] * m_viewMatrix[15];
//    m_viewProjectionMatrix[3] =
//        m_projectionMatrix[3] * m_viewMatrix[0] +
//        m_projectionMatrix[7] * m_viewMatrix[1] +
//        m_projectionMatrix[11] * m_viewMatrix[2] +
//        m_projectionMatrix[15] * m_viewMatrix[3];
//    m_viewProjectionMatrix[7] =
//        m_projectionMatrix[3] * m_viewMatrix[4] +
//        m_projectionMatrix[7] * m_viewMatrix[5] +
//        m_projectionMatrix[11] * m_viewMatrix[6] +
//        m_projectionMatrix[15] * m_viewMatrix[7];
//    m_viewProjectionMatrix[11] =
//        m_projectionMatrix[3] * m_viewMatrix[8] +
//        m_projectionMatrix[7] * m_viewMatrix[9] +
//        m_projectionMatrix[11] * m_viewMatrix[10] +
//        m_projectionMatrix[15] * m_viewMatrix[11];
//    m_viewProjectionMatrix[15] =
//        m_projectionMatrix[3] * m_viewMatrix[12] +
//        m_projectionMatrix[7] * m_viewMatrix[13] +
//        m_projectionMatrix[11] * m_viewMatrix[14] +
//        m_projectionMatrix[15] * m_viewMatrix[15];
//}

/**
*/
bool CPinholeCamera2::ComputeViewProjectionMatrix(double *matrix) const
{
    matrix[0] =
        m_projectionMatrix[0] * m_viewMatrix[0] +
        m_projectionMatrix[4] * m_viewMatrix[1] +
        m_projectionMatrix[8] * m_viewMatrix[2] +
        m_projectionMatrix[12] * m_viewMatrix[3];
    matrix[4] =
        m_projectionMatrix[0] * m_viewMatrix[4] +
        m_projectionMatrix[4] * m_viewMatrix[5] +
        m_projectionMatrix[8] * m_viewMatrix[6] +
        m_projectionMatrix[12] * m_viewMatrix[7];
    matrix[8] =
        m_projectionMatrix[0] * m_viewMatrix[8] +
        m_projectionMatrix[4] * m_viewMatrix[9] +
        m_projectionMatrix[8] * m_viewMatrix[10] +
        m_projectionMatrix[12] * m_viewMatrix[11];
    matrix[12] =
        m_projectionMatrix[0] * m_viewMatrix[12] +
        m_projectionMatrix[4] * m_viewMatrix[13] +
        m_projectionMatrix[8] * m_viewMatrix[14] +
        m_projectionMatrix[12] * m_viewMatrix[15];
    matrix[1] =
        m_projectionMatrix[1] * m_viewMatrix[0] +
        m_projectionMatrix[5] * m_viewMatrix[1] +
        m_projectionMatrix[9] * m_viewMatrix[2] +
        m_projectionMatrix[13] * m_viewMatrix[3];
    matrix[5] =
        m_projectionMatrix[1] * m_viewMatrix[4] +
        m_projectionMatrix[5] * m_viewMatrix[5] +
        m_projectionMatrix[9] * m_viewMatrix[6] +
        m_projectionMatrix[13] * m_viewMatrix[7];
    matrix[9] =
        m_projectionMatrix[1] * m_viewMatrix[8] +
        m_projectionMatrix[5] * m_viewMatrix[9] +
        m_projectionMatrix[9] * m_viewMatrix[10] +
        m_projectionMatrix[13] * m_viewMatrix[11];
    matrix[13] =
        m_projectionMatrix[1] * m_viewMatrix[12] +
        m_projectionMatrix[5] * m_viewMatrix[13] +
        m_projectionMatrix[9] * m_viewMatrix[14] +
        m_projectionMatrix[13] * m_viewMatrix[15];
    matrix[2] =
        m_projectionMatrix[2] * m_viewMatrix[0] +
        m_projectionMatrix[6] * m_viewMatrix[1] +
        m_projectionMatrix[10] * m_viewMatrix[2] +
        m_projectionMatrix[14] * m_viewMatrix[3];
    matrix[6] =
        m_projectionMatrix[2] * m_viewMatrix[4] +
        m_projectionMatrix[6] * m_viewMatrix[5] +
        m_projectionMatrix[10] * m_viewMatrix[6] +
        m_projectionMatrix[14] * m_viewMatrix[7];
    matrix[10] =
        m_projectionMatrix[2] * m_viewMatrix[8] +
        m_projectionMatrix[6] * m_viewMatrix[9] +
        m_projectionMatrix[10] * m_viewMatrix[10] +
        m_projectionMatrix[14] * m_viewMatrix[11];
    matrix[14] =
        m_projectionMatrix[2] * m_viewMatrix[12] +
        m_projectionMatrix[6] * m_viewMatrix[13] +
        m_projectionMatrix[10] * m_viewMatrix[14] +
        m_projectionMatrix[14] * m_viewMatrix[15];
    matrix[3] =
        m_projectionMatrix[3] * m_viewMatrix[0] +
        m_projectionMatrix[7] * m_viewMatrix[1] +
        m_projectionMatrix[11] * m_viewMatrix[2] +
        m_projectionMatrix[15] * m_viewMatrix[3];
    matrix[7] =
        m_projectionMatrix[3] * m_viewMatrix[4] +
        m_projectionMatrix[7] * m_viewMatrix[5] +
        m_projectionMatrix[11] * m_viewMatrix[6] +
        m_projectionMatrix[15] * m_viewMatrix[7];
    matrix[11] =
        m_projectionMatrix[3] * m_viewMatrix[8] +
        m_projectionMatrix[7] * m_viewMatrix[9] +
        m_projectionMatrix[11] * m_viewMatrix[10] +
        m_projectionMatrix[15] * m_viewMatrix[11];
    matrix[15] =
        m_projectionMatrix[3] * m_viewMatrix[12] +
        m_projectionMatrix[7] * m_viewMatrix[13] +
        m_projectionMatrix[11] * m_viewMatrix[14] +
        m_projectionMatrix[15] * m_viewMatrix[15];

    return true;
}

// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
bool CPinholeCamera2::ComputeViewProjectionInverseMatrix(double *matrix) const
{
    double viewProjectionMatrix[16] = { 0 };

    if (!ComputeViewProjectionMatrix(viewProjectionMatrix))
    {
        LOG_ERROR();

        return false;
    }

    double cache[4][8] = { { 0 } };
    double m0 = 0,
        m1 = 0,
        m2 = 0,
        m3 = 0,
        s = 0;
    double *r0 = 0,
        *r1 = 0,
        *r2 = 0,
        *r3 = 0;

    r0 = cache[0];
    r1 = cache[1];
    r2 = cache[2];
    r3 = cache[3];

    r0[0] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 0, 0);
    r0[1] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 0, 1);
    r0[2] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 0, 2);
    r0[3] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 0, 3);
    r0[4] = 1;
    r0[5] = 0;
    r0[6] = 0;
    r0[7] = 0;

    r1[0] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 1, 0);
    r1[1] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 1, 1);
    r1[2] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 1, 2);
    r1[3] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 1, 3);
    r1[5] = 1;
    r1[4] = 0;
    r1[6] = 0;
    r1[7] = 0;

    r2[0] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 2, 0);
    r2[1] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 2, 1);
    r2[2] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 2, 2);
    r2[3] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 2, 3);
    r2[6] = 1;
    r2[4] = 0;
    r2[5] = 0;
    r2[7] = 0;

    r3[0] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 3, 0);
    r3[1] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 3, 1);
    r3[2] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 3, 2);
    r3[3] = GL_HELPER_MATRIX_ACCESS(viewProjectionMatrix, 3, 3);
    r3[7] = 1;
    r3[4] = 0;
    r3[5] = 0;
    r3[6] = 0;

    // Choose pivot - or die.
    if (fabs(r3[0]) > fabs(r2[0]))
        std::swap(r3, r2);
    if (fabs(r2[0]) > fabs(r1[0]))
        std::swap(r2, r1);
    if (fabs(r1[0]) > fabs(r0[0]))
        std::swap(r1, r0);

    if (r0[0] == 0)
    {
        LOG_ERROR();

        return 0;
    }

    // Eliminate first variable;
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];

    s = r0[1];

    r1[1] -= m1*s;
    r2[1] -= m2*s;
    r3[1] -= m3*s;

    s = r0[2];

    r1[2] -= m1*s;
    r2[2] -= m2*s;
    r3[2] -= m3*s;

    s = r0[3];

    r1[3] -= m1*s;
    r2[3] -= m2*s;
    r3[3] -= m3*s;

    s = r0[4];

    if (s != 0)
    {
        r1[4] -= m1*s;
        r2[4] -= m2*s;
        r3[4] -= m3*s;
    }

    s = r0[5];

    if (s != 0)
    {
        r1[5] -= m1*s;
        r2[5] -= m2*s;
        r3[5] -= m3*s;
    }

    s = r0[6];

    if (s != 0)
    {
        r1[6] -= m1*s;
        r2[6] -= m2*s;
        r3[6] -= m3*s;
    }

    s = r0[7];

    if (s != 0)
    {
        r1[7] -= m1*s;
        r2[7] -= m2*s;
        r3[7] -= m3*s;
    }

    // Choose pivot - or die.
    if (fabs(r3[1]) > fabs(r2[1]))
        std::swap(r3, r2);

    if (fabs(r2[1]) > fabs(r1[1]))
        std::swap(r2, r1);

    if (r1[1] == 0)
    {
        LOG_ERROR();

        return 0;
    }

    // Eliminate second variable.
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];

    r2[2] -= m2*r1[2];
    r3[2] -= m3*r1[2];
    r2[3] -= m2*r1[3];
    r3[3] -= m3*r1[3];

    s = r1[4];

    if (s != 0)
    {
        r2[4] -= m2*s;
        r3[4] -= m3*s;
    }

    s = r1[5];

    if (s != 0)
    {
        r2[5] -= m2*s;
        r3[5] -= m3*s;
    }

    s = r1[6];

    if (s != 0)
    {
        r2[6] -= m2*s;
        r3[6] -= m3*s;
    }

    s = r1[7];

    if (s != 0)
    {
        r2[7] -= m2*s;
        r3[7] -= m3*s;
    }

    // Choose pivot - or die.
    if (fabs(r3[2]) > fabs(r2[2]))
        std::swap(r3, r2);

    if (r2[2] == 0)
    {
        LOG_ERROR();

        return 0;
    }

    // Eliminate third variable.
    m3 = r3[2] / r2[2];

    r3[3] -= m3*r2[3];
    r3[4] -= m3*r2[4];
    r3[5] -= m3*r2[5];
    r3[6] -= m3*r2[6];
    r3[7] -= m3*r2[7];

    // Last check
    if (r3[3] == 0)
    {
        LOG_ERROR();

        return 0;
    }

    // Now back substitute row 3
    s = 1.0 / r3[3];

    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;

    // Now back substitute row 2
    m2 = r2[3];

    s = 1.0 / r2[2];

    r2[4] = s*(r2[4] - r3[4] * m2);
    r2[5] = s*(r2[5] - r3[5] * m2);
    r2[6] = s*(r2[6] - r3[6] * m2);
    r2[7] = s*(r2[7] - r3[7] * m2);

    m1 = r1[3];

    r1[4] -= r3[4] * m1;
    r1[5] -= r3[5] * m1;
    r1[6] -= r3[6] * m1;
    r1[7] -= r3[7] * m1;

    m0 = r0[3];

    r0[4] -= r3[4] * m0;
    r0[5] -= r3[5] * m0;
    r0[6] -= r3[6] * m0;
    r0[7] -= r3[7] * m0;

    // Nnow back substitute row 1
    m1 = r1[2];

    s = 1.0 / r1[1];

    r1[4] = s*(r1[4] - r2[4] * m1);
    r1[5] = s*(r1[5] - r2[5] * m1);
    r1[6] = s*(r1[6] - r2[6] * m1);
    r1[7] = s*(r1[7] - r2[7] * m1);

    m0 = r0[2];

    r0[4] -= r2[4] * m0;
    r0[5] -= r2[5] * m0;
    r0[6] -= r2[6] * m0;
    r0[7] -= r2[7] * m0;

    // Now back substitute row 0.
    m0 = r0[1];

    s = 1.0 / r0[0];

    r0[4] = s*(r0[4] - r1[4] * m0);
    r0[5] = s*(r0[5] - r1[5] * m0);
    r0[6] = s*(r0[6] - r1[6] * m0);
    r0[7] = s*(r0[7] - r1[7] * m0);

    GL_HELPER_MATRIX_ACCESS(matrix, 0, 0) = r0[4];
    GL_HELPER_MATRIX_ACCESS(matrix, 0, 1) = r0[5];
    GL_HELPER_MATRIX_ACCESS(matrix, 0, 2) = r0[6];
    GL_HELPER_MATRIX_ACCESS(matrix, 0, 3) = r0[7];
    GL_HELPER_MATRIX_ACCESS(matrix, 1, 0) = r1[4];
    GL_HELPER_MATRIX_ACCESS(matrix, 1, 1) = r1[5];
    GL_HELPER_MATRIX_ACCESS(matrix, 1, 2) = r1[6];
    GL_HELPER_MATRIX_ACCESS(matrix, 1, 3) = r1[7];
    GL_HELPER_MATRIX_ACCESS(matrix, 2, 0) = r2[4];
    GL_HELPER_MATRIX_ACCESS(matrix, 2, 1) = r2[5];
    GL_HELPER_MATRIX_ACCESS(matrix, 2, 2) = r2[6];
    GL_HELPER_MATRIX_ACCESS(matrix, 2, 3) = r2[7];
    GL_HELPER_MATRIX_ACCESS(matrix, 3, 0) = r3[4];
    GL_HELPER_MATRIX_ACCESS(matrix, 3, 1) = r3[5];
    GL_HELPER_MATRIX_ACCESS(matrix, 3, 2) = r3[6];
    GL_HELPER_MATRIX_ACCESS(matrix, 3, 3) = r3[7];

    return true;
}

/**
 */
bool CPinholeCamera2::UpdateProjection(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane, double nearPlaneDepth, double farPlaneDepth)
{
    if (m_projectionType != ORTHOGONAL_PROJECTION)
    {
        LOG_ERROR();
        
        return false;
    }
    
    m_leftClippingPlane = leftClippingPlane;
    m_rightClippingPlane = rightClippingPlane;
    m_bottomClippingPlane = bottomClippingPlane;
    m_topClippingPlane = topClippingPlane;
    
    m_nearPlaneDepth = nearPlaneDepth;
    m_farPlaneDepth = farPlaneDepth;
    
    UpdateProjectionMatrix();
    
    return true;
}

/**
 */
bool CPinholeCamera2::UpdateProjection(double fieldOfView, double nearPlaneDepth, double farPlaneDepth)
{
    if (m_projectionType != PERSPECTIVE_PROJECTION)
    {
        LOG_ERROR();
        
        return false;
    }
    
    m_fieldOfView = fieldOfView;
    
    m_nearPlaneDepth = nearPlaneDepth;
    m_farPlaneDepth = farPlaneDepth;
    
    UpdateProjectionMatrix();
    
    //UpdateImagePlaneMatrix();
    
    return true;
}

/**
 */
void CPinholeCamera2::Create()
{
    m_projectionType = PERSPECTIVE_PROJECTION;
    
    m_cameraType = TRACKBALL_CAMERA;

    m_trackballMode = FREE;

    m_leftClippingPlane = -1;
    m_rightClippingPlane = 1;
    m_bottomClippingPlane = -1;
    m_topClippingPlane = 1;
    
    m_fieldOfView = 45.0;
    
    m_nearPlaneDepth = 1;
    m_farPlaneDepth = 10.0;
    
    m_viewport[0] = 0;
    m_viewport[1] = 0;
    m_viewport[2] = 1;
    m_viewport[3] = 1;
    
    m_pitchRotation = 0;
    m_yawRotation = 0;
    m_rollRotation = 0;
    
    m_frontDisplacement = 0;
    m_sideDisplacement = 0;
    m_upDisplacement = 0;
    
    LoadIdentity4x4(m_projectionMatrix);
    LoadIdentity4x4(m_viewMatrix);
    
    //LoadIdentity4x4(m_viewProjectionMatrix);
    //LoadIdentity4x4(m_viewProjectionMatrixInverse);
    //
    //LoadIdentity3x3(m_imagePlaneMatrix);
    
    // TODO: (08-May-2019) RE-EVALUATE
    //MyMath::Assign(0, 0, 0, m_center);
    //MyMath::Assign(0, 0, 0, m_eye);
    //MyMath::Assign(0, 0, 0, m_up);
    m_index = my::Null<int>();
}

/**
 */
void CPinholeCamera2::Copy(const CPinholeCamera2& pinholeCamera)
{
    m_projectionType = pinholeCamera.m_projectionType;
    m_cameraType = pinholeCamera.m_cameraType;
    m_trackballMode = pinholeCamera.m_trackballMode;
    m_leftClippingPlane = pinholeCamera.m_leftClippingPlane;
    m_rightClippingPlane = pinholeCamera.m_rightClippingPlane;
    m_bottomClippingPlane = pinholeCamera.m_bottomClippingPlane;
    m_topClippingPlane = pinholeCamera.m_topClippingPlane;
    m_fieldOfView = pinholeCamera.m_fieldOfView;
    m_nearPlaneDepth = pinholeCamera.m_nearPlaneDepth;
    m_farPlaneDepth = pinholeCamera.m_farPlaneDepth;
    MyMath::Assign4(pinholeCamera.m_viewport, m_viewport);
    m_pitchRotation = pinholeCamera.m_pitchRotation;
    m_yawRotation = pinholeCamera.m_yawRotation;
    m_rollRotation = pinholeCamera.m_rollRotation;
    m_frontDisplacement = pinholeCamera.m_frontDisplacement;
    m_sideDisplacement = pinholeCamera.m_sideDisplacement;
    m_upDisplacement = pinholeCamera.m_upDisplacement;
    MyMath::Assign16(pinholeCamera.m_viewMatrix, m_viewMatrix);
    MyMath::Assign16(pinholeCamera.m_projectionMatrix, m_projectionMatrix);
    // TODO: (08-May-2019) RE-EVALUATE
    //MyMath::Assign16(pinholeCamera.m_viewProjectionMatrix, m_viewProjectionMatrix);
    //MyMath::Assign16(pinholeCamera.m_viewProjectionMatrixInverse, m_viewProjectionMatrixInverse);
    //MyMath::Assign9(pinholeCamera.m_imagePlaneMatrix, m_imagePlaneMatrix);
    // TODO: (08-May-2019) RE-EVALUATE
    //MyMath::Assign3(pinholeCamera.m_center, m_center);
    //MyMath::Assign3(pinholeCamera.m_eye, m_eye);
    //MyMath::Assign3(pinholeCamera.m_up, m_up);
    m_index = pinholeCamera.m_index;
}

