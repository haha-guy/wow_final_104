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

static void AttackEnemyOutp(int h, int m, char *aHead, const char *aName, int aNo, 
	char *pHead, const char *pName, int pNo, int cityId, int wLv, int wF)
{
	printf("%03d:%02d %s %s %d attacked %s %s %d in city %d with %d elements and force %d",
		h, m,
		aHead, aName, aNo,
		pHead, pName, pNo,
		cityId, wLv, wF);
}//主动进攻输出

static void FightBackOutp(int h, int m, char *aHead, const char *aName, int aNo,
	char *pHead, const char *pName, int pNo, int cityId)
{
	printf("%03d:%02d %s %s %d attacked %s %s %d in city %d",
		h, m,
		aHead, aName, aNo,
		pHead, pName, pNo,
		cityId);
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

void CWarrior::TakeCityLifeVal(CCity &city)
{
	//if (this->fightResult == WIN)
	{
		pHeadquarter->lvFromCity += city.cityLifeValue;
		city.cityLifeValue = 0;
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


//这个函数处理的战斗包括两个活的武士以及一死一活
//将被箭射死的武士尸体将留在城市中（这样需要在前进函数中进行相应的检测）
//但这样一来，战斗函数反击函数也需要判断敌方武士是死是活,相关的战斗输出在战斗函数中进行
//该函数相当于主动进攻,该函数去调用反击函数
int CWarrior::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	char aheadColor[10];
	char pheadColor[10];
	int tmp = 0;
	int i;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;

	//主动攻击发生
	//使用武器进行攻击
	// 自己是活的敌方要是活的才使用武器，主动攻击只使用sword
	if (this->wLifeValue > 0 && enemy->wLifeValue > 0
		&& wps[SWORD] != NULL)
	{
		wps[SWORD]->UseWeapon(this, enemy);
	}
	//使用攻击力进行攻击
	if (this->wLifeValue > 0 && enemy->wLifeValue > 0)
	{
		this->pHeadquarter->GetColor(aheadColor);
		enemy->pHeadquarter->GetColor(pheadColor);
		AttackEnemyOutp(h, m,
			aheadColor, this->Names[this->nKindNo], this->nNo,
			pheadColor, enemy->Names[enemy->nKindNo], enemy->nNo,
			cityId, this->wLifeValue, this->attackForce);//产生这个输出是在敌我双方都是活的情况下
		enemy->wLifeValue -= force;
	}
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}

	//被动攻击发生了
	//执行到此处，说明对方没有被杀死，有两种情况导致这一结果，
	//即主动攻击的武士本身是死的和主动攻击的武士攻击力不足以杀死
	else
	{
		//执行到此处说明发生反击,反击既可能是主动攻击的武士本身是死的但是它有优先攻击的特权，此时也会发生反击
		//但是这时没有反击输出
		//也可能是战斗中没被主动攻击杀死，此时有反击输出，二者区分在于发起主动攻击的武士的死活
		if (enemy->FightBack(this, enemy->attackForce / 2, h, m, cityId) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		return fightResult;
	}

}//返回战斗结果

int CWarrior::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	int tmp = 0;
	char aheadColor[10];
	char pheadColor[10];
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	if (enemy->wLifeValue > 0)
	{
		if (wps[SWORD] != NULL)//反击只能使用Sword
			wps[SWORD]->UseWeapon(this, enemy);
		this->pHeadquarter->GetColor(aheadColor);
		enemy->pHeadquarter->GetColor(pheadColor);

		FightBackOutp(h, m, aheadColor, this->Names[this->nKindNo], this->nNo,
			pheadColor, enemy->Names[enemy->nKindNo], enemy->nNo,
			cityId);
		enemy->wLifeValue -= force;
	}
	if (enemy->wLifeValue <= 0)
	{
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{
		//能执行到此处，说明战斗平局 
		return fightResult = DRAW;
	}
};//基类的FightBack函数返回战斗结果，在派生类中进行派生类的相关设置

int CWarrior::HqColor(void)
{
	return pHeadquarter->nColor;
}

//这个输出函数不适合本题描述, 需要进行相应改造是满足1st输出
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



int CDragon::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{

	int result = CWarrior::AttackEnemy(enemy, force, h, m, cityId);
	if (result == WIN)//这里包括敌人被箭射死和被杀死的情况，此处主动攻击的一定是活的
	{
		//战斗胜利后做的事
		//此处包含两种含义：一是主动攻击对象是死的， 二是自身攻击致对方死掉
		fmorale += 0.2;
		yell = true;
		return fightResult;//以此作为标记，在扫描所有城市时再进行奖励和拿走城市生命值
	}
	else
	{
		fmorale -= 0.2;
		return fightResult;
	}
}

int CDragon::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	int result = CWarrior::FightBack(enemy, force, h, m, cityId);
	if (result == WIN)
	{
		//战斗胜利后的事
		//此处包含两种含义：一是反击对象是死的， 二是自身反击致对方死掉
		fmorale += 0.2;
		return WIN;
	}
	else
	{	//能执行到此处，说明战斗平局 
		fmorale -= 0.2;
		return DRAW;
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

int CNinja::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	CWarrior::AttackEnemy(enemy, force, h, m, cityId);
	return fightResult;
}

int CNinja::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
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

int CIceman::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	return CWarrior::AttackEnemy(enemy, force, h, m, cityId);
}

int CIceman::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	return CWarrior::FightBack(enemy, force, h, m, cityId);
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

int CLion::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	if (CWarrior::AttackEnemy(enemy, force, h, m, cityId) != WIN)
		ChangeLoyalty(loyaltyDec);
	return fightResult;
}

int CLion::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	if (CWarrior::FightBack(enemy, force, h, m, cityId) != WIN)
		ChangeLoyalty(loyaltyDec);
	return fightResult;
}

CLion::~CLion()
{}

CWolf::CWolf(CHeadquarter *p, int nNo_, int kind) :CWarrior(p, nNo_, kind)
{}

void CWolf::wolfSeizeWeapon(CWarrior *enemy)
{
	for (int i = 0; i < 3; i++)
		if (this->wps[i] == NULL)
		{
		this->wps[i] = enemy->wps[i];
		enemy->wps[i] = NULL;
		}
}
int CWolf::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	if (CWarrior::AttackEnemy(enemy, force, h, m, cityId) == WIN)
		wolfSeizeWeapon(enemy);
}

int CWolf::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	if (CWarrior::FightBack(enemy, force, h, m, cityId) == WIN);
		wolfSeizeWeapon(enemy);

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
	//还需要输出旗帜升起
	CWarrior *red = warriorInCity[RED], *blue = warriorInCity[BLUE];
	if (red->fightResult == WIN)
	{
		blueWinTimes = 0;
		redWinTimes += 1;
		if (redWinTimes >= 2)
		{
			flagColor = RED;
			redWinTimes = 0;
		}
	}
	if (blue->fightResult == WIN)
	{
		redWinTimes = 0;
		blueWinTimes += 1;
		if (blueWinTimes >= 2)
		{
			flagColor = BLUE;
			blueWinTimes = 0;
		}
	}
	if (red->fightResult == DRAW || blue->fightResult == DRAW)
	{
		redWinTimes = 0;
		blueWinTimes = 0;
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
CHeadquarter::CHeadquarter() :nCurMakingSeqIdx(0), nTotalWarriorNum(0), bStopped(0), lvFromCity(0)
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
void CHeadquarter::HqAccCityLv(void)
{
	nTotalLifeValue += lvFromCity;
	lvFromCity = 0;
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
