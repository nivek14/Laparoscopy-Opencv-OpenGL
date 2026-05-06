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

#include <Common.h>
#include <Logger.h>
#include <GLHelper.h>
#include <PinholeCamera2.h>

// VIEW
float m_backgroundColor[] = { 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f };
// "The angular field of view (often called FOV) is a measure of the angular size for an image produced by a camera. The value is initially set to 63.54 (the angular field of view for the iPhone 6)."
double m_angularFieldOfView = 45.0/*63.54*/;
boost::shared_ptr<CPinholeCamera2> m_viewerCamera;
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
std::map<int, CPinholeCamera2> m_markerIndexToPinholeCameraMap;

bool CreateViewerCamera()
{
    if (!m_viewerCamera)
        m_viewerCamera.reset(new CPinholeCamera2());

    HEALTH_CHECK(!m_viewerCamera, false);

    double opticalCenter[3] = { 0.0, -3.0, 2.0 },
        referencePoint[3] = { 0.0, 0.0, 0.0 },
        upVector[3] = { 0.0, 0.0, 1.0 };

    if (!m_viewerCamera->Create(opticalCenter, referencePoint, upVector, m_angularFieldOfView, 0.1, 1000.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)))
    {
        LOG_ERROR();

        return false;
    }

    m_viewerCamera->SetTrackballMode(CPinholeCamera2::AROUND_Z);

    return true;
}

void LoadCamera(CPinholeCamera2& pinholeCamera)
{
    pinholeCamera.ApplyTransform();

    // (BEGIN OF) TESTING: (18-Sep-2019) APPLY TRANSFORM!
    glMatrixMode(GL_PROJECTION);

    glLoadMatrixT(pinholeCamera.GetProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixT(pinholeCamera.GetViewMatrix());
    // (END OF) TESTING: (18-Sep-2019) APPLY TRANSFORM!
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
                CPinholeCamera2 pinholeCamera;

                // (1,1) focal length (f_y)
                double yFocalLength = m_cameraMatrix.at<double>(1, 1),
                    fovY = MyMath::RadiansToDegrees(2.0 * atan(m_viewportHeight / (2.0 * yFocalLength))),
                    nearPlaneDepth = 0.01,
                    farPlaneDepth = 10.0;

                pinholeCamera.Create(fovY, nearPlaneDepth, farPlaneDepth, m_viewportWidth, m_viewportHeight);

                cv::Mat rMat3x3(3, 3, CV_64F);

                cv::Rodrigues(rotationVectorArray.at(markerIndex), rMat3x3);

                // ARE TRANSLATIONS STORED IN METERS?
                double meterToFeet = 3.280839895013123;

                double translationVectorInFeet[3] = { meterToFeet * translationVector[0], meterToFeet * translationVector[1] , meterToFeet * translationVector[2] };

                // (BEGIN OF) TRICKY: (01-Sep-2022) THE CONVERSION FROM THE OPENCV CAMERA MATRIX (ROTATION + TRANSLATION) TO THE OPENGL PINHOLE CAMERA IS NOT WORKING, SO I'LL TAKE A DETOUR USING THE PinholeCamera TYPE!
                double viewMatrix[16] = { -rMat3x3.at<double>(0, 0), -rMat3x3.at<double>(1, 0), -rMat3x3.at<double>(2, 0), 0.0, -rMat3x3.at<double>(0, 1), -rMat3x3.at<double>(1, 1), -rMat3x3.at<double>(2, 1), 0.0, -rMat3x3.at<double>(0, 2), -rMat3x3.at<double>(1, 2), -rMat3x3.at<double>(2, 2), 0.0, -translationVectorInFeet[0], -translationVectorInFeet[1], -translationVectorInFeet[2], 1.0 };

                pinholeCamera.SetViewMatrix(viewMatrix);

                my::CVector3<double> opticalCenter = pinholeCamera.GetOpticalCenter(),
                    opticalAxis = pinholeCamera.GetOpticalAxis(),
                    upVector = pinholeCamera.GetUpAxis();

                double opticalCenterPointer[3] = { opticalCenter.x(), opticalCenter.y(), opticalCenter.z() },
                    centerPointer[3] = { opticalCenterPointer[0] + opticalAxis[0], opticalCenterPointer[1] + opticalAxis[1], opticalCenterPointer[2] + opticalAxis[2] },
                    upVectorPointer[3] = { upVector.x(), upVector.y(), upVector.z() };

                if (!pinholeCamera.Create(opticalCenterPointer, centerPointer, upVectorPointer, fovY, nearPlaneDepth, farPlaneDepth, m_viewportWidth, m_viewportHeight))
                {
                    LOG_ERROR();

                    return /*false*/;
                }

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

                m_markerIndexToPinholeCameraMap[markerId] = pinholeCamera;
            }
        }
    }
}

void DrawClip()
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
            glVertex3d(0.5 * inchesToFeet, 0.0, (-m_markerLength) * 0.5 * meterToFeet);
            glVertex3d((-1.0) * inchesToFeet, 0.0, (-m_markerLength) * 0.5 * meterToFeet);
        }

        glEnd();
    }

    glLineWidth(1.0f);

    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LINE_SMOOTH);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
}

void DrawBulldogClamp()
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
            glVertex3d(5.75 * inchesToFeet, 0.0, (-m_markerLength) * 0.5 * meterToFeet);
            // LENGTH OF THE CLAMP IN RELATION TO THE POSITION OF THE MARKER OVER THE CLAMP
            glVertex3d((-8.5) * inchesToFeet, 0.0, (-m_markerLength) * 0.5 * meterToFeet);
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

    if (markerIndexToPinholeCameraIterator != m_markerIndexToPinholeCameraMap.end())
        LoadCamera(markerIndexToPinholeCameraIterator->second);
    else
        LoadCamera(*m_viewerCamera);

    glEnable(GL_MULTISAMPLE);

    DrawGrid();

    DrawSceneMarker();

    DrawBulldogClamp();

    DrawClip();

    glDisable(GL_MULTISAMPLE);

    HEALTH_CHECK(!GlHelper::IsGlOk(), /*false*/);
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

    m_viewerCamera->SetViewport(0, 0, w, h);
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
    case '!':
        if (!m_viewerCamera->ToFile("3dPoseViewer_CAMERA.json"))
            LOG_ERROR();
        break;

    case '1':
        if (!m_viewerCamera->FromFile("3dPoseViewer_CAMERA.json"))
            LOG_ERROR();
        break;

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
        if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
        {
            m_viewerCamera->MoveSide(0.01 * dX);
            m_viewerCamera->MoveUp(0.01 * (-dY));
        }
        else if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
            m_viewerCamera->MoveFront(0.05 * dY);
        else
        {
            m_viewerCamera->Pitch(0.1 * dY);
            m_viewerCamera->Yaw(0.1 * dX);
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        m_viewerCamera->MoveSide(0.0025 * dX);
        m_viewerCamera->MoveUp(0.0025 * (-dY));
        break;

    case GLUT_RIGHT_BUTTON:
        m_viewerCamera->MoveFront(0.01 * dY);
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
        LOG_MESSAGE(e.what());

        return false;
    }

    if (!m_videoCapture.open(cameraId))
    {
        LOG_ERROR();

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
        LOG_ERROR();

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
        LOG_ERROR();

        return false;
    }

    HEALTH_CHECK(!GlHelper::IsGlOk(), false);

    return true;
}

// -d=0 -ci=0 -refine=3 -dp="aruco/default_detector_parameters.yml" -l=0.021082 -c="aruco/Logitech c922_camera_parameters.yml"
int main(int argumentCount, char** argumentArray)
{
    if (!OnCreateMarkerDetector(argumentCount, argumentArray))
    {
        LOG_ERROR();

        OnDestroy(EXIT_FAILURE);
    }

    if (!OnCreateGraphicsLibrary(argumentCount, argumentArray))
    {
        LOG_ERROR();

        OnDestroy(EXIT_FAILURE);
    }

    glutMainLoop();

    return EXIT_SUCCESS;
}
