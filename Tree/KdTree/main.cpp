#include <vector>
#include "kdtree.h"
#include <iostream>
using namespace std;

int main() {
	vector<Point> points;
	points.push_back({ 2, 3 });
	points.push_back({ 5, 4 });
	points.push_back({ 9, 6 });
	points.push_back({ 4, 7 });
	points.push_back({ 8, 1 });
	points.push_back({ 7, 2 });

	KdTree tree(points);
	//bulids tree
	tree.Bulid();
	tree.insertPoint({ 7, 2 });
	tree.insertPoint({ 3, 6 });
	const KdTreeNode* xmin = tree.findMin(0);
	if (xmin)
		cout << "xmin: (" << xmin->point().x << "," << xmin->point().y << ")" << endl;
	const KdTreeNode* ymin = tree.findMin(1);
	if (ymin)
		cout << "ymin: (" << ymin->point().x << "," << ymin->point().y << ")" << endl;
	tree.removePoint({ 4, 7 });
	Point neraestpoint = tree.findClosestPoint({ 8, 3 });
	cout << "neraestpoint: (" << neraestpoint.x << "," << neraestpoint.y << ")" << endl;
	system("pause");
	return 0;
}