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

// Coment this line to link to glew.
//#include <GLee.h>

#if !defined(__GNUC__)
#include <windows.h>
#endif // !defined(__GNUC__)

#if !defined(__glee_h_)
#include <GL/glew.h>

#if defined(_WIN32)
#include <GL/wglew.h>
#elif defined(__APPLE__)
#include <GL/glew.h>
//#else
//#include <GL/glxew.h>
#endif //#if defined(_WIN32)
#endif //#if !defined(__glee_h_)

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>

#include <Logger.h>
#include <MyMath.h>

#include <GlHelper.h>

/**
*/
bool GlHelper::IsGlOk()
{
	bool isOk;
	GLenum error;

	isOk = true;

	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		do 
		{
			switch(error) 
			{
			case GL_INVALID_ENUM:
				LOG_MESSAGE("An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
				break;
			case GL_INVALID_VALUE:
				LOG_MESSAGE("A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
				break;
			case GL_INVALID_OPERATION:
				LOG_MESSAGE("The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
				break;
			case GL_STACK_OVERFLOW:
				LOG_MESSAGE("This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.");
				break;
			case GL_STACK_UNDERFLOW:
				LOG_MESSAGE("This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.");
				break;
			case GL_OUT_OF_MEMORY:
				LOG_MESSAGE("There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
				break;
			case GL_TABLE_TOO_LARGE:
				LOG_MESSAGE("The specified table exceeds the implementation's maximum supported table size.  The offending command is ignored and has no other side effect than to set the error flag.");
				break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                LOG_MESSAGE("The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.");
                break;
			}
		} while ((error = glGetError()) != GL_NO_ERROR);

		isOk = false;
	}

	return isOk;
}

void GlHelper::ShowMessage(std::string message, int size, int x, int y)
{
#if defined(__glut_h__)
    int width,
        height;
    float aspect,
        screenSize;
    int charIndex;

    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);

    HEALTH_CHECK(message.empty(), /*false*/);
    HEALTH_CHECK(width == 0, /*false*/);
    HEALTH_CHECK(height == 0, /*false*/);

    aspect = (float)height/(float)width;

    screenSize = 2.0f*((float)size/(float)height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glTranslatef(-1.0f + 2.0f*((float)x/(float)width), -1.0f + 2.0f*((float)y/(float)height), 0.0f);

    // GLUT font re-scale (1/119.05).
    glScalef(screenSize*(1.0f/119.05f)*aspect, screenSize*(1.0f/119.05f), 1.0f);

    for (charIndex=0; charIndex!=message.size(); ++charIndex)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, (int)message[charIndex]);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
#else //#if defined(__glut_h__)
    LOG_ERROR();
#endif //#if defined(__glut_h__)
}

// XY PLANE
void GlHelper::DrawCircle(boost::shared_ptr<CVertexPool> mesh, CVertexPool::ValueType x, CVertexPool::ValueType y, CVertexPool::ValueType radius, my::int32 segmentCount)
{
    CVertexPool::ValueType theta = 2.0 * MyMath::Pi<double>() / segmentCount,
        c = cos(theta),
        s = sin(theta);

    CVertexPool::ValueType xp = radius,
        yp = 0;

    mesh->Clear();

    mesh->SetMode(CVertexPool::LINES);

    for (my::int32 i = 0; i < segmentCount; ++i)
    {
        mesh->AddVertex(xp + x, yp + y, 0.0);
        mesh->AddLine(i, (i + 1) % segmentCount);

        CVertexPool::ValueType t = xp;

        xp = c * xp - s * yp;
        yp = s * t + c * yp;
    }
}

// XY PLANE, TEXTURE COORDS [0, 1]
void GlHelper::DrawDisk(boost::shared_ptr<CVertexPool> mesh, CVertexPool::ValueType x, CVertexPool::ValueType y, CVertexPool::ValueType radius, my::int32 segmentCount)
{
    CVertexPool::ValueType theta = 2.0 * MyMath::Pi<double>() / segmentCount,
        c = cos(theta),
        s = sin(theta);

    CVertexPool::ValueType xp = radius,
        yp = 0;

    mesh->Clear();

    mesh->SetMode(CVertexPool::TRIANGLES);

    mesh->AddTexCoord(0.5, 0.5);
    mesh->AddVertex(x, y, 0.0);

    for (my::int32 i = 0; i < (segmentCount - 1); ++i)
    {
        mesh->AddTexCoord(0.5 * (xp / radius) + 0.5, 0.5 * ((-yp) / radius) + 0.5);
        mesh->AddVertex(xp + x, yp + y, 0.0);

        mesh->AddTriangle(0, i + 1, i + 2);

        CVertexPool::ValueType t = xp;

        xp = c * xp - s * yp;
        yp = s * t + c * yp;
    }

    mesh->AddTexCoord(0.5 * (xp / radius) + 0.5, 0.5 * ((-yp) / radius) + 0.5);
    mesh->AddVertex(xp + x, yp + y, 0.0);

    mesh->AddTriangle(0, segmentCount, 1);
}

bool GlHelper::DrawSphere(CVertexPool::ValueType *center, CVertexPool::ValueType radius, int slices, int stacks)
{
    CVertexPool::ValueType txz,
        ty,
        ty2,
        angleXZ,
        angleY,
        angleY2,
        sy,
        sy2,
        dx,
        dy,
        dy2,
        dz,
        norma,
        position[3];
    int i,
        j;

    HEALTH_CHECK(radius <= 1.0e-7, false);
    
    if (slices < 4)
    {
        LOG_ERROR();

        return false;
    }

    HEALTH_CHECK(stacks < 4, false);
    HEALTH_CHECK(!center, false);

    for (i=stacks ; --i; )
    {
        glBegin(GL_TRIANGLE_STRIP);

        ty = (CVertexPool::ValueType)i/(stacks - 1);
        ty2 = (CVertexPool::ValueType)(i - 1)/(stacks - 1);

		angleY = ty*MyMath::Pi<CVertexPool::ValueType>();
        angleY2 = ty2*MyMath::Pi<CVertexPool::ValueType>();

        dy = cos(angleY)*radius;
        dy2 = cos(angleY2)*radius;

        sy = sin(ty*MyMath::Pi<CVertexPool::ValueType>());
        sy2 = sin(ty2*MyMath::Pi<CVertexPool::ValueType>());

        for (j=slices; j--; )
        {
            txz = (CVertexPool::ValueType)j/(slices - 1);

            angleXZ = txz*2.0f*MyMath::Pi<CVertexPool::ValueType>();

            dx = cos(angleXZ)*radius*sy2;
            dz = sin(angleXZ)*radius*sy2;

            position[0] = center[0] + dx;
            position[1] = center[1] + dy2;
            position[2] = center[2] + dz;

            norma = sqrt(dx*dx + dy2*dy2 + dz*dz);

            glNormal3T(dx/norma, dy2/norma, dz/norma);
            glTexCoord2T(txz, ty2);
            glVertex3T(position[0], position[1], position[2]);

            dx = cos(angleXZ)*radius*sy;
            dz = sin(angleXZ)*radius*sy;

            position[0] = center[0] + dx;
            position[1] = center[1] + dy;
            position[2] = center[2] + dz;

            norma = sqrt(dx*dx + dy*dy + dz*dz);

            glNormal3T(dx/norma, dy/norma, dz/norma);
            glTexCoord2T(txz, ty);
            glVertex3T(position[0], position[1], position[2]);
        }

        glEnd();
    }

    return true;
}

bool GlHelper::DrawSphere(boost::shared_ptr<CVertexPool>& mesh, CVertexPool::ValueType *center, CVertexPool::ValueType radius, int slices, int stacks)
{
    CVertexPool::ValueType txz,
        ty,
        ty2,
        angleXZ,
        angleY,
        angleY2,
        sy,
        sy2,
        dx,
        dy,
        dy2,
        dz,
        norma,
        position[3];
    int i,
        j;
    int vertexIndex;

    HEALTH_CHECK(radius <= 1.0e-7, false);
    HEALTH_CHECK(slices < 4, false);
    HEALTH_CHECK(stacks < 4, false);
    HEALTH_CHECK(!center, false);
    HEALTH_CHECK(!mesh.get(), false);

    mesh->Clear();

    mesh->SetMode(CVertexPool::TRIANGLES);

    for (i=stacks ; --i; )
    {
        ty = (CVertexPool::ValueType)i/(stacks - 1);
        ty2 = (CVertexPool::ValueType)(i - 1)/(stacks - 1);

		angleY = ty*MyMath::Pi<CVertexPool::ValueType>();
        angleY2 = ty2*MyMath::Pi<CVertexPool::ValueType>();

        dy = cos(angleY)*radius;
        dy2 = cos(angleY2)*radius;

        sy = sin(ty*MyMath::Pi<CVertexPool::ValueType>());
        sy2 = sin(ty2*MyMath::Pi<CVertexPool::ValueType>());

        for (j=slices; j--; )
        {
            vertexIndex = mesh->GetVertexCount();

            if (j)
            {
                mesh->AddElement(vertexIndex);
                mesh->AddElement(vertexIndex + 3);
                mesh->AddElement(vertexIndex + 1);

                mesh->AddElement(vertexIndex);
                mesh->AddElement(vertexIndex + 2);
                mesh->AddElement(vertexIndex + 3);
            }

            txz = (CVertexPool::ValueType)j/(slices - 1);

            angleXZ = txz*2.0f*MyMath::Pi<CVertexPool::ValueType>();

            dx = cos(angleXZ)*radius*sy2;
            dz = sin(angleXZ)*radius*sy2;

            position[0] = center[0] + dx;
            position[1] = center[1] + dy2;
            position[2] = center[2] + dz;

            norma = sqrt(dx*dx + dy2*dy2 + dz*dz);

            mesh->AddNormal(-dx/norma, -dy2/norma, -dz/norma);
            mesh->AddTexCoord(txz, ty2);
            mesh->AddVertex(position[0], position[1], position[2]);

            dx = cos(angleXZ)*radius*sy;
            dz = sin(angleXZ)*radius*sy;

            position[0] = center[0] + dx;
            position[1] = center[1] + dy;
            position[2] = center[2] + dz;

            norma = sqrt(dx*dx + dy*dy + dz*dz);

            mesh->AddNormal(-dx/norma, -dy/norma, -dz/norma);
            mesh->AddTexCoord(txz, ty);
            mesh->AddVertex(position[0], position[1], position[2]);
        }
    }

    return true;
}

/**
*/
void GlHelper::DrawText(double screenX, double screenY, double sizeInPixels, double rotation, void* font, std::string characterArray)
{
    GlHelper::PUSH_MATRIX_HELPER pushMatrixHelper(true);

    GLint viewportMatrix[4] = { 0 };

    glGetIntegerv(GL_VIEWPORT, viewportMatrix);

    double scale = (2.0 * sizeInPixels) / ((double)viewportMatrix[3] - viewportMatrix[1]),
        aspectRatio = ((double)viewportMatrix[3] - viewportMatrix[1]) / ((double)viewportMatrix[2] - viewportMatrix[0]);

    glTranslated(screenX, screenY, 0.0);

    glRotated(rotation, 0.0, 0.0, 1.0);

    glScaled((1.0 / (119.05 + 33.33)) * scale * aspectRatio, (1.0 / (119.05 + 33.33)) * scale, 1.0);

    for (std::string::const_iterator characterIterator = characterArray.cbegin(); characterIterator != characterArray.cend(); ++characterIterator)
    {
        glutStrokeCharacter(font, (*characterIterator));
    }
}

// GLUT_BITMAP_9_BY_15
// GLUT_BITMAP_8_BY_13
// GLUT_BITMAP_TIMES_ROMAN_10
// GLUT_BITMAP_TIMES_ROMAN_24
// GLUT_BITMAP_HELVETICA_10
// GLUT_BITMAP_HELVETICA_12
// GLUT_BITMAP_HELVETICA_18
// TRICKY: (28-Oct-2020) THE OBJECT COORDINATES PRESENTED BY glRasterPos ARE TREATED JUST LIKE THOSE OF A glVertex COMMAND. THEY ARE TRANSFORMED BY THE CURRENT MODELVIEW AND PROJECTION MATRICES AND PASSED TO THE CLIPPING STAGE.
void GlHelper::DrawString(void* font, GLdouble x, GLdouble y, GLdouble z, const std::string& string)
{
    glRasterPos3d(x, y, z);

    for (int n = 0; n < string.size(); ++n)
        glutBitmapCharacter(font, string[n]);
}

/**
*/
template <>
bool GlHelper::DrawCylinder(const double *originCoord, const double *destinationCoord, double bottomRadius, double topRadius, int slices)
{
	int i = 0;
	double norma = 0,
        axis[3] = {0},
        u[3] = {0},
        v[3] = {0},
		angle = 0,
		pu = 0,
		pv = 0,
		spub = 0,
		spvb = 0,
		sput = 0,
		spvt = 0;

	norma = 0;

	for (i=3; i--; )
	{
		axis[i] = destinationCoord[i] - originCoord[i];

		norma += axis[i]*axis[i];
	}

    // Collapsed to a disk.
    if (norma < MyMath::Epsilon<double>())
        return true;

    norma = MyMath::SquareRoot(norma);

	for (i=3; i--; )
		axis[i] /= norma;

	if ((axis[0] + axis[2]) > MyMath::Epsilon<double>())
	{
		u[0] = -axis[2];
		u[1] = 0.0;
		u[2] = axis[0];
	}
	else
	{
		u[0] = axis[1];
		u[1] = -axis[0];
		u[2] = 0.0;
	}

	norma = 0.0;

	for (i=3; i--; )
		norma += u[i]*u[i];

    // Collapsed to a disk.
	if (norma < MyMath::Epsilon<double>())
		return true;

	norma = MyMath::SquareRoot(norma);

	for (i=3; i--; )
		u[i] /= norma;

	v[0] = u[1]*axis[2] - u[2]*axis[1];
	v[1] = u[2]*axis[0] - u[0]*axis[2];
	v[2] = u[0]*axis[1] - u[1]*axis[0];

	norma = 0.0;

	for (i=3; i--; )
		norma += v[i]*v[i];

    // Collapsed to a disk.
	if (norma < MyMath::Epsilon<double>())
		return true;

	norma = MyMath::SquareRoot(norma);

	for (i=3; i--; )
		v[i] /= norma;

	glBegin(GL_TRIANGLE_STRIP);

	for (i=slices; i--; )
	{
		angle = ((double)i/(slices - 1))*2.0*MyMath::Pi<double>();

		pu = cos(angle);
		pv = sin(angle);

		spub = bottomRadius*pu;
		spvb = bottomRadius*pv;
		sput = topRadius*pu;
		spvt = topRadius*pv;

		glNormal3d(-(pu*u[0] + pv*v[0]), -(pu*u[1] + pv*v[1]), -(pu*u[2] + pv*v[2]));

		glVertex3d(originCoord[0] + spub*u[0] + spvb*v[0], originCoord[1] + spub*u[1] + spvb*v[1], originCoord[2] + spub*u[2] + spvb*v[2]);
		glVertex3d(destinationCoord[0] + sput*u[0] + spvt*v[0], destinationCoord[1] + sput*u[1] + spvt*v[1], destinationCoord[2] + sput*u[2] + spvt*v[2]);
	}

	glEnd();

    glBegin(GL_TRIANGLE_FAN);

    for (i = slices; i--; )
    {
        angle = ((double)i / (slices - 1))*2.0*MyMath::Pi<double>();

        pu = cos(angle);
        pv = sin(angle);

        spub = bottomRadius*pu;
        spvb = bottomRadius*pv;

        glNormal3dv(axis);

        glVertex3d(originCoord[0] + spub*u[0] + spvb*v[0], originCoord[1] + spub*u[1] + spvb*v[1], originCoord[2] + spub*u[2] + spvb*v[2]);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    for (i = slices; i--; )
    {
        angle = ((double)i / (slices - 1))*2.0*MyMath::Pi<double>();

        pu = cos(angle);
        pv = sin(angle);

        sput = topRadius*pu;
        spvt = topRadius*pv;

        glNormal3d(-axis[0], -axis[1], -axis[2]);

        glVertex3d(destinationCoord[0] + sput*u[0] + spvt*v[0], destinationCoord[1] + sput*u[1] + spvt*v[1], destinationCoord[2] + sput*u[2] + spvt*v[2]);
    }

    glEnd();
    
    return true;
}

// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool GlHelper::GetViewProjectionMatrix(const double *viewMatrix, const double *projectionMatrix, double *viewProjectionMatrix)
{
    HEALTH_CHECK(!viewMatrix, false);
    HEALTH_CHECK(!projectionMatrix, false);
    HEALTH_CHECK(!viewProjectionMatrix, false);

    viewProjectionMatrix[0] =
    projectionMatrix[ 0]*viewMatrix[ 0] +
    projectionMatrix[ 4]*viewMatrix[ 1] +
    projectionMatrix[ 8]*viewMatrix[ 2] +
    projectionMatrix[12]*viewMatrix[ 3];
    viewProjectionMatrix[4] =
    projectionMatrix[ 0]*viewMatrix[ 4] +
    projectionMatrix[ 4]*viewMatrix[ 5] +
    projectionMatrix[ 8]*viewMatrix[ 6] +
    projectionMatrix[12]*viewMatrix[ 7];
    viewProjectionMatrix[8] =
    projectionMatrix[ 0]*viewMatrix[ 8] +
    projectionMatrix[ 4]*viewMatrix[ 9] +
    projectionMatrix[ 8]*viewMatrix[10] +
    projectionMatrix[12]*viewMatrix[11];
    viewProjectionMatrix[12] =
    projectionMatrix[ 0]*viewMatrix[12] +
    projectionMatrix[ 4]*viewMatrix[13] +
    projectionMatrix[ 8]*viewMatrix[14] +
    projectionMatrix[12]*viewMatrix[15];
    viewProjectionMatrix[1] =
    projectionMatrix[ 1]*viewMatrix[ 0] +
    projectionMatrix[ 5]*viewMatrix[ 1] +
    projectionMatrix[ 9]*viewMatrix[ 2] +
    projectionMatrix[13]*viewMatrix[ 3];
    viewProjectionMatrix[5] =
    projectionMatrix[ 1]*viewMatrix[ 4] +
    projectionMatrix[ 5]*viewMatrix[ 5] +
    projectionMatrix[ 9]*viewMatrix[ 6] +
    projectionMatrix[13]*viewMatrix[ 7];
    viewProjectionMatrix[9] =
    projectionMatrix[ 1]*viewMatrix[ 8] +
    projectionMatrix[ 5]*viewMatrix[ 9] +
    projectionMatrix[ 9]*viewMatrix[10] +
    projectionMatrix[13]*viewMatrix[11];
    viewProjectionMatrix[13] =
    projectionMatrix[ 1]*viewMatrix[12] +
    projectionMatrix[ 5]*viewMatrix[13] +
    projectionMatrix[ 9]*viewMatrix[14] +
    projectionMatrix[13]*viewMatrix[15];
    viewProjectionMatrix[2] =
    projectionMatrix[ 2]*viewMatrix[ 0] +
    projectionMatrix[ 6]*viewMatrix[ 1] +
    projectionMatrix[10]*viewMatrix[ 2] +
    projectionMatrix[14]*viewMatrix[ 3];
    viewProjectionMatrix[6] =
    projectionMatrix[ 2]*viewMatrix[ 4] +
    projectionMatrix[ 6]*viewMatrix[ 5] +
    projectionMatrix[10]*viewMatrix[ 6] +
    projectionMatrix[14]*viewMatrix[ 7];
    viewProjectionMatrix[10] =
    projectionMatrix[ 2]*viewMatrix[ 8] +
    projectionMatrix[ 6]*viewMatrix[ 9] +
    projectionMatrix[10]*viewMatrix[10] +
    projectionMatrix[14]*viewMatrix[11];
    viewProjectionMatrix[14] =
    projectionMatrix[ 2]*viewMatrix[12] +
    projectionMatrix[ 6]*viewMatrix[13] +
    projectionMatrix[10]*viewMatrix[14] +
    projectionMatrix[14]*viewMatrix[15];
    viewProjectionMatrix[3] =
    projectionMatrix[ 3]*viewMatrix[ 0] +
    projectionMatrix[ 7]*viewMatrix[ 1] +
    projectionMatrix[11]*viewMatrix[ 2] +
    projectionMatrix[15]*viewMatrix[ 3];
    viewProjectionMatrix[7] =
    projectionMatrix[ 3]*viewMatrix[ 4] +
    projectionMatrix[ 7]*viewMatrix[ 5] +
    projectionMatrix[11]*viewMatrix[ 6] +
    projectionMatrix[15]*viewMatrix[ 7];
    viewProjectionMatrix[11] =
    projectionMatrix[ 3]*viewMatrix[ 8] +
    projectionMatrix[ 7]*viewMatrix[ 9] +
    projectionMatrix[11]*viewMatrix[10] +
    projectionMatrix[15]*viewMatrix[11];
    viewProjectionMatrix[15] =
    projectionMatrix[ 3]*viewMatrix[12] +
    projectionMatrix[ 7]*viewMatrix[13] +
    projectionMatrix[11]*viewMatrix[14] +
    projectionMatrix[15]*viewMatrix[15];
    
    return true;
}


// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool GlHelper::GetViewProjectionMatrixInverse(const double *viewMatrix, const double *projectionMatrix, double *viewProjectionMatrixInverse)
{
    HEALTH_CHECK(!viewMatrix, false);
    HEALTH_CHECK(!projectionMatrix, false);
    HEALTH_CHECK(!viewProjectionMatrixInverse, false);

    double cache[4][8] = {{0}};
    double m0 = 0,
    m1 = 0,
    m2 = 0,
    m3 = 0,
    s = 0;
    double *r0 = 0,
    *r1 = 0,
    *r2 = 0,
    *r3 = 0;
    
    double viewProjectionMatrix[16] = { 0 };
    
    if (!GetViewProjectionMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix))
    {
        LOG_ERROR();
        
        return false;
    }
    
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
    m1 = r1[0]/r0[0];
    m2 = r2[0]/r0[0];
    m3 = r3[0]/r0[0];
    
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
    m2 = r2[1]/r1[1];
    m3 = r3[1]/r1[1];
    
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
    m3 = r3[2]/r2[2];
    
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
    s = 1.0/r3[3];
    
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;
    
    // Now back substitute row 2
    m2 = r2[3];
    
    s = 1.0/r2[2];
    
    r2[4] = s*(r2[4] - r3[4]*m2);
    r2[5] = s*(r2[5] - r3[5]*m2);
    r2[6] = s*(r2[6] - r3[6]*m2);
    r2[7] = s*(r2[7] - r3[7]*m2);
    
    m1 = r1[3];
    
    r1[4] -= r3[4]*m1;
    r1[5] -= r3[5]*m1;
    r1[6] -= r3[6]*m1;
    r1[7] -= r3[7]*m1;
    
    m0 = r0[3];
    
    r0[4] -= r3[4]*m0;
    r0[5] -= r3[5]*m0;
    r0[6] -= r3[6]*m0;
    r0[7] -= r3[7]*m0;
    
    // Nnow back substitute row 1
    m1 = r1[2];
    
    s = 1.0/r1[1];
    
    r1[4] = s*(r1[4] - r2[4]*m1);
    r1[5] = s*(r1[5] - r2[5]*m1);
    r1[6] = s*(r1[6] - r2[6]*m1);
    r1[7] = s*(r1[7] - r2[7]*m1);
    
    m0 = r0[2];
    
    r0[4] -= r2[4]*m0;
    r0[5] -= r2[5]*m0;
    r0[6] -= r2[6]*m0;
    r0[7] -= r2[7]*m0;
    
    // Now back substitute row 0.
    m0 = r0[1];
    
    s = 1.0/r0[0];
    
    r0[4] = s*(r0[4] - r1[4]*m0);
    r0[5] = s*(r0[5] - r1[5]*m0);
    r0[6] = s*(r0[6] - r1[6]*m0);
    r0[7] = s*(r0[7] - r1[7]*m0);
    
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 0, 0) = r0[4];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 0, 1) = r0[5];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 0, 2) = r0[6];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 0, 3) = r0[7];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 1, 0) = r1[4];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 1, 1) = r1[5];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 1, 2) = r1[6];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 1, 3) = r1[7];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 2, 0) = r2[4];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 2, 1) = r2[5];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 2, 2) = r2[6];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 2, 3) = r2[7];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 3, 0) = r3[4];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 3, 1) = r3[5];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 3, 2) = r3[6];
    GL_HELPER_MATRIX_ACCESS(viewProjectionMatrixInverse, 3, 3) = r3[7];
    
    return true;
}

// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool GlHelper::ProjectVertex(double objx, double objy, double objz, const double *viewMatrix, const double *projectionMatrix, const int *viewport, double *winx, double *winy, double *winz)
{
    double cache[8] = {0};
    
    // Modelview transform
    cache[0] = viewMatrix[ 0]*objx + viewMatrix[ 4]*objy + viewMatrix[ 8]*objz + viewMatrix[12];
    cache[1] = viewMatrix[ 1]*objx + viewMatrix[ 5]*objy + viewMatrix[ 9]*objz + viewMatrix[13];
    cache[2] = viewMatrix[ 2]*objx + viewMatrix[ 6]*objy + viewMatrix[10]*objz + viewMatrix[14];
    cache[3] = viewMatrix[ 3]*objx + viewMatrix[ 7]*objy + viewMatrix[11]*objz + viewMatrix[15];
    
    // Projection transform, the final row of projection matrix is
    // always [0 0 -1 0] so we optimize for that.
    cache[4] = projectionMatrix[ 0]*cache[0] + projectionMatrix[ 4]*cache[1] + projectionMatrix[ 8]*cache[2] + projectionMatrix[12]*cache[3];
    cache[5] = projectionMatrix[ 1]*cache[0] + projectionMatrix[ 5]*cache[1] + projectionMatrix[ 9]*cache[2] + projectionMatrix[13]*cache[3];
    cache[6] = projectionMatrix[ 2]*cache[0] + projectionMatrix[ 6]*cache[1] + projectionMatrix[10]*cache[2] + projectionMatrix[14]*cache[3];
    cache[7] = -cache[2];
    
    // The result normalizes between -1 and 1
    if (cache[7] == 0)
        return false;
    
    cache[7] = 1.0/cache[7];
    
    // Perspective division
    cache[4] *= cache[7];
    cache[5] *= cache[7];
    cache[6] *= cache[7];
    
    // Window coordinates
    // Map x, y to range 0-1
    (*winx) = (cache[4] * 0.5 + 0.5) * (double)viewport[2] + (double)viewport[0];
    (*winy) = (cache[5] * 0.5 + 0.5) * (double)viewport[3] + (double)viewport[1];
    // This is only correct when glDepthRange(0, 1)
    (*winz) = (1.0 + cache[6]) * 0.5;
    
    return true;
}

// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool GlHelper::ProjectVertex(const double *objectCoordinate, const double *viewMatrix, const double *projectionMatrix, const int *viewport, double *windowCoordinate)
{
    return GlHelper::ProjectVertex(objectCoordinate[0], objectCoordinate[1], objectCoordinate[2], viewMatrix, projectionMatrix, viewport, windowCoordinate, windowCoordinate + 1, windowCoordinate + 2);
}


// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool GlHelper::UnProjectVertex(double winx, double winy, double winz, const double *viewMatrix, const double *projectionMatrix, const int *viewport, double *objx, double *objy, double *objz)
{
    HEALTH_CHECK(!viewMatrix, false);
    HEALTH_CHECK(!projectionMatrix, false);
    HEALTH_CHECK(!viewport, false);
    HEALTH_CHECK(!objx, false);
    HEALTH_CHECK(!objy, false);
    HEALTH_CHECK(!objz, false);

    double inputVertex[4] = { 0 },
    outputVertex[4] = { 0 };
    
    double viewProjectionMatrixInverse[16] = { 0 };
    
    if (!GetViewProjectionMatrixInverse(viewMatrix, projectionMatrix, viewProjectionMatrixInverse))
    {
        LOG_ERROR();
        
        return false;
    }

    // BUG: (30-Nov-2015) The address of the array next will always be evaluated as true. The test is meaningless
    //HEALTH_CHECK(!viewProjectionMatrixInverse, false);
    
    inputVertex[0] = winx;
    inputVertex[1] = winy;
    inputVertex[2] = 2.0 * winz - 1.0;
    inputVertex[3] = 1.0;
    
    GlHelper::TransformVertex4(viewProjectionMatrixInverse, inputVertex, outputVertex);
    
    if (outputVertex[3] == 0)
    {
        LOG_ERROR();
        
        return false;
    }
    
    outputVertex[3] = 1.0 / outputVertex[3];
    
    (*objx) = outputVertex[0]*outputVertex[3];
    (*objy) = outputVertex[1]*outputVertex[3];
    (*objz) = outputVertex[2]*outputVertex[3];
    
    return true;
}

// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
template <>
bool  GlHelper::UnProjectVertex(const double *windowCoordinate, const double *viewMatrix, const double *projectionMatrix, const int *viewport, double *objectCoordinate)
{
    HEALTH_CHECK(!windowCoordinate, false);
    HEALTH_CHECK(!viewMatrix, false);
    HEALTH_CHECK(!projectionMatrix, false);
    HEALTH_CHECK(!viewport, false);
    HEALTH_CHECK(!objectCoordinate, false);
    
    return GlHelper::UnProjectVertex(windowCoordinate[0], windowCoordinate[1], windowCoordinate[2], viewMatrix, projectionMatrix, viewport, objectCoordinate, objectCoordinate + 1, objectCoordinate + 2);
}

/**
*/
template <>
bool GlHelper::UnProject(int i, int j, double& x, double& y, double& z)
{
    GLdouble viewMatrix[16] = { 0 },
		projectionMatrix[16] = { 0 };
	GLint viewport[4] = {0};
	std::vector<double> depthArray;

	glGetDoublev(GL_MODELVIEW_MATRIX, viewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

	glGetIntegerv(GL_VIEWPORT, viewport);

	depthArray.assign(9, 0);

	glReadPixels(i - 1, j - 1, 3, 3, GL_DEPTH_COMPONENT, GL_DOUBLE, &depthArray[0]);

    if (MyMath::IsClose(depthArray[0], 1.0, 1.0e-4))
        return false;

	std::sort(depthArray.begin(), depthArray.end());

	GlHelper::UnProjectVertex((double)i, (double)j, depthArray[depthArray.size()/2], viewMatrix, projectionMatrix, viewport, &x, &y, &z);

    return true;
}

/**
 */
template <>
bool GlHelper::Translate(double *matrix, double x, double y, double z)
{
    HEALTH_CHECK(!matrix, false);
    
    double T[16];
    
    T[ 0] = T[ 5] = T[10] = T[15] = 1;
    T[ 1] = T[ 2] = T[ 3] = T[ 4] = 0;
    T[ 6] = T[ 7] = T[ 8] = T[ 9] = 0;
    T[11] = 0;
    T[12] = x;
    T[13] = y;
    T[14] = z;
    
    // TODO: (25-Jan-2015)
    //multiMatrix(matrix);
    
    return true;
}

// TODO: (25-Jan-2015) Test.
template <>
bool GlHelper::LookAt(double eyeX, double eyeY, double eyeZ, double lookAtX, double lookAtY, double lookAtZ, double upX, double upY, double upZ, double *matrix)
{
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    
    glLoadIdentity();
    
    gluLookAt(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
    
    glGetTv(GL_MODELVIEW_MATRIX, matrix);
    
    glPopMatrix();
    
    return true;
}

// TODO: (25-Jan-2015) Test.
template <>
bool GlHelper::LookAt2(double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ, double *matrix)
{
	HEALTH_CHECK(!matrix, false);

	/* Make rotation matrix */
	double x[3] = { 0 }, 
		y[3] = { 0 }, 
		z[3] = { 0 };

	/* Z vector */
	z[0] = eyeX - centerX;
	z[1] = eyeY - centerY;
	z[2] = eyeZ - centerZ;

	double mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);

	if (mag) 
	{
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = upX;
	y[1] = upY;
	y[2] = upZ;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* cross product gives area of parallelogram, which is < 1.0 for
	* non-perpendicular unit-length vectors; so normalize x, y here
	*/
	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);

	if (mag) 
	{
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}

	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);

	if (mag) 
	{
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}

#define M(row,col)  matrix[col * 4 + row]
	M(0, 0) = x[0];  M(0, 1) = x[1];  M(0, 2) = x[2];  M(0, 3) = 0.0;
	M(1, 0) = y[0];  M(1, 1) = y[1];  M(1, 2) = y[2];  M(1, 3) = 0.0;
	M(2, 0) = z[0];  M(2, 1) = z[1];  M(2, 2) = z[2];  M(2, 3) = 0.0;
	M(3, 0) = 0.0;   M(3, 1) = 0.0;   M(3, 2) = 0.0;   M(3, 3) = 1.0;
#undef M

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glLoadIdentity();

	glMultMatrixd(matrix);
	/* Translate Eye to Origin */
	glTranslated(-eyeX, -eyeY, -eyeZ);

	glGetTv(GL_MODELVIEW_MATRIX, matrix);

	glPopMatrix();

	return true;
}

/**
 */
bool GlHelper::Draw(const CVertexPool& mesh)
{
    if (mesh.GetColorCount())
    {
		glColorPointerT(mesh.GetColorSize(), 0, mesh.GetColorArray());
        
        glEnableClientState(GL_COLOR_ARRAY);
    }
    
	if (mesh.GetNormalCount())
    {
		glNormalPointerT(0, mesh.GetNormalArray());
        
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    
	if (mesh.GetTexCoordCount())
    {
		glTexCoordPointerT(mesh.GetTexCoordSize(), 0, mesh.GetTexCoordArray());
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
	if (mesh.GetVertexCount())
    {
		glVertexPointerT(mesh.GetVertexSize(), 0, mesh.GetVertexArray());
        
        glEnableClientState(GL_VERTEX_ARRAY);
    }
    
	if (mesh.GetElementCount())
		glDrawElements(mesh.GetMode(), (GLsizei)mesh.GetElementCount(), GL_UNSIGNED_INT, mesh.GetElementArray());
    
	if (mesh.GetColorCount())
        glDisableClientState(GL_COLOR_ARRAY);
    
	if (mesh.GetNormalCount())
        glDisableClientState(GL_NORMAL_ARRAY);
    
	if (mesh.GetTexCoordCount())
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
	if (mesh.GetVertexCount())
        glDisableClientState(GL_VERTEX_ARRAY);
    
    return true;
}

bool GlHelper::Draw(const boost::shared_ptr<CVertexPool>& mesh)
{
	HEALTH_CHECK(!mesh, false);

	if (!Draw(*mesh))
	{
		LOG_ERROR();

		return false;
	}

	return true;
}

/**
*/
void GlHelper::EnableLight(const LIGHT_POINT& lightPoint)
{
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();

    glLightfv(GL_LIGHT0, GL_POSITION, lightPoint.m_lightPosition);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glPopMatrix();

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightPoint.m_ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightPoint.m_diffuseLight);

    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);

    glEnable(GL_NORMALIZE);

    glEnable(GL_COLOR_MATERIAL);
}

/**
*/
void GlHelper::DisableLight()
{
    glDisable(GL_COLOR_MATERIAL);

    glDisable(GL_NORMALIZE);

    glDisable(GL_LIGHTING);
}

// (BEGIN OF) TESTING: (13-Jun-2016) 
GlHelper::PUSH_MATRIX_HELPER::PUSH_MATRIX_HELPER(bool loadIdentity)
{
    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    if (loadIdentity)
        glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    if (loadIdentity)
        glLoadIdentity();
}
// (END OF) TESTING: (13-Jun-2016) 

// (BEGIN OF) TESTING: (13-Jun-2016) 
GlHelper::PUSH_MATRIX_HELPER::~PUSH_MATRIX_HELPER()
{
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glPopMatrix();
}
// (END OF) TESTING: (13-Jun-2016) 

// (BEGIN OF) TESTING: (19-Jul-2016) 
GlHelper::PUSH_COLOR_HELPER::PUSH_COLOR_HELPER()
{
    glGetFloatv(GL_CURRENT_COLOR, m_currentColor);
}
// (END OF) TESTING: (19-Jul-2016) 

// (BEGIN OF) TESTING: (19-Jul-2016) 
GlHelper::PUSH_COLOR_HELPER::~PUSH_COLOR_HELPER()
{
    glColor4fv(m_currentColor);
}
// (END OF) TESTING: (19-Jul-2016) 


/**
*/
my::CVector3<double> GlHelper::GetOpticalAxis()
{
    GLdouble viewMatrix[16] = { 0 };

    glGetDoublev(GL_MODELVIEW_MATRIX, viewMatrix);

    // x0 x1 x2 t0    0  4  8 12    00 01 02 03
    // y0 y1 y2 t1    1  5  9 13    10 11 12 13
    // z0 z1 z2 t2    2  6 10 14    20 21 22 23
    //  0  0  0  1    3  7 11 15    30 31 32 33

    return -my::CVector3<double>(viewMatrix[2], viewMatrix[6], viewMatrix[10]).Normalize();
}

/**
*/
void GlHelper::ExportFrontBufferAsPpm(std::string ppmFileName, int windowWidth, int windowHeight)
{
    if (ppmFileName.find(".ppm") == std::string::npos)
        ppmFileName += ".ppm";

    windowWidth -= windowWidth % 2;

    my::int32 pixelCount = 3 * windowWidth * windowHeight;
    
    std::vector<unsigned char> pixelArray(pixelCount, 0);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glReadBuffer(GL_FRONT);
    
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixelArray.data());

    int i, j;
    FILE *ppmFileStream = fopen(ppmFileName.c_str(), "wb"); /* b - binary mode */
    fprintf(ppmFileStream, "P6 %d %d 255", windowWidth, windowHeight);
    unsigned char *pixelBuffer = pixelArray.data();
        for (j = (windowHeight - 1); j >= 0; --j)
        {
            for (i = 0; i < windowWidth; ++i)
            {
                static unsigned char color[3];
                color[1] = pixelBuffer[3 * i + 3 * j*windowWidth];  /* red */
                color[2] = pixelBuffer[3 * i + 3 * j*windowWidth + 1];  /* green */
                color[0] = pixelBuffer[3 * i + 3 * j*windowWidth + 2];  /* blue */
                (void)fwrite(color, 1, 3, ppmFileStream);
            }
        }

        fclose(ppmFileStream);
}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//	void MultiplyTransformMatrices(const float *firstMatrix, const float *secondMatrix, float *productMatrix)
//	{
//		int i;
//
//		for (i=0; i<4; i++) 
//		{
//			MATRIX_ELEMENT(productMatrix, i, 0) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 0) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 0) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 0) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 0);
//			MATRIX_ELEMENT(productMatrix, i, 1) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 1) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 1) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 1) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 1);
//			MATRIX_ELEMENT(productMatrix, i, 2) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 2) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 2) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 2) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 2);
//			MATRIX_ELEMENT(productMatrix, i, 3) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 3) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 3) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 3) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 3);
//		}
//	}
//};

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//void CMyGLH::DrawLine(float *originCoord, float *destinationCoord, int widthMax)
//{
//	float currentColor[4];
//	int i;
//
//	glGetFloatv(GL_CURRENT_COLOR, currentColor);
//
//	glDepthMask(GL_FALSE);
//
//	glEnable(GL_LINE_SMOOTH);
//
//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//
//	for (i=widthMax; i>0; i--)
//	{
//		glLineWidth((float)i*i);
//
//		glColor3f(max(currentColor[0] - ((float)i/widthMax), 0.0f), max(currentColor[1] - ((float)i/widthMax), 0.0f), max(currentColor[2] - ((float)i/widthMax), 0.0f));
//
//		glBegin(GL_LINES);
//
//		glVertex3fv(originCoord);
//		glVertex3fv(destinationCoord);
//
//		glEnd();
//	}
//
//	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
//
//	glDisable(GL_LINE_SMOOTH);
//
//	glDepthMask(GL_TRUE);
//
//	glColor4fv(currentColor);
//}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//void CMyGLH::TransformVertex3(const float *transformMatrix, const float *inputVertex, float *outputVertex)
//{
//   outputVertex[0] = MATRIX_ELEMENT(transformMatrix, 0, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 0, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 0, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 0, 3);
//   outputVertex[1] = MATRIX_ELEMENT(transformMatrix, 1, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 1, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 1, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 1, 3);
//   outputVertex[2] = MATRIX_ELEMENT(transformMatrix, 2, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 2, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 2, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 2, 3);
//   outputVertex[3] = MATRIX_ELEMENT(transformMatrix, 3, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 3, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 3, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 3, 3);
//}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//void CMyGLH::MultiplyTransformMatrix4x4(const float *firstMatrix, const float *secondMatrix, float *productMatrix)
//{
//	int i;
//
//	for (i=0; i<4; i++) 
//	{
//		MATRIX_ELEMENT(productMatrix, i, 0) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 0) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 0) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 0) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 0);
//		MATRIX_ELEMENT(productMatrix, i, 1) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 1) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 1) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 1) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 1);
//		MATRIX_ELEMENT(productMatrix, i, 2) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 2) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 2) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 2) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 2);
//		MATRIX_ELEMENT(productMatrix, i, 3) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 3) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 3) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 3) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 3);
//	}
//}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//void CMyGLH::TransposeTransformMatrix(float *transformMatrix)
//{
//	int i,
//		j;
//	float tmp;
//
//	for (i=0; i<4; i++)
//		for (j=0; j<i; j++)
//		{
//			tmp = transformMatrix[i*4 + j];
//			transformMatrix[i*4 + j] = transformMatrix[j*4 + i];
//			transformMatrix[j*4 + i] = tmp;
//		}
//}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//bool CMyGLH::ProjectVertex3(const float viewMatrix[16], const float projectionMatrix[16], const int viewportCoord[4], const float *inputVertex, float *outputVertex)
//{
//	float homogeneousInputVertex[4], 
//		homogeneousOutputVertex[4];
//
//	homogeneousInputVertex[0] = inputVertex[0];
//	homogeneousInputVertex[1] = inputVertex[1];
//	homogeneousInputVertex[2] = inputVertex[2];
//	homogeneousInputVertex[3] = 1.0f;
//	
//	TransformVertex4(viewMatrix, homogeneousInputVertex, homogeneousOutputVertex);
//	TransformVertex4(projectionMatrix, homogeneousOutputVertex, homogeneousInputVertex);
//
//	if (homogeneousInputVertex[3] == 0.0f)
//	{
//		LOG_ERROR();
//
//		return false;
//	}
//
//	homogeneousInputVertex[0] /= homogeneousInputVertex[3];
//	homogeneousInputVertex[1] /= homogeneousInputVertex[3];
//	homogeneousInputVertex[2] /= homogeneousInputVertex[3];
//
//	outputVertex[0] = viewportCoord[0] + (1.0f + homogeneousInputVertex[0])*viewportCoord[2]/2.0f;
//	outputVertex[1] = viewportCoord[1] + (1.0f + homogeneousInputVertex[1])*viewportCoord[3]/2.0f;
//	
//	outputVertex[2] = (1.0f + homogeneousInputVertex[2])/2.0f;
//	
//	return true;
//}

// DEBUG ONLY! (25-Jan-2015) Legacy code to be reused.
//{
//	float inverseTransformMatrix[16], 
//		transformMatrix[16],
//		homogeneousInputVertex[4], 
//		homogeneousOutputVertex[4];
//
//	homogeneousInputVertex[0] = (inputVertex[0] - viewportCoord[0])*2.0f/viewportCoord[2] - 1.0f;
//	homogeneousInputVertex[1] = (inputVertex[1] - viewportCoord[1])*2.0f/viewportCoord[3] - 1.0f;
//	homogeneousInputVertex[2] = 2.0f*inputVertex[2] - 1.0f;
//	homogeneousInputVertex[3] = 1.0f;
//
//	MultiplyTransformMatrix4x4(projectionMatrix, viewMatrix, transformMatrix);
//
//	InvertTransformMatrix(transformMatrix, inverseTransformMatrix);
//
//	TransformVertex4(inverseTransformMatrix, homogeneousInputVertex, homogeneousOutputVertex);
//
//	if (homogeneousOutputVertex[3] == 0.0f)
//	{
//		LOG_ERROR();
//
//		return false;
//	}
//
//	outputVertex[0] = homogeneousOutputVertex[0]/homogeneousOutputVertex[3];
//	outputVertex[1] = homogeneousOutputVertex[1]/homogeneousOutputVertex[3];
//	outputVertex[2] = homogeneousOutputVertex[2]/homogeneousOutputVertex[3];
//
//	return true;
//}

