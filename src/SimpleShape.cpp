// Zoe - 2024
#include "SimpleShape.h"
#include <cmath>

// Define PI for circle calculations
#define PI 3.14159265358979323846

// Define global variables for window dimensions
int SimpleShape::windowWidth = 0;
int SimpleShape::windowHeight = 0;

void SimpleShape::setWindowDimensions(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}
void setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void setProjectionMatrix(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Aspect ratio calculation
    float aspectRatio = static_cast<float>(width) / height;
    
    // Orthographic projection: left, right, bottom, top, near, far
    float left = -aspectRatio;
    float right = aspectRatio;
    float bottom = -1.0f;
    float top = 1.0f;
    float near = -1.0f;
    float far = 1.0f;
    
    glOrtho(left, right, bottom, top, near, far);
    
    glMatrixMode(GL_MODELVIEW);
}

#define APP_VIRTUAL_TO_NATIVE_COORDS(x, y) \
    { \
        x = (x / windowWidth) * 2.0f - 1.0f; \
        y = (y / windowHeight) * 2.0f - 1.0f; \
    }

void SimpleShape::simplePoint(Vec3 rgb, Vec3 pt) const {
    glBegin(GL_POINTS);
    glColor3f(rgb.x, rgb.y, rgb.z);
    APP_VIRTUAL_TO_NATIVE_COORDS(pt.x, pt.y);
    glVertex3f(pt.x, pt.y, pt.z);
    glEnd();
}

void SimpleShape::simpleLine(Vec3 rgb, Vec3 startPt, Vec3 endPt, bool fill) const {
    glBegin(fill ? GL_LINES : GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    APP_VIRTUAL_TO_NATIVE_COORDS(startPt.x, startPt.y);
    APP_VIRTUAL_TO_NATIVE_COORDS(endPt.x, endPt.y);
    glVertex3f(startPt.x, startPt.y, startPt.z);
    glVertex3f(endPt.x, endPt.y, endPt.z);
    glEnd();
}

void SimpleShape::simpleLineStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    glBegin(GL_LINE_STRIP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    for (const auto& pt : points) {
        float x = pt.x; float y = pt.y;
        APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
        glVertex3f(x, y, pt.z);
    }
    glEnd();
}

void SimpleShape::simpleLineLoop(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    glBegin(GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    for (const auto& pt : points) {
        float x = pt.x; float y = pt.y;
        APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
        glVertex3f(x, y, pt.z);
    }
    glEnd();
}

void SimpleShape::simpleTriangle(Vec3 rgb, Vec3 pt1, Vec3 pt2, Vec3 pt3, bool fill) const {
    glBegin(fill ? GL_TRIANGLES : GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    APP_VIRTUAL_TO_NATIVE_COORDS(pt1.x, pt1.y);
    APP_VIRTUAL_TO_NATIVE_COORDS(pt2.x, pt2.y);
    APP_VIRTUAL_TO_NATIVE_COORDS(pt3.x, pt3.y);
    glVertex3f(pt1.x, pt1.y, pt1.z);
    glVertex3f(pt2.x, pt2.y, pt2.z);
    glVertex3f(pt3.x, pt3.y, pt3.z);
    glEnd();
}

void SimpleShape::simpleTriangleStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    // TODO: Implement triangle strip rendering
}

void SimpleShape::simpleTriangleFan(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    // TODO: Implement triangle fan rendering
}

void SimpleShape::simpleQuad(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    for (const auto& pt : points) {
        float x = pt.x; float y = pt.y;
        APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
        glVertex3f(x, y, pt.z);
    }
    glEnd();
}

void SimpleShape::simpleQuadStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    if (fill) {
        glBegin(GL_QUAD_STRIP);
        glColor3f(rgb.x, rgb.y, rgb.z);
        for (const auto& pt : points) {
            float x = pt.x; float y = pt.y;
            APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
            glVertex3f(x, y, pt.z);
        }
        glEnd();
    }
}

void SimpleShape::simplePolygon(Vec3 rgb, const std::vector<Vec3>& points, bool fill) const {
    if (fill) {
        glBegin(GL_POLYGON);
        glColor3f(rgb.x, rgb.y, rgb.z);
        for (const auto& pt : points) {
            float x = pt.x; float y = pt.y;
            APP_VIRTUAL_TO_NATIVE_COORDS(x, y);
            glVertex3f(x, y, pt.z);
        }
        glEnd();
    }
}

void SimpleShape::simpleCircle(Vec3 rgb, Vec3 center, float radius, int segments, bool fill) const {
    glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);


    glColor3f(rgb.x, rgb.y, rgb.z);


    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);

        float newX = center.x + x;
        float newY = center.y + y;

        APP_VIRTUAL_TO_NATIVE_COORDS(newX, newY);

        glVertex3f(newX, newY, center.z);
    }

    glEnd();
}

void SimpleShape::simpleCapsule(Vec3 rgb, Vec3 center, float width, float height, int segments, bool fill) const {

    float radius = width / 2.0f;
    float halfHeight = (height - width) / 2.0f; // Height minus the semicircles
    

    std::vector<Vec3> quadPoints = {
        Vec3(center.x - radius, center.y - halfHeight, center.z),
        Vec3(center.x + radius, center.y - halfHeight, center.z),
        Vec3(center.x + radius, center.y + halfHeight, center.z),
        Vec3(center.x - radius, center.y + halfHeight, center.z)
    };
    simpleQuad(rgb, quadPoints, fill);
    

    glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    for (int i = 0; i <= segments / 2; ++i) {
        float theta = PI * static_cast<float>(i) / static_cast<float>(segments / 2);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        float newX = center.x + x;
        float newY = center.y + halfHeight + y;

        APP_VIRTUAL_TO_NATIVE_COORDS(newX, newY);
        glVertex3f(newX, newY, center.z);
    }
    glEnd();
    
  
    glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
    glColor3f(rgb.x, rgb.y, rgb.z);
    for (int i = 0; i <= segments / 2; ++i) {
        float theta = PI * static_cast<float>(i) / static_cast<float>(segments / 2);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        float newX = center.x + x;
        float newY = center.y - halfHeight - y;

        APP_VIRTUAL_TO_NATIVE_COORDS(newX, newY);
        glVertex3f(newX, newY, center.z);
    }
    glEnd();
}

