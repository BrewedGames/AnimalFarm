// Zoe Somji - 2024
#pragma once
#include <glew.h>
#include <Vector.h>
#include <vector>


using namespace MATH;
class SimpleShape {
public:
    // Functions to draw various shapes
    void simplePoint(Vec3 rgb, Vec3 pt) const;
    void simpleLine(Vec3 rgb, Vec3 startPt, Vec3 endPt, bool fill = true) const;
    void simpleLineStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleLineLoop(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleTriangle(Vec3 rgb, Vec3 pt1, Vec3 pt2, Vec3 pt3, bool fill = true) const;
    void simpleTriangleStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleTriangleFan(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleQuad(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleQuadStrip(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simplePolygon(Vec3 rgb, const std::vector<Vec3>& points, bool fill = true) const;
    void simpleCircle(Vec3 rgb, Vec3 center, float radius, int segments, bool fill) const;
    void simpleCapsule(Vec3 rgb, Vec3 center, float width, float height, int segments, bool fill) const;

    // Set window dimensions for coordinate conversion
    static void setWindowDimensions(int width, int height);
    
private:
    // Window dimensions for coordinate conversion
    static int windowWidth;
    static int windowHeight;
};
