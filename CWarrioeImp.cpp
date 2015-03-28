#include <iostream>
#include <stdio.h>

#include "CWarrior.h"
using namespace std;

int arrowForce;
void setArrowForce(int af)
{
	arrowForce = af;
}

int bombPower;
void setBombPower(int bp)//用总部的初始生命力值来初始化炸弹的power,如此可以防止整数溢出等问题
{
	bombPower = bp;
}

int loyaltyDecK;
void setLoyaltyDecK(int k)
{
	loyaltyDecK = k;
}

//Implement each methods in CWarrior
CWarrior::CWarrior(CHeadquarter *p, int nNo_, int kind) :steps(0), nNo(nNo_), pHeadquarter(p), nKindNo(kind),
fightResult(OTHER)
{
	if (0 <= nNo && nNo < WARRIOR_NUM)
		wLifeValue = InitialLifeValue[nNo];
	else
		wLifeValue = 0;
	for (int i = 0; i < 2; i++)
		wps[i] = NULL;
}

void CWarrior::TakeCityLifeVal(CCity *city)
{
	//if (this->fightResult == WIN)
	{
		pHeadquarter->nTotalLifeValue += city->cityLifeValue;
		city->cityLifeValue = 0;
	}
}

void CWarrior::GetAwardFromHd(void)
{
	if (pHeadquarter->nTotalLifeValue > AWARD_LIFE_VALUE)
	{
		wLifeValue += AWARD_LIFE_VALUE;
		pHeadquarter->nTotalLifeValue -= AWARD_LIFE_VALUE;
	}
}

//problem, 当前的实现导致武士都不能前进,在主函数中创建临时保存城市中武士指针的数组，同时将城市中
//的武士指针置为空
void CWarrior::StepForward(CCity *next)
{
	steps++;
	for (int i = 0; i < 2; i++)
	{
		if (next->warriorInCity[i] != NULL)
			next->warriorInCity[i] = this;
	}
	if (this->nKindNo == ICEMAN && steps % 2 == 0)
	{
		if (this->wLifeValue > 9)
		{
			this->wLifeValue -= 9;
			this->attackForce += 20;
		}
		else
		{
			this->wLifeValue = 1;
			this->attackForce += 20;
		}
	}
}

int CWarrior::HqColor(void)
{
	return pHeadquarter->nColor;
}

//这个输出函数不适合本题描述
void CWarrior::PrintResult(int nTime,int nKindNo)
{	
		char szColor[20];
		pHeadquarter->GetColor(szColor);
		printf("%0WEAPON_NUMd:00 %s %s %d born with strength %d,%d %s in %s headquarter\n"	,
				nTime, szColor, Names[nKindNo], nNo, InitialLifeValue[nKindNo],
				pHeadquarter->anWarriorNum[nKindNo],Names[nKindNo],szColor);
}

//Implement each method in dragon warrior class
CDragon::CDragon(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind)
{

	int type = nNo % WEAPON_NUM;
	int power = CWarrior::InitialAttackForce[wps[0]->nKindNo];
	wps[type] = WeaponProduce(type, power);//The line ensures which weapon the warrior acquires.
	yell = false;//需要在主动进攻并且取得胜利才将yell置为true
	CountMorale();
}

void CDragon::CountMorale(void)
{
	fmorale = pHeadquarter -> GetTotalLifeValue() /(double)CWarrior::InitialLifeValue [0];
}
void CDragon::PrintResult(int nTime)
{
	CWarrior::PrintResult(nTime, DRAGON);
	printf("It has a %s,and it's morale is %.2f\n", 
		CWeapon::Names[wps[0]->nKindNo], fmorale);
}

//该函数相当于主动进攻,该函数去调用反击函数，
int CDragon::AttackEnemy(CWarrior *enemy, int force)
{
	int tmp = 0;
	int i;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;

	for (i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)//弓箭在主动和被动进攻中都不使用
		{
			wps[i]->UseWeapon(this, enemy);//如果武器是bomb，在bomb函数中进行判断是否使用
		}
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//使用的武器是bomb,在扫描城市时，如果发现一个武士的战斗结果为DRAW，且生命力值为<= 0,则需要析构两个武士
		return DRAW;
	if (enemy->wLifeValue > 0)
		enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		//战斗胜利后做的事
		fmorale += 0.2;//在输出时统一处理, 士气增减发生在欢呼之前
		yell = true;
		this->wLifeValue += tmp;
		return fightResult = WIN;//以此作为标记，在扫描所有城市时再进行奖励和拿走城市生命值
	}
	else
	{
		//执行到此处说明发生反击
		if (enemy->FightBack(this, enemy->attackForce / 2) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		fmorale -= 0.2;
		return fightResult;
	}
}

int CDragon::FightBack(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;

	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL && wps[i]->nKindNo == SWORD)//反击只能使用Sword
			wps[i]->UseWeapon(this, enemy);
	}
	enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		//战斗胜利后的事
		fmorale += 0.2;
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{	//能执行到此处，说明战斗平局 
		fmorale -= 0.2;
		return fightResult = DRAW;
	}
}

CDragon::~CDragon()
{
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL)
			delete wps[0];
	}
}
//Implement each method in ninja warrior class
CNinja::CNinja(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind)
{
	int type1 = nNo % WEAPON_NUM;	
	int power1 = CWarrior::InitialAttackForce[wps[0]->nKindNo];
	wps[type1] = WeaponProduce(type1, power1);
	int type2 = ( nNo + 1) % WEAPON_NUM;	
	int power2 = CWarrior::InitialAttackForce[wps[2]->nKindNo];
	wps[type2] = WeaponProduce(type2, power2);
}

int CNinja::AttackEnemy(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)
			wps[i]->UseWeapon(this, enemy);
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//使用的武器是bomb
		return DRAW;
	if (enemy->wLifeValue > 0)
		enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		fightResult = WIN;
		this->wLifeValue += tmp;
		return fightResult;
	}
	else
	{
		if (enemy->FightBack(this, enemy->attackForce / 2) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		return fightResult;
	}
}

int CNinja::FightBack(CWarrior *enemy, int force)
{
	return DRAW;
}
void CNinja::PrintResult(int nTime)
{	
	CWarrior::PrintResult(nTime,NINJA);	
	//次行输出由于武器的存储结构修改，故而要更改
	printf("It has a %s and a %s\n", 
	CWeapon::Names[wps[0]->nKindNo],
	CWeapon::Names[wps[1]->nKindNo]);
}

CNinja::~CNinja()
{
	for (int i = 0; i < WEAPON_NUM; i++)
		if (wps[i] != NULL)
			delete wps[i];
}

CIceman::CIceman(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind)
{
	int type = nNo % WEAPON_NUM;
	int power = CWarrior::InitialAttackForce[wps[0]->nKindNo];
	wps[type] = WeaponProduce(type, power);
}

int CIceman::AttackEnemy(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)//弓箭在主动和被动进攻中都不使用
		{
			wps[i]->UseWeapon(this, enemy);//如果武器是bomb，在bomb函数中进行判断是否使用
		}
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//使用的武器是bomb
		return DRAW;
	if (enemy->wLifeValue > 0)
		enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		if (enemy->FightBack(this, enemy->attackForce / 2) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		return fightResult;
	}
}

int CIceman::FightBack(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL && wps[i]->nKindNo == SWORD)//反击不使用bomb
			wps[i]->UseWeapon(this, enemy);
	}
	enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		return fightResult == DRAW;
	}
}

void CIceman::PrintResult(int nTime)
{
	CWarrior::PrintResult(nTime, ICEMAN);
	printf("It has a %s\n",
		CWeapon::Names[wps[0]->nKindNo]);
}

CIceman::~CIceman()
{
	//必须做这个判断，因为武器在用尽时会被析构，而此处析构武器是在武士挂了
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		if (wps[i] != NULL)
			delete wps[i];
	}
}
CLion::CLion(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind), loyaltyDec(loyaltyDecK)
{
	nLoyalty = pHeadquarter->GetTotalLifeValue();
}

void CLion::PrintResult(int nTime)
{
	CWarrior::PrintResult(nTime,LION);	
	printf("It's loyalty is %d\n",nLoyalty);
}

void CLion::ChangeLoyalty(int K)
{
	nLoyalty -= K;
}

int CLion::AttackEnemy(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		if (enemy->FightBack(this, enemy->attackForce / 2) == DRAW)
		{
			ChangeLoyalty(loyaltyDec);//Lion 逃跑这件事也放在扫描城市数组的时候进行
			fightResult = DRAW;
		}
		else
		{
			fightResult = LOSE;
		}
		return fightResult;
	}

}

int CLion::FightBack(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		ChangeLoyalty(loyaltyDec);
		return fightResult = DRAW;
	}
}

CLion::~CLion()
{}

CWolf::CWolf(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind)
{}

int CWolf::AttackEnemy(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		//可能出现缴获了两把sword的情况
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)
			wps[i]->UseWeapon(this, enemy);
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//使用的武器是bomb
		return DRAW;
	if (enemy->wLifeValue > 0)
		enemy->wLifeValue -= force;
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		if (enemy->FightBack(this, enemy->attackForce / 2) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		return fightResult;
	}
	
}

int CWolf::FightBack(CWarrior *enemy, int force)
{
	int tmp = 0;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	for (int i = 0; i < WEAPON_NUM; i++)
	{
		//可能出现缴获了两把sword的情况
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)
			wps[i]->UseWeapon(this, enemy);
	}
	if (enemy->wLifeValue > 0)
		enemy->wLifeValue -= force;

	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
		return fightResult = DRAW;
}

void CWolf::PrintResult(int nTime)
{
	CWarrior::PrintResult(nTime, WOLF);
}

CWolf::~CWolf()
{
	for (int i = 0; i < WEAPON_NUM; i++)
		if (wps[i] != NULL)
			delete wps[i];
}
//Implement each method in CCity class
CCity::CCity() :cityLifeValue(0), flagColor(-1),
redWinTimes(0), blueWinTimes(0)
{
	for (int i = 0; i < 2; i++)
		warriorInCity[i] = NULL;
}

inline void CCity::CityLifeValInc(void)
{
	cityLifeValue += 10;
}

//nColor can be got by the Fighting function.
void CCity::CalculateWinTimes()
{
	CWarrior* tmp;
	for (int i = 0; i < 2; i++)
	{
		tmp = warriorInCity[i];
		if (tmp->fightResult == WIN && tmp->HqColor() == RED)
		{
			redWinTimes += 1;
			blueWinTimes = 0;
			if (redWinTimes >= 2)
				flagColor = RED;
		}
		if (tmp->fightResult == WIN && tmp->HqColor() == BLUE)
		{
			redWinTimes = 0;
			blueWinTimes += 1;
			if (blueWinTimes >= 2)
				flagColor = BLUE;
		}

		if (tmp->fightResult == DRAW)
		{
			redWinTimes = 0;
			blueWinTimes = 0;
		}
	}
}

CCity::~CCity() {}

//Implement each method in CWeapon
CWeapon::CWeapon(int type, int force) :nKindNo(type), nForce(force)
{}

CWeapon::~CWeapon(){}

CSword::CSword(int warriorForce) : CWeapon(SWORD, warriorForce*0.2)
{}

//在weapon类中设定UseWeapon函数，在特定武器类中覆盖这个函数	
int CSword::UseWeapon(CWarrior *active, CWarrior *passive)
{
	passive->wLifeValue -= nForce;
	nForce *= 0.8;
	if (nForce == 0)
	{
		delete active->wps[SWORD];
		active->wps[SWORD] == NULL;
	}
	return 0;
}

int CSword::CheckWeaponStatus(void)
{
	return nForce;
}

CSword::~CSword(){}

//Implement the arrow class
CArrow::CArrow() :CWeapon(ARROW, arrowForce), useTimes(3)
{}

int CArrow::UseWeapon(CWarrior *active, CWarrior *passive)
{
	//判断武士是否拥有Arrow
	if (active->wps[ARROW] != NULL && useTimes > 0)
	{
			passive->wLifeValue -= nForce;
			useTimes -= 1;
	}
	if (useTimes == 0)
	{
		delete active->wps[ARROW];
		active->wps[ARROW] == NULL;
	}
	return 0;
}
int CArrow::CheckWeaponStatus(void)
{
	return useTimes;
}

CArrow::~CArrow(){}
//Implement bomb class
CBomb::CBomb() :CWeapon(BOMB, bombPower), bombUsed(false)
{}

//在该函数评估是否使用炸弹, FightBack函数保证了只有AttackEnemy函数才可能使用Bomb
//故而此函数中需要判断passive的攻击力的一半和sword攻击
//总和是否大于active的生命值
int CBomb::UseWeapon(CWarrior *active, CWarrior *passive)
{
	if (active->wLifeValue <= passive->wps[SWORD]->nForce + passive->attackForce/2)
	{
		active->wLifeValue = 0;
		passive->wLifeValue = 0;
		delete active->wps[BOMB];
		active->wps[BOMB] = NULL;
	}
	return 0;
}

int CBomb::CheckWeaponStatus()
{
	if (bombUsed)
		return BOMBUSED;
	else
		return BOMBNOTUSED;
}

CBomb::~CBomb(){}
class CDragon;
class CNinja;
class CIceman;
class CLion;
class CWolf;

//Implement CHeadquater
CHeadquarter::CHeadquarter() :nCurMakingSeqIdx(0), nTotalWarriorNum(0), bStopped(0)
{
	for (int i = 0; i < WARRIOR_NUM; i++)
		anWarriorNum[i] = 0;
}
void CHeadquarter::Init(int nColor_, int lv)
{
	nColor = nColor_;
	nTotalLifeValue = lv;
}
CHeadquarter::~CHeadquarter () {
	for(int i = 0;i < nTotalWarriorNum; i ++ )
		delete pWarriors[i];		
}
int CHeadquarter::Produce(int nTime)
{
	//老师的这部分代码是去寻找总部可以制造的武士类型，但本题目中不寻找只等待
	//while( CWarrior::InitialLifeValue[MakingSeq[nColor][nCurMakingSeqIdx]] > nTotalLifeValue && 
	//	nSearchingTimes < WARRIOR_NUM ) {
	//	nCurMakingSeqIdx = ( nCurMakingSeqIdx + 1 ) % WARRIOR_NUM ;
	//	nSearchingTimes ++;
	//}
	int nKindNo = MakingSeq[nColor][nCurMakingSeqIdx];

	//测试代码
	if (CWarrior::InitialLifeValue[nKindNo] > nTotalLifeValue)
	{
		char headColor[20];
		GetColor(headColor);
		cout << headColor << "headquater is waiting to produce: " 
			<< CWarrior::Names[nKindNo] << endl;
		return 0;
	}
		
	//这部分代码说明上面注释掉的while循环是因为总部生命值过低不可以制造的武士了
	//if( CWarrior::InitialLifeValue[nKindNo] > nTotalLifeValue ) {
	//	bStopped = true;
	//	if( nColor == RED)
	//		printf("%0WEAPON_NUMd red headquarter stops making warriors\n",nTime);
	//	else
	//		printf("%0WEAPON_NUMd blue headquarter stops making warriors\n",nTime);
	//	return 0;
	//}
	nTotalLifeValue -= CWarrior::InitialLifeValue[nKindNo];//在实际武士生成之前，总部的生命值已经扣掉了要生成的武士的生命值
	nCurMakingSeqIdx = ( nCurMakingSeqIdx + 1 ) % WARRIOR_NUM ;
	
	//int nTmp = anWarriorNum[nKindNo]; //统计每种武士生产了多少个
	//anWarriorNum[nKindNo] ++;

	switch( nKindNo ) {
		case DRAGON:
			pWarriors[nTotalWarriorNum] = new CDragon(this, nTotalWarriorNum + 1, nKindNo);
			break;
		case NINJA:
			pWarriors[nTotalWarriorNum] = new CNinja(this, nTotalWarriorNum + 1, nKindNo);
			break;
		case ICEMAN:
			pWarriors[nTotalWarriorNum] = new CIceman(this, nTotalWarriorNum + 1, nKindNo);
			break;
		case LION:
			pWarriors[nTotalWarriorNum] = new CLion(this, nTotalWarriorNum + 1, nKindNo);
			break;
		case WOLF:
			pWarriors[nTotalWarriorNum] = new CWolf(this, nTotalWarriorNum + 1, nKindNo);
			break;

	}
	pWarriors[nTotalWarriorNum]->PrintResult(nTime);//打印第一种输出
	nTotalWarriorNum ++;
	return 1;
}
void CHeadquarter::GetColor( char * szColor)
{
	if( nColor == 0)
		strcpy(szColor,"red");
	else
		strcpy(szColor,"blue");
}

const char * CWeapon::Names[WEAPON_NUM] = {"sword","bomb","arrow" };

const char * CWarrior::Names[WARRIOR_NUM] = { "dragon","ninja","iceman","lion","wolf" };
int CWarrior::InitialLifeValue [WARRIOR_NUM];
int CWarrior::InitialAttackForce[WARRIOR_NUM];
int CHeadquarter::MakingSeq[2][WARRIOR_NUM] = { { 2,WEAPON_NUM,4,1,0 },{WEAPON_NUM,0,1,2,4} };

CWeapon* WeaponProduce(int nKindNo, int nForce)
{
	switch (nKindNo)
	{
	case SWORD:
		return new CSword(nForce);
		break;
	case ARROW:
		return new CArrow();
		break;
	case BOMB:
		return new CBomb();
		break;
	}
}
