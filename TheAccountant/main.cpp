#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

struct Point
{
	int id;
	int x;
	int y;
};

float calculateDistance(const Point& p1, const Point& p2)
{
	float diffY = p1.y - p2.y;
	float diffX = p1.x - p2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

Point calculateBarycentre(vector<Point> pointList)
{
	Point barycentre;

	int xSum = 0;
	int ySum = 0;
	for (int i = 0; i < pointList.size(); ++i)
	{
		xSum += pointList[i].x;
		ySum += pointList[i].y;
	}
	barycentre.x = static_cast<int>(xSum / pointList.size());
	barycentre.y = static_cast<int>(ySum / pointList.size());

	return barycentre;
}

string generateAction(Point myPosition, vector<Point> dataList, vector<Point> enemyList)
{
	int nearestEnemyId = -1;
	float distanceMinimum = 999999;

	for (int i = 0; i < enemyList.size(); ++i)
	{
		const float distanceFromEnemy = calculateDistance(myPosition, enemyList[i]);

		cerr << "Enemy ID : " + to_string(enemyList[i].id) + " : " + to_string(distanceFromEnemy) << endl;
		if (distanceFromEnemy < distanceMinimum) {
			nearestEnemyId = enemyList[i].id;
			distanceMinimum = distanceFromEnemy;
		}
	}

	Point barycentre = calculateBarycentre(dataList);
	cerr << "Barycentre : " + to_string(barycentre.x) + "," + to_string(barycentre.y) << endl;

	if (calculateDistance(myPosition, barycentre) < 1500)
	{
		return "SHOOT " + to_string(nearestEnemyId);
	}
	else
	{
		return "MOVE " + to_string(barycentre.x) + " " + to_string(barycentre.y);
	}
}


int main()
{
	Point myPosition = { -1, 7500,2000 };


	Point data1 = { 0, 9000, 1200 };
	Point data2 = { 1, 400, 6000 };
	Point enemy1 = { 0, 500, 4500 };
	Point enemy2 = { 1, 13900, 5000 };
	Point enemy3 = { 2, 7000, 7500 };

	vector<Point> dataList;
	dataList.push_back(data1);
	dataList.push_back(data2);

	vector<Point> enemyList;
	enemyList.push_back(enemy1);
	enemyList.push_back(enemy2);
	enemyList.push_back(enemy3);

	string output = generateAction(myPosition, dataList, enemyList);
	string expectedOutput = "MOVE 4700 3600";

	assert(output == expectedOutput);
}