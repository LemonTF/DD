#include <iostream>
#include <geos_c.h>
#include <geos.h>
#include <geos/algorithm/Angle.h>
#include <geos/algorithm/Centroid.h>
#include <geos/algorithm/InteriorPointLine.h>

enum abc{
    ABC=0X0,
    ABF,
    ABE,
    ABD,
};
using namespace std;
using namespace geos;
using namespace geos::geom;
static const GeometryFactory* g_factory = geos::geom::GeometryFactory::getDefaultInstance(); //全局对象,所有的图形都由此对象创建

//单点的创建
Point* createGeosPoint(double x, double y)
{
	Coordinate pt(x, y);    //坐标
	Point* p = g_factory->createPoint(pt);
	return p;
}

//多点的创建
MultiPoint* createGeosMultiPoint(double x, double y, double offset)
{
	CoordinateArraySequence *cas = new CoordinateArraySequence(); //构建点序列
	std::vector<Coordinate> points;
	points.push_back(Coordinate(x, y));
	points.push_back(Coordinate(x + offset, y));
	points.push_back(Coordinate(x + 2 * offset, y + offset));
	points.push_back(Coordinate(x + 3 * offset, y + 2 * offset));
	MultiPoint* Mp = g_factory->createMultiPoint(points);
    std::cout <<"mulitiPoint:"<<Mp->toString()<<std::endl;
	return Mp;
}

//非闭合线
LineString* createGeosLine(double x, double y, double offset)
{
	CoordinateArraySequence *cas = new CoordinateArraySequence(); //构建点序列
	cas->add(Coordinate(x, y));
	cas->add(Coordinate(x, y + offset));
	cas->add(Coordinate(x + offset, y + offset));
	cas->add(Coordinate(x + offset, y + 2 * offset));
	cas->add(Coordinate(x + 2 * offset, y + 2 * offset));
	LineString *ls = g_factory->createLineString(cas);
	return ls;
}

//创建一条环线，与线的区别就是环线是闭合的。即第一个点和最后一点重合
LinearRing* createGeosRing(double x, double y, double offset)
{
	CoordinateArraySequence *cas = new CoordinateArraySequence(); //构建点序列
	cas->add(Coordinate(x, y));
	cas->add(Coordinate(x, y + offset));
	cas->add(Coordinate(x + offset, y + offset));
	cas->add(Coordinate(x + offset, y + 2 * offset));
	cas->add(Coordinate(x + 2 * offset, y + 2 * offset));
	cas->add(Coordinate(x + 2 * offset, y));
	cas->add(Coordinate(x, y)); //与第一个点相等
	LinearRing *lr = g_factory->createLinearRing(cas);
    std::cout <<"linearRing:"<< lr->toString()<<std::endl;
	return lr;
}

//创建一个多边形，如果多边形内部没有孔洞实际上与环线是一样的
Polygon* createGeosPolygon(double x, double y, double offset)
{
	LinearRing *lr = createGeosRing(x, y, offset);
	Polygon *poly = g_factory->createPolygon(lr, NULL); //如果多边形中间没有孔洞，第二个参数设为NULL
	return poly;
}

int main()
{
	Point* point = createGeosPoint(10, 20);
	std::string typeStr = point->getGeometryType();
	cout << "创建的点类型:" << typeStr << endl;

	MultiPoint* multiPoint = createGeosMultiPoint(10, 20,5);
	int count = multiPoint->getNumPoints();
	cout << "多点的个数:" << count << endl;

	LineString *ls = createGeosRing(10, 10, 5);
	cout << "线条点数：" << ls->getNumPoints() << " 线条长度：" << ls->getLength() << "  to_string:"<<ls->toString()<<endl;

	Polygon *poly = createGeosPolygon(10, 10, 5);
	cout << "多边形面积:" << poly->getArea() << endl;

    Coordinate p(4,4);
    std::cout<<"Angle:" << geos::algorithm::Angle::angle(p)<<std::endl;
    {
        CoordinateArraySequence * cas=new CoordinateArraySequence();
        std::vector<Coordinate> vc;
        vc.emplace_back(0,0);
        vc.emplace_back(2,2);
        vc.emplace_back(4,0);
        vc.emplace_back(6,2);
        vc.emplace_back(8,0);
        cas->setPoints(vc);
        std::cout << cas->toString()<<std::endl;
        LineString * ls=g_factory->createLineString(cas);
        std::cout <<"lineString:: "<<ls->toString()<<std::endl;
        std::cout <<"lineString:: "<<ls->toText()<<std::endl;
        //获取几何图形的中心
        //不一定在线上
        geos::algorithm::Centroid cd(*ls);
        //一定是一个顶点或者端点
        geos::algorithm::InteriorPointLine ipl(ls);
        geos::geom::Coordinate c;
        cd.getCentroid(c);
        std::cout << c.toString()<<std::endl;
        ipl.getInteriorPoint(c);
        std::cout << c.toString()<<std::endl;
        {
            geos::io::WKTReader reader;
            std::string s("MULTILINESTRING((117.02349 36.78219,117.02516 36.78215,117.02536 36.78215),(117.02536 36.78215,117.02672 36.78211),(117.02672 36.78211,117.027 36.78209),(117.00676 36.7825,117.00885 36.78249,117.00978 36.78246),(117.00978 36.78246,117.01355 36.7824,117.01467 36.78237),(117.01467 36.78237,117.01558 36.78234),(117.01558 36.78234,117.017 36.78231))");
            //Geometry *gm=reader.read(s);
            auto gm=reader.read(s);
            geos::algorithm::InteriorPointLine ipl(gm.get());
            geos::geom::Coordinate c;
            ipl.getInteriorPoint(c);
            std::cout << "test_:("<<c.toString()<<")"<<std::endl;
            std::cout << c.x<<","<<c.y<<std::endl;
        }

    }
    std::cout << sizeof(abc)<<std::endl;

    return 0;
}
