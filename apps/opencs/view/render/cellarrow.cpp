
#include "cellarrow.hpp"

#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>

#include "elements.hpp"

CSVRender::CellArrowTag::CellArrowTag (CellArrow *arrow)
: TagBase (Element_CellArrow), mArrow (arrow)
{}

CSVRender::CellArrow *CSVRender::CellArrowTag::getCellArrow() const
{
    return mArrow;
}


void CSVRender::CellArrow::adjustTransform()
{
    // position
    const int cellSize = 8192;
    const int offset = cellSize / 2 + 800;

    int x = mCoordinates.getX()*cellSize + cellSize/2;
    int y = mCoordinates.getY()*cellSize + cellSize/2;

    float xr = 0;
    float yr = 0;
    float zr = 0;

    float angle = osg::DegreesToRadians (90.0f);

    switch (mDirection)
    {
        case Direction_North: y += offset; xr = -angle; zr = angle; break;
        case Direction_West: x -= offset; yr = -angle; break;
        case Direction_South: y -= offset; xr = angle; zr = angle; break;
        case Direction_East: x += offset; yr = angle; break;
    };

    mBaseNode->setPosition (osg::Vec3f (x, y, 0));

    // orientation
    osg::Quat xr2 (xr, osg::Vec3f (1,0,0));
    osg::Quat yr2 (yr, osg::Vec3f (0,1,0));
    osg::Quat zr2 (zr, osg::Vec3f (0,0,1));
    mBaseNode->setAttitude (zr2*yr2*xr2);
}

void CSVRender::CellArrow::buildShape()
{
    osg::ref_ptr<osg::Geometry> geometry (new osg::Geometry);

    const int arrowWidth = 4000;
    const int arrowLength = 1500;
    const int arrowHeight = 500;

    osg::Vec3Array *vertices = new osg::Vec3Array;
    for (int i2=0; i2<2; ++i2)
        for (int i=0; i<2; ++i)
        {
            float height = i ? -arrowHeight/2 : arrowHeight/2;
            vertices->push_back (osg::Vec3f (height, -arrowWidth/2, 0));
            vertices->push_back (osg::Vec3f (height, arrowWidth/2, 0));
            vertices->push_back (osg::Vec3f (height, 0, arrowLength));
        }

    geometry->setVertexArray (vertices);

    osg::DrawElementsUShort *primitives = new osg::DrawElementsUShort (osg::PrimitiveSet::TRIANGLES, 0);

    // top
    primitives->push_back (0);
    primitives->push_back (1);
    primitives->push_back (2);

    // bottom
    primitives->push_back (5);
    primitives->push_back (4);
    primitives->push_back (3);

    // back
    primitives->push_back (3+6);
    primitives->push_back (4+6);
    primitives->push_back (1+6);

    primitives->push_back (3+6);
    primitives->push_back (1+6);
    primitives->push_back (0+6);

    // sides
    primitives->push_back (0+6);
    primitives->push_back (2+6);
    primitives->push_back (5+6);

    primitives->push_back (0+6);
    primitives->push_back (5+6);
    primitives->push_back (3+6);

    primitives->push_back (4+6);
    primitives->push_back (5+6);
    primitives->push_back (2+6);

    primitives->push_back (4+6);
    primitives->push_back (2+6);
    primitives->push_back (1+6);

    geometry->addPrimitiveSet (primitives);

    osg::Vec4Array *colours = new osg::Vec4Array;

    for (int i=0; i<6; ++i)
        colours->push_back (osg::Vec4f (1.0f, 0.0f, 0.0f, 1.0f));
    for (int i=0; i<6; ++i)
        colours->push_back (osg::Vec4f (0.8f, (i==2 || i==5) ? 0.6f : 0.4f, 0.0f, 1.0f));

    geometry->setColorArray (colours, osg::Array::BIND_PER_VERTEX);

    geometry->getOrCreateStateSet()->setMode (GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::Geode> geode (new osg::Geode);
    geode->addDrawable (geometry);

    mBaseNode->addChild (geode);
}

CSVRender::CellArrow::CellArrow (osg::Group *cellNode, Direction direction,
    const CSMWorld::CellCoordinates& coordinates)
: mDirection (direction), mParentNode (cellNode), mCoordinates (coordinates)
{
    mBaseNode = new osg::PositionAttitudeTransform;

    mBaseNode->setUserData (new CellArrowTag (this));

    mParentNode->addChild (mBaseNode);

    // 0x1 reserved for separating cull and update visitors
    mBaseNode->setNodeMask (Element_CellArrow<<1);

    adjustTransform();
    buildShape();
}

CSVRender::CellArrow::~CellArrow()
{
    mParentNode->removeChild (mBaseNode);
}

CSMWorld::CellCoordinates CSVRender::CellArrow::getCoordinates() const
{
    return mCoordinates;
}

CSVRender::CellArrow::Direction CSVRender::CellArrow::getDirection() const
{
    return mDirection;
}