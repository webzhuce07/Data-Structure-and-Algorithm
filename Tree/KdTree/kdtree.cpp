#include "kdtree.h"
#include <algorithm>
#include <limits>
#include <iostream>
using namespace std;

/************************************************************************/
/*KdTreeNode                                                            */
/************************************************************************/
KdTreeNode::KdTreeNode(const Point &p)
	:point_(p)
{
	updateBounds();
}

KdTreeNode::~KdTreeNode()
{
	if (left_ != nullptr)
		delete left_;
	if (right_ != nullptr)
		delete right_;
}

void KdTreeNode::setPoint(const Point& p)
{
	point_ = p;
	updateBounds();
}

void KdTreeNode::setLeft(KdTreeNode* node)
{
	left_ = node;
	updateBounds();
}

void KdTreeNode::setRight(KdTreeNode* node)
{
	right_ = node;
	updateBounds();
}

void KdTreeNode::updateBounds()
{
	bounds_[0] = point_.x;
	bounds_[1] = point_.x;
	bounds_[2] = point_.y;
	bounds_[3] = point_.y;
	if (left_ != nullptr) {
		bounds_[0] = min(left_->bounds_[0], bounds_[0]);
		bounds_[1] = max(left_->bounds_[1], bounds_[1]);
		bounds_[2] = min(left_->bounds_[2], bounds_[2]);
		bounds_[3] = max(left_->bounds_[3], bounds_[3]);
	}
	if (right_ != nullptr) {
		bounds_[0] = min(right_->bounds_[0], bounds_[0]);
		bounds_[1] = max(right_->bounds_[1], bounds_[1]);
		bounds_[2] = min(right_->bounds_[2], bounds_[2]);
		bounds_[3] = max(right_->bounds_[3], bounds_[3]);
	}

}

double KdTreeNode::distanceToBounds(const Point &p) const
{
	bool xless = (p.x < bounds_[0]);
	bool xmore = (p.x > bounds_[1]);
	bool yless = (p.y < bounds_[2]);
	bool ymore = (p.y > bounds_[3]);
	bool withinX = !xless && !xmore;
	bool withinY = !yless && !ymore;
	//point is inside the box
	if (withinX && withinY)
		return 0.0;
	if (withinX)
		return max(abs(p.y - bounds_[2]), abs(p.y - bounds_[3]));
	if (withinY)
		return max(abs(p.x - bounds_[0]), abs(p.x - bounds_[1]));
	double d1 = max(Point::EuclideanDistance(p, { bounds_[0], bounds_[2] }), Point::EuclideanDistance(p, { bounds_[0], bounds_[2] }));
	double d2 = max(Point::EuclideanDistance(p, { bounds_[1], bounds_[2] }), Point::EuclideanDistance(p, { bounds_[1], bounds_[2] }));
	return max(d1, d2);
}

/************************************************************************/
/*KdTree                                                                */
/************************************************************************/
KdTree::KdTree(const vector<Point> &points)
	:points_(points)
{

}

KdTree::~KdTree()
{
	if (root_ != nullptr)
		delete root_;
}

void KdTree::Bulid()
{
	root_ = divideRegion(0, points_.size());
}

void KdTree::insertPoint(const Point &p)
{
	insertNode(p, root_, 0);
}

const KdTreeNode* KdTree::findMin(int dim)
{
	return findMinNode(root_, 0, dim);
}

void KdTree::removePoint(const Point &p)
{
	removeNode(p, root_, 0);
}

Point KdTree::findClosestPoint(const Point &p)
{
	Point result = root_->point();
	nearestnode_ = root_;
	mindinstance_ = DBL_MAX;
	findNearestNeighborNode(p, root_);
	if (nearestnode_ != nullptr)
		result = nearestnode_->point();
	return result;
}

const KdTreeNode* KdTree::findMinNode(const KdTreeNode* node, int depth, int dim)
{
	// empty tree
	if (node == nullptr)
		return nullptr;
	// node splits on the dimension we are searching only visit left subtree
	if (depth % 2 == dim) {
		if (node->left() == nullptr)
			return node;
		else return findMinNode(node->left(), depth + 1, dim);
	}
	// node splits on a different dimension, we have to search both subtrees
	else {
		const KdTreeNode* minnode = node;
		Comparator cmp(dim);
		const KdTreeNode* node1 = findMinNode(node->left(), depth + 1, dim);
		if (node1)
			minnode = cmp(minnode->point(), node1->point()) ? minnode : node1;
		const KdTreeNode* node2 = findMinNode(node->right(), depth + 1, dim);
		if (node1)
			minnode = cmp(minnode->point(), node2->point()) ? minnode : node2;
		return minnode;
	}
}

void KdTree::insertNode(Point p, KdTreeNode* node, int depth)
{
	Comparator cmp(depth);
	if (node == nullptr)
		node = new KdTreeNode(p);
	else if (p == node->point())
		cout << "error duplicate" << endl;
	else if (cmp(p, node->point())) {
		insertNode(p, node->left(), depth + 1);
	} else insertNode(p, node->right(), depth + 1);
}

KdTreeNode* KdTree::divideRegion(int leftindex, int rightindex, int depth /*= 0*/)
{
	if (leftindex >= rightindex)
		return nullptr;
	int mid = (leftindex + rightindex) / 2;
	nth_element(points_.begin() + leftindex, points_.begin() + mid, points_.begin() + rightindex, Comparator(depth));
	KdTreeNode *node = new KdTreeNode(points_[mid]);
	KdTreeNode *leftnode = divideRegion(leftindex, mid, depth + 1);
	node->setLeft(leftnode);
	KdTreeNode *rightnode = divideRegion(mid + 1, rightindex, depth + 1);
	node->setRight(rightnode);
	return node;
}

KdTreeNode* KdTree::removeNode(const Point &p, KdTreeNode* node, int depth)
{
	if (node == nullptr)
		return nullptr;
	Comparator cmp(depth);
	// This is the point to delete
	if (p == node->point())
		// use min(cd) from right subtree
		if (node->right() != nullptr) {
			const KdTreeNode* minnode = findMinNode(node->right(), (depth + 1) % 2, depth % 2);
			node->setPoint(minnode->point());
			KdTreeNode* deletenode = removeNode(node->point(), node->right(), depth % 2);
			if (deletenode) {
				delete deletenode;
				node->setRight(nullptr);
			}
			return nullptr;
		}
	// swap subtrees and use min(cd) from new right:
		else if (node->left() != nullptr) {
			const KdTreeNode* minnode = findMinNode(node->left(), (depth + 1) % 2, depth % 2);
			node->setPoint(minnode->point());
			KdTreeNode* deletenode = removeNode(node->point(), node->left(), depth + 1);
			if (deletenode) {
				delete deletenode;
				node->setLeft(nullptr);
			}
			return nullptr;
		}
		// it is a leaf: just remove
		else {
			cout << "delete success" << endl;
			return node;
		}
		// this is not the point, so search for it:
	else if (cmp(p, node->point()))
		return  removeNode(p, node->left(), depth + 1);
	else return removeNode(p, node->right(), depth + 1);
}


void KdTree::findNearestNeighborNode(const Point &p, const KdTreeNode* node, int depth /*= 0*/)
{
	if (node == nullptr)
		return;
	// if this bounding box is too far, do nothing
	if (node->distanceToBounds(p) > mindinstance_)
		return;
	// if this point is better than the best:
	double dist = Point::EuclideanDistance(p, node->point());
	if (dist < mindinstance_) {
		nearestnode_ = node;
		mindinstance_ = dist;
	}
	// visit subtrees is most promising order
	Comparator cmp(depth);
	if (cmp(p, node->point())) {
		findNearestNeighborNode(p, node->left(), depth + 1);
		findNearestNeighborNode(p, node->right(), depth + 1);
	}
	else {
		findNearestNeighborNode(p, node->right(), depth + 1);
		findNearestNeighborNode(p, node->left(), depth + 1);
	}
}
