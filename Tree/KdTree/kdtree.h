#ifndef KdTree_h
#define KdTree_h

#include <vector>

/*2 Dimensional*/
struct Point
{
	int x, y;
	Point(int x = 0, int y = 0)
		: x(x), y(y)
	{

	}
	bool operator == (const Point &p) const
	{
		return x == p.x && y == p.y;
	}
	static double EuclideanDistance(const Point &p1, const Point &p2)
	{
		double d = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
		return sqrt(d);
	}
};

class KdTreeNode
{
public:
	KdTreeNode(const Point &p);
	~KdTreeNode();
	void setPoint(const Point& p);
	const Point& point() const { return point_; }
	void setLeft(KdTreeNode* node);
	KdTreeNode* left() const { return left_; }
	void setRight(KdTreeNode* node);
	KdTreeNode* right() const { return right_; }
	//calculates the closest distance from the point to the borders of the rectangle
	double distanceToBounds(const Point &p) const;
private:
	//updates the rectangle covered by the current point
	void updateBounds();
	Point point_;
	int bounds_[4] = { 0, 0, 0, 0 }; //{xmin, xmax, ymin, ymax},
	KdTreeNode* left_ = nullptr;
	KdTreeNode* right_ = nullptr;
};
class Comparator
{
public:
	explicit Comparator(int depth)
		:depth_(depth)
	{}
	bool operator() (const Point &p1, const Point &p2) const
	{
		if (depth_ % 2 == 0)
			return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
		return (p1.y < p2.y) || (p1.y == p2.y && p1.x < p2.x);
	}
private:
	int depth_;
};

class KdTree {
public:
	KdTree(const std::vector<Point> &points);
	~KdTree();
	void Bulid();
	void insertPoint(const Point &p);
	const KdTreeNode* findMin(int dim);
	void removePoint(const Point &p);
	Point findClosestPoint(const Point &p);
	
private:
	KdTreeNode* divideRegion(int leftindex, int rightindex, int depth = 0);
	void insertNode(Point p, KdTreeNode* node, int depth);
	const KdTreeNode* findMinNode(const KdTreeNode* node, int depth, int dim);
	KdTreeNode* removeNode(const Point &p, KdTreeNode* node, int depth);
	void findNearestNeighborNode(const Point &p, const KdTreeNode* node, int depth = 0);
	std::vector<Point> points_;
	KdTreeNode* root_ = nullptr;
	const KdTreeNode* nearestnode_ = nullptr;
	double mindinstance_ = DBL_MAX;
};


#endif


