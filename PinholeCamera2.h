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

#if !defined(PINHOLE_CAMERA_2_INCLUDED)
#define PINHOLE_CAMERA_2_INCLUDED

#include <Vector3.h>

class CPinholeCamera2
{
public:
	enum PROJECTION_TYPE {
		PERSPECTIVE_PROJECTION,
		ORTHOGONAL_PROJECTION
	};

	enum CAMERA_TYPE {
		TRACKBALL_CAMERA,
		FLYBY_CAMERA
	};

	enum TRACKBALL_MODE {
		FREE,
		AROUND_Y,
        AROUND_Z
	};

	CPinholeCamera2();
	CPinholeCamera2(const CPinholeCamera2 &pinholeCamera);

	CPinholeCamera2& operator=(const CPinholeCamera2 &pinholeCamera);

	// ORTHOGONAL
    bool Create(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight);
	// PERSPECTIVE
    bool Create(double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight);
    bool Create(my::CVector3<double> opticalCenter, my::CVector3<double> referencePoint, my::CVector3<double> upVector, double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight);
    bool Create(const double* opticalCenter, const double* referencePoint, const double* upVector, double fieldOfView, double nearPlaneDepth, double farPlaneDepth, int viewportWidth, int viewportHeight);

    PROJECTION_TYPE GetProjectionType() const;

    void SetCameraType(CAMERA_TYPE type);
    CAMERA_TYPE GetCameraType() const;

    TRACKBALL_MODE GetTrackballMode() const;
    void SetTrackballMode(TRACKBALL_MODE trackballMode);

    double GetLeftClippingPlane() const;
	bool SetLeftClippingPlane(double leftClippingPlane);
	double GetRightClippingPlane() const;
	bool SetRightClippingPlane(double rightClippingPlane);
	double GetBottomClippingPlane() const;
	bool SetBottomClippingPlane(double bottomClippingPlane);
	double GetTopClippingPlane() const;
	bool SetTopClippingPlane(double topClippingPlane);

    // fovY?
	double GetFieldOfView() const;
    // fovY?
    bool SetFieldOfView(double fieldOfView);
    bool SetFieldOfView(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane);

	double GetNearPlaneDepth() const;
	bool SetNearPlaneDepth(double nearPlaneDepth);
	double GetFarPlaneDepth() const;
	bool SetFarPlaneDepth(double farPlaneDepth);

    int GetViewportWidth() const;
    int GetViewportHeight() const;

    // "YES", AROUND X AXIS
	void Pitch(double angle);
    // "NO", AROUND Y AXIS
	void Yaw(double angle);
    // AROUND Z AXIS
	void Roll(double angle);

    // TRICKY: (??-???-????) ALONG Z AXIS, WORLD COORDINATES.
	void MoveFront(double displacement);
    // TRICKY: (??-???-????) ALONG X AXIS, WORLD COORDINATES.
    void MoveSide(double displacement);
    // TRICKY: (??-???-????) ALONG Y AXIS, WORLD COORDINATES.
    void MoveUp(double displacement);

    //// ORTHOGONAL/PERSPECTIVE
    //void Zoom(double factor);

	void ApplyTransform();

    // OpenGL ADAPTOR
    const double *GetViewMatrix() const;
    // OpenGL ADAPTOR
    void SetViewMatrix(const double*viewMatrix, bool transpose = false);

    // OpenGL ADAPTOR
    const double *GetProjectionMatrix() const;
    // OpenGL ADAPTOR
    void SetProjectionMatrix(const double* projectionMatrix, bool transpose = false);
    //template <typename T>
    //void SetProjectionMatrix(const T *projectionMatrix);
    //template <typename T>
    //void SetProjectionMatrix(my::CMatrix4<T>& projectionMatrix);

    // TODO: (08-May-2019) RE-EVALUATE
    //const double *GetViewProjectionMatrix();
    //const double *GetViewProjectionInverseMatrix();

    const int *GetViewport() const;
    //void SetViewport(const int *viewport);
    void SetViewport(int x, int y, int width, int height);

    // TODO: (08-May-2019) RE-EVALUATE
    //const double *GetImagePlaneMatrix();

    my::CVector3<double> GetOpticalCenter() const;
    my::CVector3<double> GetOpticalAxis() const;
    my::CVector3<double> GetUpAxis() const;

    // TODO: (08-May-2019) RE-EVALUATE
    //double GetAspect() const;

    bool ToFile(std::string cameraFileName) const;
    bool FromFile(std::string cameraFileName);

    // TODO: (08-May-2019) RE-EVALUATE
    //bool ProjectVertex(const double* worldCoord, double *screenCoord) const;
    // TRICKY: (07-Jun-2019) THE ORIGIN OF THE CAMERA IS AT THE LEFT-BOTTOM CORNER!
    // http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
    my::CVector3<double> ProjectVertex(my::CVector3<double> worldCoord) const;
    bool UnProjectVertex(const double *screenCoord, double* worldCoord);
    //my::CVector3<double> UnProjectVertex(my::CVector3<double> screenCoord);

    // TODO: (08-May-2019) RE-EVALUATE
    // DEBUG ONLY! (25-Sep-2015)
    //bool GetViewProjectionMatrix(my::CViewProjectionMatrix<double>& viewProjectionMatrix) const
    //{
    //    viewProjectionMatrix.SetViewMatrix(GetViewMatrix());
    //    viewProjectionMatrix.SetProjectionMatrix(GetProjectionMatrix());

    //    HEALTH_CHECK(!viewProjectionMatrix.GetOpticalCenter().IsValid(), false);
    //    HEALTH_CHECK(!viewProjectionMatrix.GetOpticalAxis().IsValid(), false);
    //    HEALTH_CHECK(!viewProjectionMatrix.GetUp().IsValid(), false);
    //    HEALTH_CHECK(!MyMath::IsValid(viewProjectionMatrix.GetNearClippingPlane()), false);
    //    HEALTH_CHECK(!MyMath::IsValid(viewProjectionMatrix.GetFarClippingPlane()), false);

    //    const int* viewport = GetViewport();

    //    HEALTH_CHECK(!viewport, false);

    //    viewProjectionMatrix.SetViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    //    viewProjectionMatrix.SetFieldOfView(GetFieldOfView());

    //    HEALTH_CHECK(!MyMath::IsValid(viewProjectionMatrix.GetFieldOfView()), false);

    //    //// (BEGIN OF) DEBUG ONLY! (06-Oct-2016) 
    //    //my::CVector3<double> opticalCenter = viewProjectionMatrix.GetOpticalCenter();
    //    //std::cerr << "DEBUG ONLY! opticalCenter: " << "<" << opticalCenter.x() << ", " << opticalCenter.y() << ", " << opticalCenter.z() << ">" << std::endl;
    //    //HEALTH_CHECK(!opticalCenter.IsValid(), false);
    //    //// (END OF) DEBUG ONLY! (06-Oct-2016) 

    //    //// (BEGIN OF) DEBUG ONLY! (06-Oct-2016) 
    //    //my::CVector3<double> opticalAxis = viewProjectionMatrix.GetOpticalAxis();
    //    //std::cerr << "DEBUG ONLY! opticalAxis: " << "<" << opticalAxis.x() << ", " << opticalAxis.y() << ", " << opticalAxis.z() << ">" << std::endl;
    //    //HEALTH_CHECK(!opticalAxis.IsValid(), false);
    //    //// (END OF) DEBUG ONLY! (06-Oct-2016) 

    //    //// (BEGIN OF) DEBUG ONLY! (06-Oct-2016) 
    //    //my::CVector3<double> up = viewProjectionMatrix.GetUp();
    //    //std::cerr << "DEBUG ONLY! up: " << "<" << up.x() << ", " << up.y() << ", " << up.z() << ">" << std::endl;
    //    //HEALTH_CHECK(!up.IsValid(), false);
    //    //// (END OF) DEBUG ONLY! (06-Oct-2016) 

    //    //// (BEGIN OF) DEBUG ONLY! (06-Oct-2016) 
    //    //std::cerr << "DEBUG ONLY! nearClippingPlane: " << viewProjectionMatrix.GetNearClippingPlane() << std::endl;
    //    //HEALTH_CHECK(!MyMath::IsValid(viewProjectionMatrix.GetNearClippingPlane()), false);
    //    //// (END OF) DEBUG ONLY! (06-Oct-2016) 

    //    //// (BEGIN OF) DEBUG ONLY! (06-Oct-2016) 
    //    //std::cerr << "DEBUG ONLY! farClippingPlane: " << viewProjectionMatrix.GetFarClippingPlane() << std::endl;
    //    //HEALTH_CHECK(!MyMath::IsValid(viewProjectionMatrix.GetFarClippingPlane()), false);
    //    //// (END OF) DEBUG ONLY! (06-Oct-2016) 

    //    return true;
    //}

    // TODO: (08-May-2019) RE-EVALUATE
    //// DEBUG ONLY! (25-Sep-2015)
    //bool SetViewProjectionMatrix(const my::CViewProjectionMatrix<double>& viewProjectionMatrix);

    // TODO: (08-May-2019) RE-EVALUATE
    //std::string ToJson() const;

    int GetIndex() const;
    void SetIndex(int index);

private:
    // TODO: (08-May-2019) RE-EVALUATE
    //void LoadIdentity3x3(double *matrix) const;
    void LoadIdentity4x4(double *matrix) const;

    void UpdateViewMatrix(const double *opticalCenter, const double *referencePoint, const double *up);
    void UpdateProjectionMatrix();
    // TODO: (08-May-2019) RE-EVALUATE
    //void UpdateImagePlaneMatrix();
    //void UpdateViewProjectionMatrix();

    bool ComputeViewProjectionMatrix(double *matrix) const;
    bool ComputeViewProjectionInverseMatrix(double *matrix) const;

    bool UpdateProjection(double leftClippingPlane, double rightClippingPlane, double bottomClippingPlane, double topClippingPlane, double nearPlaneDepth, double farPlaneDepth);
	bool UpdateProjection(double fieldOfView, double nearPlaneDepth, double farPlaneDepth);

	void Create();
	void Copy(const CPinholeCamera2 &pinholeCamera);

protected:
	PROJECTION_TYPE m_projectionType;

	CAMERA_TYPE m_cameraType;  

    TRACKBALL_MODE m_trackballMode;

	double m_leftClippingPlane;
	double m_rightClippingPlane;
	double m_bottomClippingPlane;
	double m_topClippingPlane;

	double m_fieldOfView;

	double m_nearPlaneDepth;
	double m_farPlaneDepth;

    // X, Y, WIDTH, HEIGHT
	int m_viewport[4];

    // "YES", AROUND X AXIS
    double m_pitchRotation;
    // "NO", AROUND Y AXIS
    double m_yawRotation;
    // AROUND Z AXIS
    double m_rollRotation;

    // TRICKY: (??-???-????) ALONG Z AXIS, WORLD COORDINATES.
    double m_frontDisplacement;
    // TRICKY: (??-???-????) ALONG X AXIS, WORLD COORDINATES.
    double m_sideDisplacement;
    // TRICKY: (??-???-????) ALONG Y AXIS, WORLD COORDINATES.
    double m_upDisplacement;

    // COLUMN-MAJOR ORDER
	double m_viewMatrix[16];
    // COLUMN-MAJOR ORDER
    double m_projectionMatrix[16];

    // TODO: (08-May-2019) RE-EVALUATE
    //double m_viewProjectionMatrix[16];
	//double m_viewProjectionMatrixInverse[16];

    // TODO: (08-May-2019) RE-EVALUATE
    //double m_imagePlaneMatrix[9];

    // TODO: (08-May-2019) RE-EVALUATE
    //// SPECIFIES THE POSITION OF THE EYE POINT
    //double m_center[3];
    //// Z AXIS, REFERENCE MINUS CENTER POINT.
    //double m_eye[3];
    //double m_up[3];

    int m_index;
};

#endif //#if !defined(PINHOLE_CAMERA_2_INCLUDED)

