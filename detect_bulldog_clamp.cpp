// https://docs.opencv.org/3.4/d5/dae/tutorial_aruco_detection.html

// https://www.stereolabs.com/docs/unity/using-opencv-with-unity
// https://assetstore.unity.com/packages/tools/integration/opencv-for-unity-21088?srsltid=AfmBOooYxNG8NfvxEkzuXhI6kUA67I9sKUbaotyZ7InIkFV5Zz6XUq4n

#include <map>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/glew.h>

#ifdef _WIN32
#include <GL/wglew.h>
#endif // _WIN32

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <boost/shared_ptr.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

#include "aruco_samples_utility.h"

struct PINHOLE_CAMERA {
    PINHOLE_CAMERA()
    {
        double identityMatrix[16] = {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };

        m_viewport[0] = 0;
        m_viewport[1] = 0;
        m_viewport[2] = 1280;
        m_viewport[3] = 720;

        memcpy(m_viewMatrix, identityMatrix, sizeof(double) * 16);
        memcpy(m_projectionMatrix, identityMatrix, sizeof(double) * 16);
        memset(m_viewProjectionInverseMatrix, 0, sizeof(double) * 16);
    }

    PINHOLE_CAMERA(const PINHOLE_CAMERA& pinholeCamera)
    {
        memcpy(m_viewport, pinholeCamera.m_viewport, sizeof(int) * 4);
        memcpy(m_viewMatrix, pinholeCamera.m_viewMatrix, sizeof(double) * 16);
        memcpy(m_projectionMatrix, pinholeCamera.m_projectionMatrix, sizeof(double) * 16);
        memcpy(m_viewProjectionInverseMatrix, pinholeCamera.m_viewProjectionInverseMatrix, sizeof(double) * 16);
    }

    void operator=(const PINHOLE_CAMERA& pinholeCamera)
    {
        memcpy(m_viewport, pinholeCamera.m_viewport, sizeof(int) * 4);
        memcpy(m_viewMatrix, pinholeCamera.m_viewMatrix, sizeof(double) * 16);
        memcpy(m_projectionMatrix, pinholeCamera.m_projectionMatrix, sizeof(double) * 16);
        memcpy(m_viewProjectionInverseMatrix, pinholeCamera.m_viewProjectionInverseMatrix, sizeof(double) * 16);
    }

    // COMPUTES THE INVERSE OF THE VIEWMODEL-PROJECTION MATRIX
    bool Initialize()
    {
        // (BEGIN OF) COMPUTE THE VIEWMODEL-PROJECTION MATRIX
        double viewProjectionMatrix[16] = { 0 };

        viewProjectionMatrix[0] = m_projectionMatrix[0] * m_viewMatrix[0] + m_projectionMatrix[4] * m_viewMatrix[1] + m_projectionMatrix[8] * m_viewMatrix[2] + m_projectionMatrix[12] * m_viewMatrix[3];
        viewProjectionMatrix[4] = m_projectionMatrix[0] * m_viewMatrix[4] + m_projectionMatrix[4] * m_viewMatrix[5] + m_projectionMatrix[8] * m_viewMatrix[6] + m_projectionMatrix[12] * m_viewMatrix[7];
        viewProjectionMatrix[8] = m_projectionMatrix[0] * m_viewMatrix[8] + m_projectionMatrix[4] * m_viewMatrix[9] + m_projectionMatrix[8] * m_viewMatrix[10] + m_projectionMatrix[12] * m_viewMatrix[11];
        viewProjectionMatrix[12] = m_projectionMatrix[0] * m_viewMatrix[12] + m_projectionMatrix[4] * m_viewMatrix[13] + m_projectionMatrix[8] * m_viewMatrix[14] + m_projectionMatrix[12] * m_viewMatrix[15];
        viewProjectionMatrix[1] = m_projectionMatrix[1] * m_viewMatrix[0] + m_projectionMatrix[5] * m_viewMatrix[1] + m_projectionMatrix[9] * m_viewMatrix[2] + m_projectionMatrix[13] * m_viewMatrix[3];
        viewProjectionMatrix[5] = m_projectionMatrix[1] * m_viewMatrix[4] + m_projectionMatrix[5] * m_viewMatrix[5] + m_projectionMatrix[9] * m_viewMatrix[6] + m_projectionMatrix[13] * m_viewMatrix[7];
        viewProjectionMatrix[9] = m_projectionMatrix[1] * m_viewMatrix[8] + m_projectionMatrix[5] * m_viewMatrix[9] + m_projectionMatrix[9] * m_viewMatrix[10] + m_projectionMatrix[13] * m_viewMatrix[11];
        viewProjectionMatrix[13] = m_projectionMatrix[1] * m_viewMatrix[12] + m_projectionMatrix[5] * m_viewMatrix[13] + m_projectionMatrix[9] * m_viewMatrix[14] + m_projectionMatrix[13] * m_viewMatrix[15];
        viewProjectionMatrix[2] = m_projectionMatrix[2] * m_viewMatrix[0] + m_projectionMatrix[6] * m_viewMatrix[1] + m_projectionMatrix[10] * m_viewMatrix[2] + m_projectionMatrix[14] * m_viewMatrix[3];
        viewProjectionMatrix[6] = m_projectionMatrix[2] * m_viewMatrix[4] + m_projectionMatrix[6] * m_viewMatrix[5] + m_projectionMatrix[10] * m_viewMatrix[6] + m_projectionMatrix[14] * m_viewMatrix[7];
        viewProjectionMatrix[10] = m_projectionMatrix[2] * m_viewMatrix[8] + m_projectionMatrix[6] * m_viewMatrix[9] + m_projectionMatrix[10] * m_viewMatrix[10] + m_projectionMatrix[14] * m_viewMatrix[11];
        viewProjectionMatrix[14] = m_projectionMatrix[2] * m_viewMatrix[12] + m_projectionMatrix[6] * m_viewMatrix[13] + m_projectionMatrix[10] * m_viewMatrix[14] + m_projectionMatrix[14] * m_viewMatrix[15];
        viewProjectionMatrix[3] = m_projectionMatrix[3] * m_viewMatrix[0] + m_projectionMatrix[7] * m_viewMatrix[1] + m_projectionMatrix[11] * m_viewMatrix[2] + m_projectionMatrix[15] * m_viewMatrix[3];
        viewProjectionMatrix[7] = m_projectionMatrix[3] * m_viewMatrix[4] + m_projectionMatrix[7] * m_viewMatrix[5] + m_projectionMatrix[11] * m_viewMatrix[6] + m_projectionMatrix[15] * m_viewMatrix[7];
        viewProjectionMatrix[11] = m_projectionMatrix[3] * m_viewMatrix[8] + m_projectionMatrix[7] * m_viewMatrix[9] + m_projectionMatrix[11] * m_viewMatrix[10] + m_projectionMatrix[15] * m_viewMatrix[11];
        viewProjectionMatrix[15] = m_projectionMatrix[3] * m_viewMatrix[12] + m_projectionMatrix[7] * m_viewMatrix[13] + m_projectionMatrix[11] * m_viewMatrix[14] + m_projectionMatrix[15] * m_viewMatrix[15];
        // (END OF) COMPUTE THE VIEWMODEL-PROJECTION MATRIX

        double cache[4][8] = { { 0 } };
        double m0 = 0,
            m1 = 0,
            m2 = 0,
            m3 = 0,
            s = 0;
        double* r0 = 0,
            * r1 = 0,
            * r2 = 0,
            * r3 = 0;

        r0 = cache[0];
        r1 = cache[1];
        r2 = cache[2];
        r3 = cache[3];

#define READ_MATRIX_CELL(M, i, j) (*(M + i + (j<<2)))

        r0[0] = READ_MATRIX_CELL(viewProjectionMatrix, 0, 0);
        r0[1] = READ_MATRIX_CELL(viewProjectionMatrix, 0, 1);
        r0[2] = READ_MATRIX_CELL(viewProjectionMatrix, 0, 2);
        r0[3] = READ_MATRIX_CELL(viewProjectionMatrix, 0, 3);
        r0[4] = 1;
        r0[5] = 0;
        r0[6] = 0;
        r0[7] = 0;

        r1[0] = READ_MATRIX_CELL(viewProjectionMatrix, 1, 0);
        r1[1] = READ_MATRIX_CELL(viewProjectionMatrix, 1, 1);
        r1[2] = READ_MATRIX_CELL(viewProjectionMatrix, 1, 2);
        r1[3] = READ_MATRIX_CELL(viewProjectionMatrix, 1, 3);
        r1[5] = 1;
        r1[4] = 0;
        r1[6] = 0;
        r1[7] = 0;

        r2[0] = READ_MATRIX_CELL(viewProjectionMatrix, 2, 0);
        r2[1] = READ_MATRIX_CELL(viewProjectionMatrix, 2, 1);
        r2[2] = READ_MATRIX_CELL(viewProjectionMatrix, 2, 2);
        r2[3] = READ_MATRIX_CELL(viewProjectionMatrix, 2, 3);
        r2[6] = 1;
        r2[4] = 0;
        r2[5] = 0;
        r2[7] = 0;

        r3[0] = READ_MATRIX_CELL(viewProjectionMatrix, 3, 0);
        r3[1] = READ_MATRIX_CELL(viewProjectionMatrix, 3, 1);
        r3[2] = READ_MATRIX_CELL(viewProjectionMatrix, 3, 2);
        r3[3] = READ_MATRIX_CELL(viewProjectionMatrix, 3, 3);
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
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return 0;
        }

        // Eliminate first variable;
        m1 = r1[0] / r0[0];
        m2 = r2[0] / r0[0];
        m3 = r3[0] / r0[0];

        s = r0[1];

        r1[1] -= m1 * s;
        r2[1] -= m2 * s;
        r3[1] -= m3 * s;

        s = r0[2];

        r1[2] -= m1 * s;
        r2[2] -= m2 * s;
        r3[2] -= m3 * s;

        s = r0[3];

        r1[3] -= m1 * s;
        r2[3] -= m2 * s;
        r3[3] -= m3 * s;

        s = r0[4];

        if (s != 0)
        {
            r1[4] -= m1 * s;
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }

        s = r0[5];

        if (s != 0)
        {
            r1[5] -= m1 * s;
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }

        s = r0[6];

        if (s != 0)
        {
            r1[6] -= m1 * s;
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }

        s = r0[7];

        if (s != 0)
        {
            r1[7] -= m1 * s;
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        // Choose pivot - or die.
        if (fabs(r3[1]) > fabs(r2[1]))
            std::swap(r3, r2);

        if (fabs(r2[1]) > fabs(r1[1]))
            std::swap(r2, r1);

        if (r1[1] == 0)
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return 0;
        }

        // Eliminate second variable.
        m2 = r2[1] / r1[1];
        m3 = r3[1] / r1[1];

        r2[2] -= m2 * r1[2];
        r3[2] -= m3 * r1[2];
        r2[3] -= m2 * r1[3];
        r3[3] -= m3 * r1[3];

        s = r1[4];

        if (s != 0)
        {
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }

        s = r1[5];

        if (s != 0)
        {
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }

        s = r1[6];

        if (s != 0)
        {
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }

        s = r1[7];

        if (s != 0)
        {
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        // Choose pivot - or die.
        if (fabs(r3[2]) > fabs(r2[2]))
            std::swap(r3, r2);

        if (r2[2] == 0)
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return 0;
        }

        // Eliminate third variable.
        m3 = r3[2] / r2[2];

        r3[3] -= m3 * r2[3];
        r3[4] -= m3 * r2[4];
        r3[5] -= m3 * r2[5];
        r3[6] -= m3 * r2[6];
        r3[7] -= m3 * r2[7];

        // Last check
        if (r3[3] == 0)
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

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

        r2[4] = s * (r2[4] - r3[4] * m2);
        r2[5] = s * (r2[5] - r3[5] * m2);
        r2[6] = s * (r2[6] - r3[6] * m2);
        r2[7] = s * (r2[7] - r3[7] * m2);

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

        r1[4] = s * (r1[4] - r2[4] * m1);
        r1[5] = s * (r1[5] - r2[5] * m1);
        r1[6] = s * (r1[6] - r2[6] * m1);
        r1[7] = s * (r1[7] - r2[7] * m1);

        m0 = r0[2];

        r0[4] -= r2[4] * m0;
        r0[5] -= r2[5] * m0;
        r0[6] -= r2[6] * m0;
        r0[7] -= r2[7] * m0;

        // Now back substitute row 0.
        m0 = r0[1];

        s = 1.0 / r0[0];

        r0[4] = s * (r0[4] - r1[4] * m0);
        r0[5] = s * (r0[5] - r1[5] * m0);
        r0[6] = s * (r0[6] - r1[6] * m0);
        r0[7] = s * (r0[7] - r1[7] * m0);

        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 0, 0) = r0[4];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 0, 1) = r0[5];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 0, 2) = r0[6];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 0, 3) = r0[7];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 1, 0) = r1[4];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 1, 1) = r1[5];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 1, 2) = r1[6];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 1, 3) = r1[7];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 2, 0) = r2[4];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 2, 1) = r2[5];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 2, 2) = r2[6];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 2, 3) = r2[7];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 3, 0) = r3[4];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 3, 1) = r3[5];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 3, 2) = r3[6];
        READ_MATRIX_CELL(m_viewProjectionInverseMatrix, 3, 3) = r3[7];

        return true;
    }

    // (x, y, z) -> (s [-1, 1], t [-1, 1], depth [-1, 1])
    bool ProjectVertex(double x, double y, double z, double *s, double *t, double *d) const
    {
        double cache[7] = { 0 };

        cache[0] = m_viewMatrix[0] * x + m_viewMatrix[4] * y + m_viewMatrix[8] * z + m_viewMatrix[12];
        cache[1] = m_viewMatrix[1] * x + m_viewMatrix[5] * y + m_viewMatrix[9] * z + m_viewMatrix[13];
        cache[2] = m_viewMatrix[2] * x + m_viewMatrix[6] * y + m_viewMatrix[10] * z + m_viewMatrix[14];
        cache[3] = m_viewMatrix[3] * x + m_viewMatrix[7] * y + m_viewMatrix[11] * z + m_viewMatrix[15];

        cache[4] = m_projectionMatrix[0] * cache[0] + m_projectionMatrix[4] * cache[1] + m_projectionMatrix[8] * cache[2] + m_projectionMatrix[12] * cache[3];
        cache[5] = m_projectionMatrix[1] * cache[0] + m_projectionMatrix[5] * cache[1] + m_projectionMatrix[9] * cache[2] + m_projectionMatrix[13] * cache[3];
        cache[6] = m_projectionMatrix[2] * cache[0] + m_projectionMatrix[6] * cache[1] + m_projectionMatrix[10] * cache[2] + m_projectionMatrix[14] * cache[3];

        if (cache[2] == 0.0)
            return false;

        cache[2] = -1.0 / cache[2];

        cache[4] *= cache[2];
        cache[5] *= cache[2];
        cache[6] *= cache[2];

        *s = cache[4];
        *t = cache[5];
        *d = cache[6];

        return true;
    }

    bool UnProjectVertex(double s, double t, double d, double *x, double *y, double *z) const
    {
        *x = m_viewProjectionInverseMatrix[0] * s + m_viewProjectionInverseMatrix[4] * t + m_viewProjectionInverseMatrix[8] * d + m_viewProjectionInverseMatrix[12];
        *y = m_viewProjectionInverseMatrix[1] * s + m_viewProjectionInverseMatrix[5] * t + m_viewProjectionInverseMatrix[9] * d + m_viewProjectionInverseMatrix[13];
        *z = m_viewProjectionInverseMatrix[2] * s + m_viewProjectionInverseMatrix[6] * t + m_viewProjectionInverseMatrix[10] * d + m_viewProjectionInverseMatrix[14];

        double scale = m_viewProjectionInverseMatrix[3] * s + m_viewProjectionInverseMatrix[7] * t + m_viewProjectionInverseMatrix[11] * d + m_viewProjectionInverseMatrix[15];

#if defined(_DEBUG)
        if (scale == 0)
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return false;
        }
#endif //#if defined(_DEBUG)

        scale = 1.0 / scale;

        *x *= scale;
        *y *= scale;
        *z *= scale;

        return true;
    }

    bool ToCamera(double ix, double iy, double iz, const PINHOLE_CAMERA& pinholeCamera, double *ox, double *oy, double *oz)
    {
        double s = 0.0,
            t = 0.0,
            d = 0.0;

        if (!ProjectVertex(ix, iy, iz, &s, &t, &d))
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return false;
        }

        if (!pinholeCamera.UnProjectVertex(s, t, d, ox, oy, oz))
        {
            printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

            return false;
        }

        return true;
    }

    int m_viewport[4];
    double m_viewMatrix[16];
    double m_projectionMatrix[16];
    double m_viewProjectionInverseMatrix[16];
};

// VIEW
float m_backgroundColor[] = { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f };
// "The angular field of view (often called FOV) is a measure of the angular size for an image produced by a camera. The value is initially set to 63.54 (the angular field of view for the iPhone 6)."
double m_angularFieldOfView = 45.0/*63.54*/;
PINHOLE_CAMERA m_viewerCamera;
int m_mouseButton = 0;
int m_mouseX = -1;
int m_mouseY = -1;

// DETECTOR
const char* m_parserKeyDefinition =
"{d        | 0     | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
"DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
"DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
"DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16,"
"DICT_APRILTAG_16h5=17, DICT_APRILTAG_25h9=18, DICT_APRILTAG_36h10=19, DICT_APRILTAG_36h11=20}"
"{cd       |       | Input file with custom dictionary }"
"{ci       | 0     | Camera id }"
"{c        |       | Camera intrinsic parameters. Needed for camera pose }"
"{l        | 0.1   | Marker side length (in meters). Needed for correct scale in camera pose }"
"{dp       |       | File of marker detector parameters }"
"{r        |       | show rejectedCornerPointArray candidates too }"
"{refine   |       | Corner refinement: CORNER_REFINE_NONE=0, CORNER_REFINE_SUBPIX=1,"
"CORNER_REFINE_CONTOUR=2, CORNER_REFINE_APRILTAG=3}";
const std::string m_refinementMethodArray[4] = {
    "None",
    "Subpixel",
    "Contour",
    "AprilTag"
};
cv::VideoCapture m_videoCapture;
int m_viewportWidth = 0;
int m_viewportHeight = 0;
boost::shared_ptr<cv::aruco::ArucoDetector> m_markerDetector;
cv::Mat m_cameraMatrix;
cv::Mat m_distortionCoefficientArray;
float m_markerLength = 1.0f;
std::map<int, PINHOLE_CAMERA> m_markerIndexToPinholeCameraMap;

bool CreateViewerCamera()
{
    int viewport[4] = { 0, 0, 1280, 720 };

    double viewMatrix[16] = { 1.0000000000000000, 0.0000000000000000, 0.0000000000000000, 0.0000000000000000,
        0.0000000000000000, 0.55470019622522915, -0.83205029433784372, 0.0000000000000000,
        0.0000000000000000, 0.83205029433784372, 0.55470019622522915, 0.0000000000000000,
        0.0000000000000000, 0.0000000000000000, -3.6055512754639896, 1.0000000000000000 },
        projectionMatrix[16] = { 1.3579951288348662, 0.0000000000000000, 0.0000000000000000, 0.0000000000000000,
        0.0000000000000000, 2.4142135623730954, 0.0000000000000000, 0.0000000000000000,
        0.0000000000000000, 0.0000000000000000, -1.0002000200020003, -1.0000000000000000,
        0.0000000000000000, 0.0000000000000000, -0.20002000200020004, 0.0000000000000000 };

    memcpy(m_viewerCamera.m_viewport, viewport, sizeof(int) * 4);
    memcpy(m_viewerCamera.m_viewMatrix, viewMatrix, sizeof(double) * 16);
    memcpy(m_viewerCamera.m_projectionMatrix, projectionMatrix, sizeof(double) * 16);

    return true;
}

void LoadCamera(PINHOLE_CAMERA& pinholeCamera)
{
    // (BEGIN OF) TESTING: (18-Sep-2019) APPLY TRANSFORM!
    glMatrixMode(GL_PROJECTION);

    glLoadMatrixd(pinholeCamera.m_projectionMatrix);

    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixd(pinholeCamera.m_viewMatrix);
    // (END OF) TESTING: (18-Sep-2019) APPLY TRANSFORM!
}

// TESTING: (09-May-2019) http://www.songho.ca/opengl/gl_projectionmatrix.html
template <typename T>
bool glFrustum(T left, T right, T bottom, T top, T nearVal, T farVal, T* m)
{
    double w = right - left,
        h = top - bottom,
        d = farVal - nearVal;

    if (fabs(w) < 1.0e-7)
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    if (fabs(h) < 1.0e-7)
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    if (fabs(d) < 1.0e-7)
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

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

void OnDestroy(int returnValue)
{
    int windowName = glutGetWindow();

    if (windowName)
        glutDestroyWindow(windowName);

    exit(returnValue);
}

void DrawGrid()
{
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_LINE_STIPPLE);

    glLineStipple(2, 0xAAAA);

    glLineWidth(1.0);

    glBegin(GL_LINES);

    glColor4d(0.5, 0.5, 0.5, 1.0);

    double meterToFeet = 3.280839895013123;

    for (int i = -10; i <= 10; ++i)
    {
        glVertex2d(meterToFeet * (-10), meterToFeet * i);
        glVertex2d(meterToFeet * 10, meterToFeet * i);

        glVertex2d(meterToFeet * i, meterToFeet * (-10));
        glVertex2d(meterToFeet * i, meterToFeet * 10);
    }

    glEnd();

    glLineWidth(1.0);

    glDisable(GL_LINE_STIPPLE);

    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
}

void DrawSceneMarker()
{
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);

    // INCHES TO FEET
    double inchesToFeet = 0.0833333333333333f;

    glPolygonOffset(1.0, 1.0);

    glEnable(GL_POLYGON_OFFSET_FILL);

    glLineWidth(2.0f);

    // (BEGIN OF) MARKERS
    glBegin(GL_LINE_LOOP);

    glColor3d(0.0, 0.0, 0.0);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(4.0 * inchesToFeet, 0.0, 0.0);
    glVertex3d(4.0 * inchesToFeet, 4.0 * inchesToFeet, 0.0);
    glVertex3d(0.0, 4.0 * inchesToFeet, 0.0);

    glEnd();
    // (END OF) MARKERS

    glPolygonOffset(1.0, 2.0);

    glLineWidth(4.0f);

    // (BEGIN OF) AXES
    glBegin(GL_LINES);

    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(1.0 * inchesToFeet, 0.0, 0.0);

    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 1.0 * inchesToFeet, 0.0);

    glEnd();
    // (END OF) AXES

    glLineWidth(1.0f);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
}

void UpdateMarkers()
{
    cv::Mat image;

    m_videoCapture.retrieve(image);

    if (image.empty())
        return /*false*/;

    if ((m_viewportWidth == 0) /*||
        (m_viewportHeight == 0)*/)
    {
        m_viewportWidth = image.cols;
        m_viewportHeight = image.rows;
    }

    std::vector<int> makerIdArray;

    std::vector<std::vector<cv::Point2f> > cornerPointArray, 
        rejectedCornerPointArray;

    m_markerDetector->detectMarkers(image, cornerPointArray, makerIdArray, rejectedCornerPointArray);

    size_t markerCount = cornerPointArray.size();

    std::vector<cv::Vec3d> rotationVectorArray(markerCount),
        translationVectorArray(markerCount);

    if (!makerIdArray.empty())
    {
        cv::Mat objectPointArray(4, 1, CV_32FC3);

        objectPointArray.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-m_markerLength / 2.0f, m_markerLength / 2.0f, 0);
        objectPointArray.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(m_markerLength / 2.0f, m_markerLength / 2.0f, 0);
        objectPointArray.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(m_markerLength / 2.0f, -m_markerLength / 2.0f, 0);
        objectPointArray.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-m_markerLength / 2.0f, -m_markerLength / 2.0f, 0);
        
        for (size_t markerIndex = 0; markerIndex < markerCount; ++markerIndex)
        {
            const auto& translationVector = translationVectorArray.at(markerIndex);

            if (solvePnP(objectPointArray, cornerPointArray.at(markerIndex), m_cameraMatrix, m_distortionCoefficientArray, rotationVectorArray.at(markerIndex), translationVector))
            {
                // (1,1) focal length (f_y)
                double yFocalLength = m_cameraMatrix.at<double>(1, 1),
                    fovY = (2.0 * atan(m_viewportHeight / (2.0 * yFocalLength))) * 180.0 / 3.1415926535897931,
                    nearPlaneDepth = 0.01,
                    farPlaneDepth = 10.0;

                PINHOLE_CAMERA pinholeCamera;

                memcpy(pinholeCamera.m_viewport, m_viewerCamera.m_viewport, sizeof(int) * 4);

                // (BEGIN OF) TESTING: (09-Jul-2025) CREATE A PERSPECTIVE MATRIX http://www.songho.ca/opengl/gl_projectionmatrix.html
                // TANGENT OF HALF FOVY
                double tangent = tan((fovY / 2.0) * 3.1415926535897931 / 180.0),
                    // HALF HEIGHT OF NEAR PLANE
                    height = nearPlaneDepth * tangent,
                    viewportAspect = (double)m_viewportWidth / m_viewportHeight,
                    // HALF WIDTH OF NEAR PLANE
                    width = height * viewportAspect;

                if (!glFrustum(-width, width, -height, height, nearPlaneDepth, farPlaneDepth, pinholeCamera.m_projectionMatrix))
                {
                    printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

                    return /*false*/;
                }
                // (END OF) TESTING: (09-Jul-2025) CREATE A PERSPECTIVE MATRIX http://www.songho.ca/opengl/gl_projectionmatrix.html

                cv::Mat rMat3x3(3, 3, CV_64F);

                cv::Rodrigues(rotationVectorArray.at(markerIndex), rMat3x3);

                // ARE TRANSLATIONS STORED IN METERS?
                double meterToFeet = 3.280839895013123;

                double translationVectorInFeet[3] = { meterToFeet * translationVector[0], meterToFeet * translationVector[1] , meterToFeet * translationVector[2] };

                // (BEGIN OF) TRICKY: (01-Sep-2022) THE CONVERSION FROM THE OPENCV CAMERA MATRIX (ROTATION + TRANSLATION) TO THE OPENGL PINHOLE CAMERA IS NOT WORKING, SO I'LL TAKE A DETOUR USING THE PinholeCamera TYPE!
                double viewMatrix[16] = { -rMat3x3.at<double>(0, 0), -rMat3x3.at<double>(1, 0), -rMat3x3.at<double>(2, 0), 0.0, -rMat3x3.at<double>(0, 1), -rMat3x3.at<double>(1, 1), -rMat3x3.at<double>(2, 1), 0.0, -rMat3x3.at<double>(0, 2), -rMat3x3.at<double>(1, 2), -rMat3x3.at<double>(2, 2), 0.0, -translationVectorInFeet[0], -translationVectorInFeet[1], -translationVectorInFeet[2], 1.0 };

                memcpy(pinholeCamera.m_viewMatrix, viewMatrix, sizeof(double) * 16);

                int markerId = makerIdArray[markerIndex];
            
                // CLAMP MARKERS WILL BE GIVEN AS THE ID "-1"
                if ((markerId == 2) ||
                    (markerId == 10) ||
                    (markerId == 19) ||
                    (markerId == 21))
                {
                    markerId = -1;
                }

                // CLIP MARKERS WILL BE GIVEN AS THE ID "-2"
                if ((markerId == 0) ||
                    (markerId == 48) ||
                    (markerId == 27))
                {
                    markerId = -2;
                }

                if (!pinholeCamera.Initialize())
                {
                    printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

                    return /*false*/;
                }

                m_markerIndexToPinholeCameraMap[markerId] = pinholeCamera;
            }
        }
    }
}

void DrawClip(const PINHOLE_CAMERA& pinholeCamera)
{
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);

    glLineWidth(8.0f);

    double inchesToFeet = 0.0833333333333333,
        meterToFeet = 3.280839895013123;

    for (auto& markerIndexToPinholeCamera : m_markerIndexToPinholeCameraMap)
    {
        LoadCamera(markerIndexToPinholeCamera.second);

        glBegin(GL_LINES);

        // CLIP MARKERS WILL BE GIVEN AS THE ID "-2"
        if (/*(markerIndexToPinholeCamera.first == 0) ||
            (markerIndexToPinholeCamera.first == 48) ||
            (markerIndexToPinholeCamera.first == 27)*/
            (markerIndexToPinholeCamera.first == -2))
        {
            glColor3d(0.0, 0.0, 0.0);

            double x = 0.5 * inchesToFeet,
                y = 0.0,
                z = (-m_markerLength) * 0.5 * meterToFeet,
                ox = 0.0,
                oy = 0.0,
                oz = 0.0;

            //markerIndexToPinholeCamera.second.ToCamera(x, y, z, pinholeCamera, &ox, &oy, &oz);

            //x = ox;
            //y = oy;
            //z = oz;

            glVertex3d(x, y, z);

            x = (-1.0) * inchesToFeet;
            y = 0.0;
            z = (-m_markerLength) * 0.5 * meterToFeet;

            //markerIndexToPinholeCamera.second.ToCamera(x, y, z, pinholeCamera, &ox, &oy, &oz);

            //x = ox;
            //y = oy;
            //z = oz;

            glVertex3d(x, y, z);
        }

        glEnd();
    }

    glLineWidth(1.0f);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
}

void DrawBulldogClamp(const PINHOLE_CAMERA& pinholeCamera)
{
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LINE_SMOOTH);

    glLineWidth(8.0f);

    double inchesToFeet = 0.0833333333333333,
        meterToFeet = 3.280839895013123;

    for (auto& markerIndexToPinholeCamera : m_markerIndexToPinholeCameraMap)
    {
        LoadCamera(markerIndexToPinholeCamera.second);

        glBegin(GL_LINES);

        // CLAMP MARKERS WILL BE GIVEN AS THE ID "-1"
        if (/*(markerIndexToPinholeCamera.first == 2) ||
            (markerIndexToPinholeCamera.first == 10) ||
            (markerIndexToPinholeCamera.first == 19) ||
            (markerIndexToPinholeCamera.first == 21)*/
            (markerIndexToPinholeCamera.first == -1))
        {
            glColor3d(0.0, 0.0, 0.0);
            
            // LENGTH OF THE CLAMP IN RELATION TO THE POSITION OF THE MARKER OVER THE CLAMP
            double x = 5.75 * inchesToFeet,
                y = 0.0,
                z = (-m_markerLength) * 0.5 * meterToFeet,
                ox = 0.0,
                oy = 0.0,
                oz = 0.0;

            //markerIndexToPinholeCamera.second.ToCamera(x, y, z, pinholeCamera, &ox, &oy, &oz);

            //x = ox;
            //y = oy;
            //z = oz;

            glVertex3d(x, y, z);

            // LENGTH OF THE CLAMP IN RELATION TO THE POSITION OF THE MARKER OVER THE CLAMP
            x = (-8.5) * inchesToFeet;
            y = 0.0;
            z = (-m_markerLength) * 0.5 * meterToFeet;

            //markerIndexToPinholeCamera.second.ToCamera(x, y, z, pinholeCamera, &ox, &oy, &oz);

            //x = ox;
            //y = oy;
            //z = oz;

            glVertex3d(x, y, z);
        }

        glEnd();
    }

    glLineWidth(1.0f);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
}

void DrawScene()
{
    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], m_backgroundColor[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateMarkers();

    const auto& markerIndexToPinholeCameraIterator = m_markerIndexToPinholeCameraMap.find(5);

    glEnable(GL_MULTISAMPLE);

    if (markerIndexToPinholeCameraIterator != m_markerIndexToPinholeCameraMap.end())
    {
        // 'SCENE' CAMERA (MARKER AT THE BOTTOM OF THE BOX)
        LoadCamera(markerIndexToPinholeCameraIterator->second);

        DrawGrid();

        DrawSceneMarker();

        DrawBulldogClamp(markerIndexToPinholeCameraIterator->second);

        DrawClip(markerIndexToPinholeCameraIterator->second);
    }

    glDisable(GL_MULTISAMPLE);
}

void OnPaint()
{
    DrawScene();

    glColor4d(1.0, 0.5, 0.0, 1.0);

    glutSwapBuffers();
}

void OnSize(GLsizei w, GLsizei h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    int viewport[4] = { 0, 0, w, h };

    memcpy(m_viewerCamera.m_viewport, viewport, sizeof(int) * 4);
}

void OnIdle()
{
    m_videoCapture.grab();

    glutPostRedisplay();
}

void OnVisible(int isVisible)
{
    if (isVisible == GLUT_VISIBLE)
        glutIdleFunc(OnIdle);
    else
        glutIdleFunc(0);
}

void OnKeyDown(unsigned char key, int x, int y)
{
    switch (key)
    {
        // ESC
    case 27:
        OnDestroy(EXIT_SUCCESS);
        break;
    }

    glutPostRedisplay();
}

void OnMouse(int button, int state, int x, int y)
{
    m_mouseButton = button;

    if (state == GLUT_UP)
        m_mouseButton = 0;

    m_mouseX = x;
    m_mouseY = y;
}

void OnMouseMove(int x, int y)
{
    int dX = x - m_mouseX,
        dY = y - m_mouseY,
        dMax;

    if (abs(dX) > abs(dY))
        dMax = dX;
    else
        dMax = dY;

    switch (m_mouseButton) {
    case GLUT_LEFT_BUTTON:
        break;

    case GLUT_MIDDLE_BUTTON:
        break;

    case GLUT_RIGHT_BUTTON:
        break;
    }

    m_mouseX = x;
    m_mouseY = y;

    glutPostRedisplay();
}

void OnPassiveMouseMove(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;

    glutPostRedisplay();
}

bool OnCreateMarkerDetector(int argumentCount, char** argumentArray)
{
    cv::CommandLineParser parser(argumentCount, argumentArray, m_parserKeyDefinition);

    m_markerLength = parser.get<float>("l");

    cv::aruco::DetectorParameters detectorParams = readDetectorParamsFromCommandLine(parser);

    cv::aruco::Dictionary dictionary = readDictionatyFromCommandLine(parser);

    if (parser.has("refine"))
    {
        // OVERRIDE CORNERREFINEMENTMETHOD READ FROM CONFIG FILE
        int userDefinedMethod = parser.get<cv::aruco::CornerRefineMethod>("refine");

        if ((userDefinedMethod) < 0 ||
            (userDefinedMethod) >= 4)
        {
            std::cout << "Corner refinement method should be in range 0..3" << std::endl;

            return false;
        }

        detectorParams.cornerRefinementMethod = userDefinedMethod;
    }

    std::cout << "Corner refinement method: " << m_refinementMethodArray[detectorParams.cornerRefinementMethod] << std::endl;

    int cameraId = parser.get<int>("ci");

    if (!parser.check())
    {
        parser.printErrors();

        return false;
    }

    readCameraParamsFromCommandLine(parser, m_cameraMatrix, m_distortionCoefficientArray);

    try
    {
        m_markerDetector.reset(new cv::aruco::ArucoDetector(dictionary, detectorParams));
    }
    catch (std::exception& e)
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    if (!m_videoCapture.open(cameraId))
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    return true;
}

bool OnCreateGraphicsLibrary(int argumentCount, char** argumentArray)
{
    glutInit(&argumentCount, argumentArray);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);

    glutCreateWindow("detect_bulldog_clamp");

    if (glewInit() != GLEW_OK)
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    glutDisplayFunc(OnPaint);
    glutKeyboardFunc(OnKeyDown);
    glutMouseFunc(OnMouse);
    glutMotionFunc(OnMouseMove);
    glutPassiveMotionFunc(OnPassiveMouseMove);
    glutReshapeFunc(OnSize);
    glutVisibilityFunc(OnVisible);

    if (!CreateViewerCamera())
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        return false;
    }

    return true;
}

// -d=0 -ci=0 -refine=3 -dp="aruco/default_detector_parameters.yml" -l=0.021082 -c="aruco/Logitech c922_camera_parameters.yml"
int main(int argumentCount, char** argumentArray)
{
    if (!OnCreateMarkerDetector(argumentCount, argumentArray))
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        OnDestroy(EXIT_FAILURE);
    }

    if (!OnCreateGraphicsLibrary(argumentCount, argumentArray))
    {
        printf("%s (%d): An error has occurred.\n", __FUNCTION__, __LINE__);

        OnDestroy(EXIT_FAILURE);
    }

    glutMainLoop();

    return EXIT_SUCCESS;
}
