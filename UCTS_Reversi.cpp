

// ���ںڰ��壨Reversi���������������޸�
// UCTS�㷨��������Ч������ʽ��������һ���ھ�����
// ���ߣ�Yuchen
// ��Ϸ��Ϣ��http://www.botzone.org/games#Reversi


#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "jsoncpp/json.h" // C++����ʱĬ�ϰ����˿�
#include <stdio.h>
#include <math.h>
using namespace std;

int review[8][8] = {    //��λ��ֵ������ʽ����������UCTS�㷨
	20, -3, 11,  8,  8, 11, -3, 20,
	-3, -7, -4,  1,  1, -4, -7, -3,
	11, -4,  2,  2,  2,  2, -4, 11,
	8,  1,  2, -3, -3,  2,  1,  8,
	8,  1,  2, -3, -3,  2,  1,  8,
	11, -4,  2,  2,  2,  2, -4, 11,
	-3, -7, -4,  1,  1, -4, -7, -3,
	20, -3, 11,  8,  8, 11, -3, 20,
};


class CTreeNode;	//ǰ��������tNodeLink��CTreeNode���ຬ�б˴�

					//�ڵ�����ṹ��
struct tNodeLink
{
	CTreeNode *pChild;		//���ӽڵ�ĵ�ַ
	tNodeLink *pNext;
};

//���ڵ���
class CTreeNode
{
public:
	friend class Tree;

	CTreeNode();
	CTreeNode(int inx,int iny);
	~CTreeNode();
	int getNum();									//��ȡ���
	float getValue();								//��ȡ�ڵ��ֵ
	void setValue(int c=1);						//���ýڵ��ֵ
	int getFaith();								//��ȡ������ֵ
	void setFaith(int c=1);						//���ô�����ֵ
	tNodeLink *getChildHead();						//ȡ�����ӽڵ�����ͷ
	bool hasFatherNode();							//�ж��Ƿ��и��׽ڵ� ûҪ��
	bool setFatherNode(CTreeNode *pFather);			//���ø��׽ڵ� 
	bool addChildNode(CTreeNode *pChild);	//���Ӷ��ӽڵ�
	bool delChildNode(CTreeNode *pChild);			//ɾ�����ӽڵ�

	int getX();								//��ȡ�ڵ��ֵ
	int getY();								//��ȡ�ڵ��ֵ
	CTreeNode *m_pFather;	//���׽ڵ�
	bool isEnd;
	static int s_max;
	float m_value;			//�ڵ��ֵ
	int x;
	int y;//�ڵ��ֵ
private:
			//�ѷ���ı�ŵ���Ŀ
	int m_num;				//�ڵ���
	
	int faith;
	int win;

	
	tNodeLink *m_pChildHead;//���ӽڵ�����
	
};






int CTreeNode::s_max = 0;	//�ѷ���ڵ���Ŀ��ʼ��Ϊ0

							/*************************************************************/
							/*��CTreeNode*/

							/*��������*/
CTreeNode::CTreeNode()
{
	m_num = ++s_max;	//Ϊ�ڵ����һ�������ı��
	m_value = 0;		//��ʼ���ڵ��ֵ
	faith = 0;
	win = 0;
	x = -1;
	y = -1;
	m_pFather = NULL;	//��ʼ�����׽ڵ�Ϊ��
	m_pChildHead = NULL;//��ʼ����������ͷΪ��
	isEnd = false;
}
CTreeNode::CTreeNode(int inx,int iny)
{
	m_num = ++s_max;	//Ϊ�ڵ����һ�������ı��
	m_value = 0;		//��ʼ���ڵ��ֵ
	faith = 0;
	win = 0;
	x = inx;
	y = iny;
	m_pFather = NULL;	//��ʼ�����׽ڵ�Ϊ��
	m_pChildHead = NULL;//��ʼ����������ͷΪ��
	isEnd = false;
}
/*
��������
1.���Լ��Ӹ��׵Ķ���������ɾ��
2.�������Լ��Ķ����ǵĸ��ӹ�ϵ
3.�ͷŶ�������Ŀռ�
*/
CTreeNode::~CTreeNode()
{
	tNodeLink *pCurrent = this->m_pChildHead;
	tNodeLink *pNext;

	//����и��ף����Լ��Ӹ��׵Ķ���������ɾ��
	if (m_pFather != NULL) m_pFather->delChildNode(this);

	//ɾ�������ǵĸ���
	while (pCurrent != NULL)
	{
		pNext = pCurrent->pNext;

		//�ͷŶ�������Ŀռ�
		(pCurrent->pChild)->setFatherNode(NULL);
		pCurrent = pNext;
	}
}

/*��ȡ���*/
int CTreeNode::getNum()
{
	return m_num;
}

/*��ȡ�ڵ��ֵ*/
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



/*���ýڵ��ֵ*/
void CTreeNode::setValue(int c)
{
	m_value+=c;
}


/*��ȡ�ڵ��ֵ*/
int CTreeNode::getFaith()
{
	return faith;
}

/*���ýڵ��ֵ*/
void CTreeNode::setFaith(int c)
{
	faith+=c;
}


/*��ȡ���ӽڵ�����ͷ��ָ��*/
tNodeLink *CTreeNode::getChildHead()
{
	return m_pChildHead;
}

/*�ж��Ƿ��и��׽ڵ�*/
bool CTreeNode::hasFatherNode()
{
	if (m_pFather == NULL) return false;

	return true;
}

/*
���ø��׽ڵ�
1.���û�и��׽ڵ㣬��ֱ������
2.������и��׽ڵ㣬���Լ��Ӹ��ڵ�Ķ���������ɾ��
*/
bool CTreeNode::setFatherNode(CTreeNode *pFather)
{
	//���и��׽ڵ�
	if (m_pFather != NULL) m_pFather->delChildNode(this);

	//����
	m_pFather = pFather;

	return true;
}

/*
���Ӷ��ӽڵ�
1.����ָ��Ϊ�գ�����false
2.�����Ӽ��뵽����ĩβ
*/
bool CTreeNode::addChildNode(CTreeNode *pChild)
{
	tNodeLink **ppLast = &this->m_pChildHead;
	tNodeLink *pCurrent = this->m_pChildHead;
	tNodeLink *pNewChild;

	//����ָ��Ϊ��
	if (pChild == NULL)
	{
		printf("���󣺶���ָ��Ϊ��\n");
		return false;
	}

	

	//ָ���������һ������
	while (pCurrent != NULL)
	{
		ppLast = &pCurrent->pNext;
		pCurrent = pCurrent->pNext;
	}

	//�����¶���
	pNewChild = new tNodeLink;
	pNewChild->pChild = pChild;
	pNewChild->pChild->setFatherNode(this);
	pNewChild->pNext = NULL;
	*ppLast = pNewChild;

	return true;
}

/*
ɾ�����ӽڵ�
1.����ָ��Ϊ�գ�����false
2.������ж��ӽڵ㣬�򽫶��Ӵ�������ɾ��
3.���û�ж��ӽڵ㣬�򷵻�false
*/
bool CTreeNode::delChildNode(CTreeNode *pChild)
{
	tNodeLink **ppLast = &this->m_pChildHead;
	tNodeLink *pCurrent = this->m_pChildHead;

	//����ָ��Ϊ��
	if (pChild == NULL)
	{
		printf("���󣺶���ָ��Ϊ��\n");
		return false;
	}

	while (pCurrent != NULL)
	{
		//�ҵ����ӽڵ�
		if (pCurrent->pChild == pChild)
		{
			*ppLast = pCurrent->pNext;
			delete pCurrent;

			return true;
		}

		ppLast = &pCurrent->pNext;
		pCurrent = pCurrent->pNext;
	}

	//û�ҵ�����
	return false;
}



/*************************************************************/
/*��CLeafNode*/







/*����ѡ����ţ�ƶ���*/
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
				else if (price==max&&review[pCurrend->pChild->x][pCurrend->pChild->y] > valuemax) {//ȡ��ֵ��õ�����
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
				else if (price == max&&review[pCurrend->pChild->x][pCurrend->pChild->y] > valuemax) {//ȡ��ֵ��õ�����
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






















int currBotColor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[8][8] = { 0 }; // ��x��y����¼����״̬
//int blackPieceCount = 2, whitePieceCount = 2;
int realChoice[2] = { -1 };
int Guess[8][8] = { 0 };
int tempInfo[8][8] = { 0 };
int tempweiCount = 0;//�Ȼ���¼
int allPower = 0;

// ��Direction����Ķ����꣬�������Ƿ�Խ��
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

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
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
				//tempweiCount = weiCount;//���浱ǰ����չ��
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
		tempweiCount = weiCount;//���浱ǰ����չ��
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





//������ӻ�ý��
int randTest(int inx, int iny, int color) {
	int x, y;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++)tempInfo[x][y] = Guess[x][y];
	};	
	if(inx!=-1)ProcStepb(inx, iny, color);
	int colornow = 1;
	if (color == 1)colornow = -1;
	//��ʼ�������
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
			if (true) {//��ֵ�·�
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
		//תɫ
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

	for (x = 0; x < 8; x++) {//��ʼ������
		for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
	};
	int num = 0;
	int strong = 1;//���ǿ��ֵ
	clock_t start;
	start = clock();
	while (((double)(clock() - start) / CLOCKS_PER_SEC)<0.98) {//��������
		
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
			current = root;//���ûظ��ڵ�
			flagColor = true;
			curcolor = currBotColor;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
        else if (temp == NULL&&curcolor == currBotColor) {//�ҷ�����
			int possiblePos[64][2], posCount = 0;
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					if (ProcStepc(x, y, curcolor, true))//���п���
					{
						possiblePos[posCount][0] = x;
						
						possiblePos[posCount++][1] = y;
					}
			if (posCount > 0) {
				int save = 0;//�ӽڵ���е���չ
				int valuemax = -30,cache=0;
				for (int i = 0; i < posCount; i++) {//��ֵ����
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
					if (i == cache) {//for (int d = 0; d < 1; d++) {//ģ������
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
							if (ProcStepc(x, y, tempcolor, true))//��һ���ܷ���
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
			current = root;//���ûظ��ڵ�
			flagColor = true;
			curcolor = currBotColor;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
		else if(temp == NULL&&curcolor != currBotColor){//�ط�����
			int possiblePos[64][2], posCount = 0;
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					if (ProcStepc(x, y, curcolor, true))
					{
						possiblePos[posCount][0] = x;
						
						possiblePos[posCount++][1] = y;
					}
			if (posCount > 0) {
				int save = 0;//�ӽڵ���е���չ
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
							if (ProcStepc(x, y, tempcolor, true))//��һ���ܷ���
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
			current = root;//���ûظ��ڵ�
			curcolor = currBotColor;
			flagColor = true;
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++)Guess[x][y] = gridInfo[x][y];
			};
		}
		else//ѡȡ����ӵ���Ѱ
		{
			current = temp;
			if(current->getX()!=-1)ProcStepc(current->getX(), current->getY(), curcolor);//����
			curcolor *= -1;//��ɫ			
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

	// ��ʼ������
	gridInfo[3][4] = gridInfo[4][3] = 1;  //|��|��|
	gridInfo[3][3] = gridInfo[4][4] = -1; //|��|��|

										  // ����JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : -1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
	for (int i = 0; i < turnID; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		x = input["requests"][i]["x"].asInt();
		y = input["requests"][i]["y"].asInt();
		if (x >= 0)
			ProcStep(x, y, -currBotColor); // ģ��Է�����
		x = input["responses"][i]["x"].asInt();
		y = input["responses"][i]["y"].asInt();
		if (x >= 0)
			ProcStep(x, y, currBotColor); // ģ�⼺������
	}

	// �����Լ����غ�����
	x = input["requests"][turnID]["x"].asInt();
	y = input["requests"][turnID]["y"].asInt();
	if (x >= 0)
		ProcStep(x, y, -currBotColor); // ģ��Է�����

									   // �ҳ��Ϸ����ӵ�
	
	// �������ߣ���ֻ���޸����²��֣�
	
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
	// ���߽���������������ֻ���޸����ϲ��֣�

	Json::Value ret;
	ret["response"]["x"] = resultX;
	ret["response"]["y"] = resultY;
	Json::FastWriter writer;
	std::cout << writer.write(ret) << endl;
	return 0;
}