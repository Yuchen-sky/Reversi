

// 基于黑白棋（Reversi）基础样例程序修改
// UCTS算法及尝试有效的启发式方法，在一秒内尽量算
// 作者：Yuchen
// 游戏信息：http://www.botzone.org/games#Reversi


#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "jsoncpp/json.h" // C++编译时默认包含此库
#include <stdio.h>
#include <math.h>
using namespace std;

int review[8][8] = {    //子位价值，启发式方法，辅助UCTS算法
	20, -3, 11,  8,  8, 11, -3, 20,
	-3, -7, -4,  1,  1, -4, -7, -3,
	11, -4,  2,  2,  2,  2, -4, 11,
	8,  1,  2, -3, -3,  2,  1,  8,
	8,  1,  2, -3, -3,  2,  1,  8,
	11, -4,  2,  2,  2,  2, -4, 11,
	-3, -7, -4,  1,  1, -4, -7, -3,
	20, -3, 11,  8,  8, 11, -3, 20,
};


class CTreeNode;	//前置申明，tNodeLink和CTreeNode互相含有彼此

					//节点链表结构体
struct tNodeLink
{
	CTreeNode *pChild;		//儿子节点的地址
	tNodeLink *pNext;
};

//树节点类
class CTreeNode
{
public:
	friend class Tree;

	CTreeNode();
	CTreeNode(int inx,int iny);
	~CTreeNode();
	int getNum();									//获取编号
	float getValue();								//获取节点的值
	void setValue(int c=1);						//设置节点的值
	int getFaith();								//获取次数的值
	void setFaith(int c=1);						//设置次数的值
	tNodeLink *getChildHead();						//取出儿子节点链表头
	bool hasFatherNode();							//判断是否有父亲节点 没要求
	bool setFatherNode(CTreeNode *pFather);			//设置父亲节点 
	bool addChildNode(CTreeNode *pChild);	//增加儿子节点
	bool delChildNode(CTreeNode *pChild);			//删除儿子节点

	int getX();								//获取节点的值
	int getY();								//获取节点的值
	CTreeNode *m_pFather;	//父亲节点
	bool isEnd;
	static int s_max;
	float m_value;			//节点的值
	int x;
	int y;//节点的值
private:
			//已分配的编号的数目
	int m_num;				//节点编号
	
	int faith;
	int win;

	
	tNodeLink *m_pChildHead;//儿子节点链表
	
};






int CTreeNode::s_max = 0;	//已分配节点数目初始化为0

							/*************************************************************/
							/*类CTreeNode*/

							/*主构函数*/
CTreeNode::CTreeNode()
{
	m_num = ++s_max;	//为节点分配一个独立的编号
	m_value = 0;		//初始化节点的值
	faith = 0;
	win = 0;
	x = -1;
	y = -1;
	m_pFather = NULL;	//初始化父亲节点为空
	m_pChildHead = NULL;//初始化儿子链表头为空
	isEnd = false;
}
CTreeNode::CTreeNode(int inx,int iny)
{
	m_num = ++s_max;	//为节点分配一个独立的编号
	m_value = 0;		//初始化节点的值
	faith = 0;
	win = 0;
	x = inx;
	y = iny;
	m_pFather = NULL;	//初始化父亲节点为空
	m_pChildHead = NULL;//初始化儿子链表头为空
	isEnd = false;
}
/*
析构函数
1.将自己从父亲的儿子链表中删除
2.脱离与自己的儿子们的父子关系
3.释放儿子链表的空间
*/
CTreeNode::~CTreeNode()
{
	tNodeLink *pCurrent = this->m_pChildHead;
	tNodeLink *pNext;

	//如果有父亲，则将自己从父亲的儿子链表中删除
	if (m_pFather != NULL) m_pFather->delChildNode(this);

	//删除儿子们的父亲
	while (pCurrent != NULL)
	{
		pNext = pCurrent->pNext;

		//释放儿子链表的空间
		(pCurrent->pChild)->setFatherNode(NULL);
		pCurrent = pNext;
	}
}

/*获取编号*/
int CTreeNode::getNum()
{
	return m_num;
}

/*获取节点的值*/
float CTreeNode::getValue()
{
	return m_value;
}

int CTreeNode::getX()
{
	return x;
}

int CTreeNode::getY()
{
	return y;
}



/*设置节点的值*/
void CTreeNode::setValue(int c)
{
	m_value+=c;
}


/*获取节点的值*/
int CTreeNode::getFaith()
{
	return faith;
}

/*设置节点的值*/
void CTreeNode::setFaith(int c)
{
	faith+=c;
}


/*获取儿子节点链表头的指针*/
tNodeLink *CTreeNode::getChildHead()
{
	return m_pChildHead;
}

/*判断是否有父亲节点*/
bool CTreeNode::hasFatherNode()
{
	if (m_pFather == NULL) return false;

	return true;
}

/*
设置父亲节点
1.如果没有父亲节点，则直接设置
2.如果已有父亲节点，则将自己从父节点的儿子链表中删除
*/
bool CTreeNode::setFatherNode(CTreeNode *pFather)
{
	//已有父亲节点
	if (m_pFather != NULL) m_pFather->delChildNode(this);

	//设置
	m_pFather = pFather;

	return true;
}

/*
增加儿子节点
1.儿子指针为空，返回false
2.将儿子加入到链表末尾
*/
bool CTreeNode::addChildNode(CTreeNode *pChild)
{
	tNodeLink **ppLast = &this->m_pChildHead;
	tNodeLink *pCurrent = this->m_pChildHead;
	tNodeLink *pNewChild;

	//儿子指针为空
	if (pChild == NULL)
	{
		printf("错误：儿子指针为空\n");
		return false;
	}

	

	//指向链表最后一个儿子
	while (pCurrent != NULL)
	{
		ppLast = &pCurrent->pNext;
		pCurrent = pCurrent->pNext;
	}

	//加入新儿子
	pNewChild = new tNodeLink;
	pNewChild->pChild = pChild;
	pNewChild->pChild->setFatherNode(this);
	pNewChild->pNext = NULL;
	*ppLast = pNewChild;

	return true;
}

/*
删除儿子节点
1.儿子指针为空，返回false
2.如果已有儿子节点，则将儿子从链表中删除
3.如果没有儿子节点，则返回false
*/
bool CTreeNode::delChildNode(CTreeNode *pChild)
{
	tNodeLink **ppLast = &this->m_pChildHead;
	tNodeLink *pCurrent = this->m_pChildHead;

	//儿子指针为空
	if (pChild == NULL)
	{
		printf("错误：儿子指针为空\n");
		return false;
	}

	while (pCurrent != NULL)
	{
		//找到儿子节点
		if (pCurrent->pChild == pChild)
		{
			*ppLast = pCurrent->pNext;
			delete pCurrent;

			return true;
		}

		ppLast = &pCurrent->pNext;
		pCurrent = pCurrent->pNext;
	}

	//没找到儿子
	return false;
}



/*************************************************************/
/*类CLeafNode*/







/*遍历选出最牛逼儿子*/
CTreeNode * preorderTraversal(CTreeNode *pRoot,bool flag)
{
	tNodeLink *pCurrend;
	CTreeNode * save=NULL;
	int n = pRoot->getFaith();
	float a = 0.0;
	int ni = 0;
	float price = 0.0, max = -1.0;
	int valuemax = -30;
	if (pRoot != NULL)
	{
		pCurrend = pRoot->getChildHead();
		if (flag == true) {
			while (pCurrend != NULL)
			{
				ni = pCurrend->pChild->getFaith();
				a = pCurrend->pChild->getValue();
				if (ni == 0)price = 10000000;
				else price =float( a / ni + 0.5*sqrt(log(n) / ni));
				if (price > max) {					
					valuemax = review[pCurrend->pChild->x][pCurrend->pChild->y];
					max = price; save = pCurrend->pChild;
				}
				else if (price==max&&review[pCurrend->pChild->x][pCurrend->pChild->y] > valuemax) {//取估值最好的先做
					valuemax = review[pCurrend->pChild->x][pCurrend->pChild->y];
					max = price; save = pCurrend->pChild;
				}
				pCurrend = pCurrend->pNext;
			}
		}
		else {
			while (pCurrend != NULL)
			{
				ni = pCurrend->pChild->getFaith();
				a = pCurrend->pChild->getValue();
				if (ni == 0)price = 10000000;
				else price = float((ni-a) / ni + 0.5*sqrt(float(1.96)*log(n) / ni));
				if (price > max) {
					valuemax = review[pCurrend->pChild->x][pCurrend->pChild->y];
					max = price; save = pCurrend->pChild;
				}
				else if (price == max&&review[pCurrend->pChild->x][pCurrend->pChild->y] > valuemax) {//取估值最好的先做
					valuemax = review[pCurrend->pChild->x][pCurrend->pChild->y];
					max = price; save = pCurrend->pChild;
				}

				pCurrend = pCurrend->pNext;
			}
		}

	}
	return save;
}

CTreeNode * findBest(CTreeNode *pRoot)
{
	tNodeLink *pCurrend;
	CTreeNode * save = NULL;
	
	float a = 0.0;
	int ni = 1;
	float price = 0.0, max = 0.0;
	if (pRoot != NULL)
	{
		pCurrend = pRoot->getChildHead();
			while (pCurrend != NULL)
			{
				ni = pCurrend->pChild->getFaith();
				a = pCurrend->pChild->getValue();
				price = float(a / ni);
				if (price >= max) {
					max = price; save = pCurrend->pChild;
				}

				pCurrend = pCurrend->pNext;
			}
	}
	return save;
}






















int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[8][8] = { 0 }; // 先x后y，记录棋盘状态
//int blackPieceCount = 2, whitePieceCount = 2;
int realChoice[2] = { -1 };
int Guess[8][8] = { 0 };
int tempInfo[8][8] = { 0 };
int tempweiCount = 0;//先机记录
int allPower = 0;

// 向Direction方向改动坐标，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
	if (Direction == 0 || Direction == 6 || Direction == 7)
		x++;
	if (Direction == 0 || Direction == 1 || Direction == 2)
		y++;
	if (Direction == 2 || Direction == 3 || Direction == 4)
		x--;
	if (Direction == 4 || Direction == 5 || Direction == 6)
		y--;
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	return true;
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStepc(int xPos, int yPos, int color, bool checkOnly = false)
{
	int effectivePoints[8][2];
	int dir, x, y, currCount;
	bool isValidMove = false;
	if (Guess[xPos][yPos] != 0)
		return false;
	for (dir = 0; dir < 8; dir++)
	{
		x = xPos;
		y = yPos;
		currCount = 0;
		while (1)
		{
			if (!MoveStep(x, y, dir))
			{
				currCount = 0;
				break;
			}
			if (Guess[x][y] == -color)
			{
				currCount++;
				effectivePoints[currCount][0] = x;
				effectivePoints[currCount][1] = y;
			}
			else if (Guess[x][y] == 0)
			{
				currCount = 0;
				break;
			}
			else
			{
				break;
			}
		}
		if (currCount != 0)
		{
			isValidMove = true;
			if (checkOnly)
				return true;

			while (currCount > 0)
			{
				x = effectivePoints[currCount][0];
				y = effectivePoints[currCount][1];
				Guess[x][y] *= -1;
				currCount--;
			}
		}
	}
	if (isValidMove)
	{
		Guess[xPos][yPos] = color;
		return true;
	}
	else
		return false;
}

bool ProcStepb(int xPos, int yPos, int color, bool checkOnly = false)
{
	int effectivePoints[8][2];
	int dir, x, y, currCount,weiCount=0,allpo=0;
	bool checkFlag = false;
	bool isValidMove = false;
	if (tempInfo[xPos][yPos] != 0)
		return false;
	allpo = review[xPos][yPos]*2;
	for (dir = 0; dir < 8; dir++)
	{
		x = xPos;
		y = yPos;
		currCount = 0;
		while (1)
		{
			if (!MoveStep(x, y, dir))
			{
				currCount = 0;
				break;
			}
			if (tempInfo[x][y] == -color)
			{
				
				currCount++;
				effectivePoints[currCount][0] = x;
				effectivePoints[currCount][1] = y;
				
			}
			else if (tempInfo[x][y] == 0)
			{
				currCount = 0;
				weiCount++;
				break;
			}
			else
			{
				break;
			}
		}
		if (currCount != 0)
		{
			isValidMove = true;
			if (checkOnly) {
				//tempweiCount = weiCount;//保存当前子扩展力
				checkFlag = true;
				while (currCount > 0)
				{
					x = effectivePoints[currCount][0];
					y = effectivePoints[currCount][1];
					allpo += review[x][y];
					currCount--;
				}
			}
			else {

				while (currCount > 0)
				{
					x = effectivePoints[currCount][0];
					y = effectivePoints[currCount][1];
					tempInfo[x][y] *= -1;
					currCount--;
				}
			}
		}
		
	}
	if (checkOnly) {
		tempweiCount = weiCount;//保存当前子扩展力
		allPower = allpo;
		return checkFlag;
	}
	if (isValidMove)
	{
		tempInfo[xPos][yPos] = color;
		return true;
	}
	else
		return false;
}


bool ProcStep(int xPos, int yPos, int color, bool checkOnly = false)
{
	int effectivePoints[8][2];
	int dir, x, y, currCount;
	bool isValidMove = false;
	if (gridInfo[xPos][yPos] != 0)
		return false;
	for (dir = 0; dir < 8; dir++)
	{
		x = xPos;
		y = yPos;
		currCount = 0;
		while (1)
		{
			if (!MoveStep(x, y, dir))
			{
				currCount = 0;
				break;
			}
			if (gridInfo[x][y] == -color)
			{
				currCount++;
				effectivePoints[currCount][0] = x;
				effectivePoints[currCount][1] = y;
			}
			else if (gridInfo[x][y] == 0)
			{
				currCount = 0;
				break;
			}
			else
			{
				break;
			}
		}
		if (currCount != 0)
		{
			isValidMove = true;
			if (checkOnly)
				return true;

			while (currCount > 0)
			{
				x = effectivePoints[currCount][0];
				y = effectivePoints[currCount][1];
				gridInfo[x][y] *= -1;
				currCount--;
			}
		}
	}
	if (isValidMove)
	{
		gridInfo[xPos][yPos] = color;
		return true;
	}
	else
		return false;
}





//随机落子获得结果
int randTest(int inx, int iny, int color) {
	int x, y;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++)tempInfo[x][y] = Guess[x][y];
	};	
	if(inx!=-1)ProcStepb(inx, iny, color);
	int colornow = 1;
	if (color == 1)colornow = -1;
	//开始随机落子
	bool flag[2] = { true };
	for (int count = 0; count < 60; count++) {
		int possiblePos[60][4], posCount = 0, choice;
		for (y = 0; y < 8; y++)
			for (x = 0; x < 8; x++)
				if (ProcStepb(x, y, colornow, true))
				{
					possiblePos[posCount][0] = x;
					possiblePos[posCount][2] = tempweiCount;
					possiblePos[posCount][3] = allPower;
					possiblePos[posCount++][1] = y;
					tempweiCount = 0;
					allPower = 0;
				}
		int resultX=0, resultY=0;
		if (posCount > 0)
		{
			//bool sizeflag =true;
			int max=-30,power=-10,saveit=-100;
			if (true) {//估值下法
				for (int c = 0; c < posCount; c++) {
					//saveit=review[possiblePos[c][0]][possiblePos[c][1]];
					saveit = possiblePos[c][3];
					if (saveit>max) {
					  
						resultX = possiblePos[c][0];
						resultY = possiblePos[c][1];
						power= possiblePos[c][2];
						max = review[resultX][resultY];
						//sizeflag = false;
						
					}
					else if (saveit == max&&possiblePos[c][2]>power) {
						resultX = possiblePos[c][0];
						resultY = possiblePos[c][1];
						power = possiblePos[c][2];
						//max = review[resultX][resultY];
					}
				}
			}
		
			flag[0] = true;
			flag[1] = true;
			ProcStepb(resultX, resultY, colornow);
		}
		else
		{
			resultX = -1;
			resultY = -1;
			if (colornow == 1)flag[0] = false;
			else
			{
				flag[1] = false;
			}
		}
		if (flag[0] == false && flag[1] == false)break;
		//转色
		colornow *= -1;
	}
	int point = 0;
	for (x = 0; x < 8; x++)
		for (y = 0; y < 8; y++)point += tempInfo[x][y];
	return point;
}



void caculate() {
	CTreeNode * root = new CTreeNode();	
	CTreeNode *current = root;
	int x, y;
	realChoice[0] = -1;
	realChoice[1] = -1;
	int curcolor = currBotColor;
	bool flagColor = true;

	for (x = 0; x < 8; x++) {//初始化棋盘
		for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
	};
	int num = 0;
	int strong = 1;//结果强化值
	clock_t start;
	start = clock();
	while (((double)(clock() - start) / CLOCKS_PER_SEC)<0.98) {//迭代次数
		
		CTreeNode* temp=preorderTraversal(current,flagColor);
		if (temp != NULL && 0 == temp->getFaith()) {
			bool flagwin = false;
			int save = 0;
			save = randTest(temp->getX(), temp->getY(), curcolor);
			temp->setFaith();
			if ((save > 0 && currBotColor == 1) || (save < 0 && currBotColor == -1)) { flagwin = true; temp->setValue(); }
			CTreeNode * findFa = temp;
			while (findFa->hasFatherNode()) {
				findFa->m_pFather->setFaith();
				if (flagwin == true)findFa->m_pFather->setValue();
				findFa = findFa->m_pFather;
			}
			current = root;//重置回根节点
			flagColor = true;
			curcolor = currBotColor;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
        else if (temp == NULL&&curcolor == currBotColor) {//我方下子
			int possiblePos[64][2], posCount = 0;
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					if (ProcStepc(x, y, curcolor, true))//所有可能
					{
						possiblePos[posCount][0] = x;
						
						possiblePos[posCount++][1] = y;
					}
			if (posCount > 0) {
				int save = 0;//子节点可行点扩展
				int valuemax = -30,cache=0;
				for (int i = 0; i < posCount; i++) {//估值优先
					if (review[possiblePos[i][0]][possiblePos[i][1]] > valuemax) {
						valuemax = review[possiblePos[i][0]][possiblePos[i][1]];
						cache = i;
					}
				}
				for (int i = 0; i < posCount; i++) {					
					CTreeNode *child = new CTreeNode(possiblePos[i][0], possiblePos[i][1]);
					CTreeNode *findFa = child;
					current->addChildNode(child);
					bool flagwin = false;
					if (i == cache) {//for (int d = 0; d < 1; d++) {//模拟出结果
						flagwin = false;
						save = randTest(possiblePos[i][0], possiblePos[i][1], curcolor);
						child->setFaith();
						if ((save > 0 && currBotColor == 1) || (save < 0 && currBotColor == -1)) { flagwin = true; child->setValue(); }
						while (findFa->hasFatherNode()) {
							findFa->m_pFather->setFaith();
							if (flagwin == true)findFa->m_pFather->setValue();
							findFa = findFa->m_pFather;
						}
					}//}
				}
			}
			else { 
				bool end = true; 
				int point = 0;
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) { 
						if (Guess[x][y] == 0)end = false; 
						point += Guess[x][y];
					}
				};
				if (end == true) {
					CTreeNode *findFa = current;
					bool flagwin = false;
					if ((point > 0 && currBotColor == 1) || (point < 0 && currBotColor == -1)) 
					{ flagwin = true; current->setValue(strong);
					}
					current->setFaith(strong);
					
					while (findFa->hasFatherNode()) {
						findFa->m_pFather->setFaith(strong);
						if (flagwin)findFa->m_pFather->setValue(strong);
						findFa = findFa->m_pFather;
					}
				}
				else {
					int  aposCount = 0;
					int tempcolor = (-1)*curcolor;
					for (y = 0; y < 8; y++) {
						for (x = 0; x < 8; x++)
							if (ProcStepc(x, y, tempcolor, true))//另一方能否下
							{
								aposCount++;
								break;
							};
					}
					if (aposCount > 0) {
						CTreeNode *child = new CTreeNode(-1,-1 );
						CTreeNode *findFa = child;
						current->addChildNode(child);
						bool flagwin = false;
						int show = 0;
						show = randTest(-1,-1, curcolor);
						child->setFaith();
						if ((show > 0 && currBotColor == 1) || (show < 0 && currBotColor == -1)) { flagwin = true; child->setValue(); }
						while (findFa->hasFatherNode()) {
								findFa->m_pFather->setFaith();
								if (flagwin)findFa->m_pFather->setValue();
								findFa = findFa->m_pFather;
						}						
					}
					else {
						CTreeNode *findFa = current;
						bool flagwin = false;
						if ((point > 0 && currBotColor == 1) || (point < 0 && currBotColor == -1))  flagwin = true;
						current->setFaith(strong);
						if (flagwin)current->setValue(strong);
						while (findFa->hasFatherNode()) {
							findFa->m_pFather->setFaith(strong);
							if (flagwin)findFa->m_pFather->setValue(strong);
							findFa = findFa->m_pFather;
						}
					}				
				}
			}
			current = root;//重置回根节点
			flagColor = true;
			curcolor = currBotColor;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
		else if(temp == NULL&&curcolor != currBotColor){//地方落子
			int possiblePos[64][2], posCount = 0;
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					if (ProcStepc(x, y, curcolor, true))
					{
						possiblePos[posCount][0] = x;
						
						possiblePos[posCount++][1] = y;
					}
			if (posCount > 0) {
				int save = 0;//子节点可行点扩展
				int valuemax = -30, cache = 0;
				for (int i = 0; i < posCount; i++) {
					if (review[possiblePos[i][0]][possiblePos[i][1]] > valuemax) {
						valuemax = review[possiblePos[i][0]][possiblePos[i][1]];
						cache = i;
					}
				}
				for (int i = 0; i < posCount; i++) {
					CTreeNode *child = new CTreeNode(possiblePos[i][0], possiblePos[i][1]);
					CTreeNode *findFa = child;
					current->addChildNode(child);
					bool flagwin = false;
					if (i == cache) {//for (int d = 0; d < 1; d++) {
						flagwin = false;
						save = randTest(possiblePos[i][0], possiblePos[i][1], curcolor);
						child->setFaith();
						if ((save > 0 && currBotColor == 1) || (save < 0 && currBotColor == -1)) { flagwin = true; child->setValue(); }
						while (findFa->hasFatherNode()) {
							findFa->m_pFather->setFaith();
							if (flagwin == true)findFa->m_pFather->setValue();
							findFa = findFa->m_pFather;
						}
					}//}
				}
			}
			else {
				bool end = true;
				int point = 0;
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) {
						if (Guess[x][y] == 0)end = false;
						point += Guess[x][y];
					}
				};
				if (end == true) {
					CTreeNode *findFa = current;
					bool flagwin = false;
					if ((point > 0 && currBotColor == 1) || (point < 0 && currBotColor == -1))  flagwin = true;
					current->setFaith(strong);
					if (flagwin)current->setValue(strong);
					while (findFa->hasFatherNode()) {
						findFa->m_pFather->setFaith(strong);
						if (flagwin)findFa->m_pFather->setValue(strong);
						findFa = findFa->m_pFather;
					}
				}
				else {
					int  aposCount = 0;
					int tempcolor = (-1)*curcolor;
					for (y = 0; y < 8; y++)
						for (x = 0; x < 8; x++)
							if (ProcStepc(x, y, tempcolor, true))//另一方能否下
							{
								aposCount++;
								break;
							}
					if (aposCount > 0) {
						CTreeNode *child = new CTreeNode(-1, -1);
						CTreeNode *findFa = child;
						current->addChildNode(child);
						bool flagwin = false;
						int show = 0;
						show = randTest(-1, -1, curcolor);
						child->setFaith();
						if ((show > 0 && currBotColor == 1) || (show < 0 && currBotColor == -1)) { flagwin = true; child->setValue(); }
						while (findFa->hasFatherNode()) {
							findFa->m_pFather->setFaith();
							if (flagwin)findFa->m_pFather->setValue();
							findFa = findFa->m_pFather;
						}
					}
					else {
						CTreeNode *findFa = current;
						bool flagwin = false;
						if ((point > 0 && currBotColor == 1) || (point < 0 && currBotColor == -1))  flagwin = true;
						current->setFaith(strong);
						if (flagwin)current->setValue(strong);
						while (findFa->hasFatherNode()) {
							findFa->m_pFather->setFaith(strong);
							if (flagwin)findFa->m_pFather->setValue(strong);
							findFa = findFa->m_pFather;
						}
					}
				}
			}
			current = root;//重置回根节点
			curcolor = currBotColor;
			flagColor = true;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
		else//选取最好子点下寻
		{
			current = temp;
			if(current->getX()!=-1)ProcStepc(current->getX(), current->getY(), curcolor);//落子
			curcolor *= -1;//变色			
			if (curcolor == currBotColor)flagColor = true;
			else { flagColor = false; }
		}
		//num++;
	}
	CTreeNode* temp = findBest(root);
	if (temp != NULL) {
		realChoice[0]=temp->getX();
		realChoice[1] = temp->getY();
	}
	delete root;   
}






int main()
{
	int x, y;

	// 初始化棋盘
	gridInfo[3][4] = gridInfo[4][3] = 1;  //|白|黑|
	gridInfo[3][3] = gridInfo[4][4] = -1; //|黑|白|

										  // 读入JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : -1; // 第一回合收到坐标是-1, -1，说明我是黑方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		x = input["requests"][i]["x"].asInt();
		y = input["requests"][i]["y"].asInt();
		if (x >= 0)
			ProcStep(x, y, -currBotColor); // 模拟对方落子
		x = input["responses"][i]["x"].asInt();
		y = input["responses"][i]["y"].asInt();
		if (x >= 0)
			ProcStep(x, y, currBotColor); // 模拟己方落子
	}

	// 看看自己本回合输入
	x = input["requests"][turnID]["x"].asInt();
	y = input["requests"][turnID]["y"].asInt();
	if (x >= 0)
		ProcStep(x, y, -currBotColor); // 模拟对方落子

									   // 找出合法落子点
	
	// 做出决策（你只需修改以下部分）
	
	caculate();
	/*
	int resultX, resultY;
	if (posCount > 0)
	{
		srand(time(0));
		choice = rand() % posCount;
		resultX = possiblePos[choice][0];
		resultY = possiblePos[choice][1];
	}
	else
	{
		resultX = -1;
		resultY = -1;
	}
	*/
	int resultX = realChoice[0];
	int resultY = realChoice[1];
	// 决策结束，输出结果（你只需修改以上部分）

	Json::Value ret;
	ret["response"]["x"] = resultX;
	ret["response"]["y"] = resultY;
	Json::FastWriter writer;
	std::cout << writer.write(ret) << endl;
	return 0;
}