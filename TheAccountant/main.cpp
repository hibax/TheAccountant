#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <ctime>

using namespace std;

struct Point
{
	int x;
	int y;
};


class Entity
{
protected:
	Point position;
	int id;
public:
	Entity(int id, int x, int y) : id(id), position({ x,y }) {};
	virtual ~Entity() {};
	Point getPosition() const { return position; };
	void setPosition(Point pos) { position = pos; };
	int getId() const { return id; };
	void setId(int id) { this->id = id; };
};

class Enemy : public Entity
{
	int life;
	Point target;
	Point nextPosition;
public:
	Enemy(int id, int x, int y, int l) : Entity(id, x, y), life(l) {};
	~Enemy() {};
	int getLife() const { return life; };
	void setLife(int l) { life = l; };
	void setTarget(Point p) { target = p; };
	Point getTarget() const { return target; };
	void setNextPosition(Point p) { nextPosition = p; };
	Point getNextPosition() const { return nextPosition; };
};

class DataPoint : public Entity
{
public:
	DataPoint(int id, int x, int y) : Entity(id, x, y) {};
	~DataPoint() {};

};


float calculateDistance(const Point& p1, const Point& p2)
{
	float diffY = p1.y - p2.y;
	float diffX = p1.x - p2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

Point calculateBarycentre(vector<Entity*> &pointList)
{
	Point barycentre;

	int xSum = 0;
	int ySum = 0;
	for (size_t i = 0; i < pointList.size(); ++i)
	{
		xSum += pointList[i]->getPosition().x;
		ySum += pointList[i]->getPosition().y;
	}
	barycentre.x = static_cast<int>(xSum / pointList.size());
	barycentre.y = static_cast<int>(ySum / pointList.size());

	return barycentre;
}


Entity * calculateNearestEntity(const Point &pos, vector<Entity*> &entityList) {

	Entity * nearestEntity = nullptr;
	float distanceMinimum = 999999;
	for (size_t i = 0; i < entityList.size(); ++i)
	{
		float distanceFromEntity = calculateDistance(pos, entityList[i]->getPosition());

		if (distanceFromEntity < distanceMinimum) {
			nearestEntity = entityList[i];
			distanceMinimum = distanceFromEntity;
		}
	}
	return nearestEntity;
}

Point calculateDeplacement(const Point &from, const Point &to, const float &distance_to_do) {
	Point targetPosition;

	targetPosition.x = static_cast<int>(from.x + (distance_to_do / calculateDistance(from, to))*(to.x - from.x));
	targetPosition.y = static_cast<int>(from.y + (distance_to_do / calculateDistance(from, to))*(to.y - from.y));

	return targetPosition;
}

void calculateEnemiesDeplacements(vector<Entity*> &enemyList, vector<Entity*> &dataList) {

	for (size_t i = 0; i < enemyList.size(); ++i) {
		Entity * nearestDataPoint = calculateNearestEntity(enemyList[i]->getPosition(), dataList);
		static_cast<Enemy* >(enemyList[i])->setTarget(nearestDataPoint->getPosition());

		Point nextPosition = calculateDeplacement(enemyList[i]->getPosition(), nearestDataPoint->getPosition(), 500);
		static_cast<Enemy* >(enemyList[i])->setNextPosition(nextPosition);
	}
}

Enemy * calculateDangerousEnemies(Point &myPosition, vector<Entity*> &enemyList) {
	Enemy * dangerousEnemy;
	for (size_t i = 0; i < enemyList.size(); ++i) {
		dangerousEnemy = static_cast<Enemy * >(enemyList[i]);
		if (calculateDistance(myPosition, dangerousEnemy->getNextPosition()) < 2100) {
			return dangerousEnemy;
		}
	}
	return nullptr;
}


Enemy * calculateDangerousEnemies2(Point &myPosition, vector<Entity*> &enemyList) {
	Enemy * dangerousEnemy;
	for (size_t i = 0; i < enemyList.size(); ++i) {
		dangerousEnemy = static_cast<Enemy *>(enemyList[i]);
		if (calculateDistance(myPosition, dangerousEnemy->getNextPosition()) < 2100) {
			return dangerousEnemy;
		}

		Point nextPosition2 = calculateDeplacement(dangerousEnemy->getPosition(), dangerousEnemy->getTarget(), 1500);
		if (calculateDistance(myPosition, nextPosition2) < 2100) {
			return dangerousEnemy;
		}
	}
	return nullptr;
}



bool isThereEnemiesInRange(Point &myPosition, vector<Entity*> &enemyList, int range) {
	for (size_t i = 0; i < enemyList.size(); ++i) {
		if (calculateDistance(myPosition, enemyList[i]->getPosition()) < range) {
			return true;
		}
	}
	return false;
}


Entity * isThereDataPointInDanger(vector<Entity*> &dataList, vector<Entity*> &enemyList, int range, int lifeMax)
{
	float minDistance = range;
	Entity * entity = nullptr;

	for (size_t i = 0; i < dataList.size(); ++i) {

		Entity * tmp = calculateNearestEntity(dataList[i]->getPosition(), enemyList);
		if (calculateDistance(tmp->getPosition(), dataList[i]->getPosition()) < minDistance)
		{
			if (static_cast<Enemy * >(tmp)->getLife() < lifeMax)
			{
				minDistance = calculateDistance(tmp->getPosition(), dataList[i]->getPosition());
				entity = tmp;
			}
		}
	}
	return entity;
}


Point findSafePosition(Point &myPosition, vector<Entity*> &enemyList, Enemy * &nearestEnemy) {
	Point sumEnemyVectors = myPosition;
	Point safePos;
	vector<Enemy * > dangerousEnemies;
	for (size_t i = 0; i < enemyList.size(); ++i) {
		Enemy * dangerousEnemy = static_cast<Enemy * >(enemyList[i]);
		if (calculateDistance(myPosition, dangerousEnemy->getPosition()) < 4000) {
			dangerousEnemies.push_back(dangerousEnemy);
		}
	}
	for (size_t i = 0; i < dangerousEnemies.size(); ++i) {
		Enemy * dangerousEnemy = dangerousEnemies[i];

		sumEnemyVectors.x = dangerousEnemy->getPosition().x - myPosition.x;
		sumEnemyVectors.y = dangerousEnemy->getPosition().y - myPosition.x;
	}

	safePos = calculateDeplacement(myPosition, sumEnemyVectors, -1000);
	int count = 0;
	cerr << to_string(count + 1) + "st attempt = " + to_string(safePos.x) + "," + to_string(safePos.y) << endl;


	double pi = std::acos(-1);
	vector<double> angles = { pi / 6, pi / 4, pi / 3, pi / 2, 2 * pi / 3, 3 * pi / 4, 5 * pi / 6, -pi / 6, -pi / 4, -pi / 3, -pi / 2, -2 * pi / 3, -3 * pi / 4, -5 * pi / 6 };

	Point tmpPos = calculateDeplacement(myPosition, nearestEnemy->getPosition(), -1000);
	for (size_t i = 0; i < angles.size(); ++i)
	{
		Point targetVectorWithAngle;
		Point targetVector = { tmpPos.x - myPosition.x, tmpPos.y - myPosition.y };
		targetVectorWithAngle.x = cos(angles[i]) * targetVector.x - sin(angles[i]) * targetVector.y;
		targetVectorWithAngle.y = sin(angles[i]) * targetVector.x - cos(angles[i]) * targetVector.y;
		safePos = calculateDeplacement(myPosition, { myPosition.x + targetVectorWithAngle.x, myPosition.y + targetVectorWithAngle.y }, 1000);
		cerr << to_string(count + 1) + "st attempt = " + to_string(safePos.x) + "," + to_string(safePos.y) << endl;

		if (calculateDangerousEnemies(safePos, enemyList) == nullptr)
		{
			break;
		}
	}
	return safePos;
}

string generateAction(Point &myPosition, vector<Entity*> &dataList, vector<Entity*> &enemyList)
{

	Enemy * nearestEnemy = static_cast<Enemy * >(calculateNearestEntity(myPosition, enemyList));

	Point barycentre = calculateBarycentre(dataList);
	cerr << "Barycentre : " + to_string(barycentre.x) + "," + to_string(barycentre.y) << endl;

	calculateEnemiesDeplacements(enemyList, dataList);

	Enemy * dangerousEnemy = calculateDangerousEnemies(myPosition, enemyList);

	if (dangerousEnemy != nullptr) {
		cerr << "Enemy is dangerously approching" << endl;

		Point target = calculateDeplacement(myPosition, nearestEnemy->getPosition(), -1000);

		cerr << "Nearest enemy = " + to_string(nearestEnemy->getPosition().x) + "," + to_string(nearestEnemy->getPosition().y) << endl;
		Enemy * dangerousEnemy = calculateDangerousEnemies(target, enemyList);
		if (dangerousEnemy != nullptr) {
			cerr << "Aie... First escape attempt didn't work" << endl;
			target = findSafePosition(myPosition, enemyList, nearestEnemy);
		}
		return "MOVE " + to_string(target.x) + " " + to_string(target.y);
	}
	else if (isThereEnemiesInRange(myPosition, enemyList, 3000))
	{
		return "SHOOT " + to_string(nearestEnemy->getId());
	}
	else
	{
		int range = 500;
		int lifeMax = 3;

		Entity * entity = isThereDataPointInDanger(dataList, enemyList, range, lifeMax);
		if (entity != nullptr) {
			return "SHOOT " + to_string(entity->getId());
		}

		range = 1000;
		lifeMax = 5;

		entity = isThereDataPointInDanger(dataList, enemyList, range, lifeMax);
		if (entity != nullptr) {
			return "SHOOT " + to_string(entity->getId());
		}


		Point target = calculateDeplacement(myPosition, nearestEnemy->getPosition(), 1000);
		Enemy * dangerousEnemy = calculateDangerousEnemies2(target, enemyList);

		if (dangerousEnemy == nullptr) {
			return "MOVE " + to_string(target.x) + " " + to_string(target.y);
		}
		else {
			return "SHOOT " + to_string(nearestEnemy->getId());
		}

	}
}

int main()
{
	int i = 0;
	vector<Entity*> dataList;
	vector<Entity*> enemyList;

	/*
		Here is the game loop
	*/

	Point myPosition = { 7500,2000 };

	if (i == 0)
	{
		// Test case  = 3 enemies 

		int dataListSize = 2;
		DataPoint *data1 = new DataPoint(0, 9000, 1200);
		DataPoint *data2 = new DataPoint(1, 400, 6000);

		dataList.reserve(dataListSize);
		dataList.push_back(data1);
		dataList.push_back(data2);

		int EnemyListSize = 3;
		Enemy *enemy1 = new Enemy(0, 500, 4500, 10);
		Enemy *enemy2 = new Enemy(1, 13900, 5000, 10);
		Enemy *enemy3 = new Enemy(2, 7000, 7500, 10);

		enemyList.reserve(EnemyListSize);
		enemyList.push_back(enemy1);
		enemyList.push_back(enemy2);
		enemyList.push_back(enemy3);
	}
	else
	{
		/*
			Example of how to re-fill list. Datas are wrong
		*/
		size_t newDataListSize = 2;
		for (size_t i = 0; i < dataList.size(); ++i)
		{
			if (i < newDataListSize) {
				dataList[i]->setId(0);
				dataList[i]->setPosition({ 9000, 1200 });
			}
			else {
				delete dataList[i];
			}
		}
		dataList.resize(newDataListSize);

		size_t newEnemyListSize = 3;
		for (size_t i = 0; i < enemyList.size(); ++i)
		{
			if (i < newEnemyListSize) {
				enemyList[i]->setId(0);
				enemyList[i]->setPosition({ 9000, 1200 });
				static_cast<Enemy*>(enemyList[i])->setLife(500);
			}
			else {
				delete enemyList[i];
			}
		}
		enemyList.resize(newEnemyListSize);
	}

	string output = generateAction(myPosition, dataList, enemyList);
	string expectedOutput = "MOVE 7409 2995";

	++i;
	/*
		End of game loop
	*/

	assert(output == expectedOutput);

	system("pause");
}


/*
	Working main

int main()
{
    int i = 0;
	vector<Entity*> dataList;
	vector<Entity*> enemyList;
    // game loop
    while (1) {
        Point myPosition;
        cin >> myPosition.x >> myPosition.y; cin.ignore();
        cerr << "My Position : " + to_string(myPosition.x) + " : " + to_string(myPosition.y) << endl;
        
        
        if (i == 0)
    	{
            int dataCount;
            cin >> dataCount; cin.ignore();
            
            dataList.reserve(dataCount);
            for (int i = 0; i < dataCount; i++) {
                int id, x, y;
                cin >> id >> x >> y; cin.ignore();
                dataList.push_back(new DataPoint(id, x, y));
                cerr << "Data Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y) << endl;
            }
            
            int enemyCount;
            cin >> enemyCount; cin.ignore();
            
            enemyList.reserve(enemyCount);
            for (int i = 0; i < enemyCount; i++) {
                int id, x, y, enemyLife;
                cin >> id >> x >> y >> enemyLife; cin.ignore();
                enemyList.push_back(new Enemy(id, x, y, enemyLife));
                cerr << "Enemy Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y)  + " : " + to_string(enemyLife) << endl;
            }
    	}
    	else
    	{
            int newDataListSize;
            cin >> newDataListSize; cin.ignore();
    	    
    		for (int i = 0; i < dataList.size(); ++i)
    		{
    			if (i < newDataListSize) {
    			    int id, x, y;
                    cin >> id >> x >> y; cin.ignore();
    				dataList[i]->setId(id);
    				dataList[i]->setPosition({ x, y });
    				cerr << "Data Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y) << endl;
    			}
    			else {
    				delete dataList[i];
    			}
    		}
    		dataList.resize(newDataListSize);
    		
            int newEnemyListSize;
            cin >> newEnemyListSize; cin.ignore();
    		for (int i = 0; i < enemyList.size(); ++i)
    		{
    			if (i < newEnemyListSize) {
    			    int id, x, y, l;
                    cin >> id >> x >> y >> l; cin.ignore();
    				enemyList[i]->setId(id);
    				enemyList[i]->setPosition({ x, y });
    				static_cast<Enemy*>(enemyList[i])->setLife(l);
    				cerr << "Enemy Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y)  + " : " + to_string(l) << endl;
    			}
    			else {
    				delete enemyList[i];
    			}
    		}
    		enemyList.resize(newEnemyListSize);
    	}
    	
    	
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << generateAction(myPosition, dataList, enemyList)  << endl; // MOVE x y or SHOOT id
        ++i;
    }
}


*/
