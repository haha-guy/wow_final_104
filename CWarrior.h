#ifndef _CWARRIOR_H
#define _CWARRIOR_H

#include <iostream>
#include <stdio.h>
#include <climits>
using namespace std;

#define WARRIOR_NUM 5
#define WEAPON_NUM 3
#define MAX_WARRIORS 1000
#define AWARD_LIFE_VALUE 8
enum {DRAGON, NINJA, ICEMAN, LION, WOLF};
enum {RED, BLUE};
enum {SWORD, ARROW, BOMB};
enum {LOSE=-1, DRAW=0, WIN=1, OTHER = -2};
enum {BOMBUSED, BOMBNOTUSED};
 
extern void setArrowForce(int af);
extern void setBombPower(int bp);
extern void setLoyaltyDecK(int k);
/*
char * CWarrior::Names[WARRIOR_NUM] = { "dragon","ninja","iceman","lion","wolf" };
红方司令部按照 iceman、lion、wolf、ninja、dragon 的顺序制造武士。 
蓝方司令部按照 lion、dragon、ninja、iceman、wolf 的顺序制造武士。 
*/

//类设计的一个方案，一些可以固定设定的成员变量在构造函数中进行
//而需要动态从外部函数从获得数据以进行初始化的成员变量配置Init函数, 此函数在一般在主函数中调用
//理由也很直观，因为只有在主函数中知道相关的配置信息
class CWeapon;
class CWarrior;
class CCity;
class CHeadquarter
{
	private:
		int nTotalLifeValue;
		bool bStopped;
		int nColor;
		int nCurMakingSeqIdx;
		int anWarriorNum[WARRIOR_NUM];
	public:
		int nTotalWarriorNum;
		CWarrior * pWarriors[MAX_WARRIORS];
		int enemies;//if enemies == 2, war stops
		friend class CWarrior;
		static int MakingSeq[2][WARRIOR_NUM];
		CHeadquarter();
		void Init(int nColor_, int lv);
		int Produce(int nTime);
		void GetColor( char * szColor);
		int GetColor(void){ return nColor; };
		int GetTotalLifeValue() { return nTotalLifeValue; }
		~CHeadquarter();
};

class CWarrior
{
	protected:
		CHeadquarter *pHeadquarter;
		int steps;
	public:
		int nNo;//武士在生产中的序号
		int fightResult;//战斗结果-1 负，0 平， 1 胜，初始值-2；但是武士往下一个城市前进时，需要修改战斗结果为-2
		int nKindNo;
		int wLifeValue;
		int attackForce;
		CWeapon *wps[3];//武器当然应该是可见的,对应的武器挂着相应的位置

		static const char * Names[WARRIOR_NUM];
		static int InitialLifeValue [WARRIOR_NUM];
		static int InitialAttackForce[WARRIOR_NUM];
		CWarrior( CHeadquarter * p,int nNo_, int kind);
		void TakeCityLifeVal(CCity *city);
		void GetAwardFromHd(void);
		int HqColor(void);
		void StepForward(CCity *next);	//StepForward 改变steps的值并将自身挂到相应城市中
		
/******AttackEnemy 和 FightBack 函数说明*********
*题目设定保证一座城市最多只有一个某一方的武士，当一座城市出现两个武士，则
*要发生战斗.通过扫描CCity对象的数组，以决定是否发生战斗，这样就将时间复杂度
*控制在了O(N).该函数完成最主要的功能，主要有跟总部，跟城市交互，修改自身武器的属性
*/
		virtual int AttackEnemy(CWarrior *enemy, int force) = 0;//返回战斗结果
		virtual int FightBack(CWarrior *enemy, int force) = 0;//返回战斗结果
		
		virtual int ReportLionLoyalty(void){ return 0; };
		virtual double ReportDragonMoral(void) { return 0.0; }
		virtual void PrintResult(int nTime,int nKindNo);
		virtual void PrintResult(int nTime) = 0;
		virtual ~CWarrior() { }
}; 

//specified warrior classes
class CDragon :public CWarrior
{
	private:
		double fmorale;	
	public:
		bool yell;
		CDragon(CHeadquarter *p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force);
		int FightBack(CWarrior *enemy, int force);
		void CountMorale(void);
		void PrintResult(int nTime);
		double ReportDragonMoral(void){ return fmorale; }
		~CDragon();
};

class CNinja :public CWarrior
{
	public:
		CNinja(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force);
		int FightBack(CWarrior *enemy, int force);
		void PrintResult(int nTime);
		~CNinja();
};

class CIceman :public CWarrior
{
	public:
		CIceman(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force);
		int FightBack(CWarrior *enemy, int force);
		void PrintResult(int nTime);
		~CIceman();
};

class CLion:public CWarrior
{
	private:
		int nLoyalty;
		int loyaltyDec;
	public:
		CLion(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force);
		int FightBack(CWarrior *enemy, int force);
		int ReportLionLoyalty(void) { return nLoyalty; };
		void ChangeLoyalty(int K);//改变忠诚度
		void PrintResult(int nTime);
		~CLion();
};

class CWolf :public CWarrior
{
	public:
		CWolf(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force);
		int FightBack(CWarrior *enemy, int force);
		void PrintResult(int nTime);
		~CWolf();
};

//the city class

class CCity
{
	public:
		bool killedByArrow;
		int cityLifeValue;
		int flagColor;
		int redWinTimes;
		int blueWinTimes;
		int cityID;
		CWarrior *warriorInCity[2];//主函数中遍历城市对象构成的数组,看其中是否有两个武士以决定是否发生战斗

		CCity();
		void cityInit(int i){ cityID = i; };
		inline void CityLifeValInc(void);
		void CalculateWinTimes(void);
		friend void CWarrior::StepForward(CCity *next);
		friend void CWarrior::TakeCityLifeVal(CCity *city);
		~CCity();
};

class CWeapon
{
	public:
		int nKindNo;
		int nForce;
		static const char * Names[WEAPON_NUM];
		CWeapon(int type, int force);
		
		//将特定武器的使用封装在UseWeapon这个函数中,即每个武器定义自己的武器使用函数
		//如UseBomb，在子类中的UseWeapon调用UseBomb
		//UseWeapon函数原型的含义“谁对谁使用武器”
		//在UseWeapon中判断武器是否消失
		virtual int UseWeapon(CWarrior *active, CWarrior *passive) = 0;
		virtual int CheckWeaponStatus(void) = 0;//为武士决定是否失去了武器提供接口
		virtual ~CWeapon();
};

class CSword :public CWeapon
{
	public:
		CSword(int wf);
		int UseWeapon(CWarrior *active, CWarrior *passive);
		int CheckWeaponStatus(void);
		~CSword();
};

class CArrow :public CWeapon
{
	private:
		int useTimes;
	public:
		CArrow();
		int UseWeapon(CWarrior *active, CWarrior *passive);
		int CheckWeaponStatus(void);
		~CArrow();
};

class CBomb:public CWeapon
{
	private:
		bool bombUsed;
	public:
		CBomb();
		int CheckWeaponStatus(void);
		int UseWeapon(CWarrior *active, CWarrior *passive);
		~CBomb();
};

CWeapon* WeaponProduce(int nKindNo, int nForce);//使用该函数来生成武器对象，并将武器给相应的武士
#endif

/*对于每一座城市，扫描其中武士的武器状态决定武器是否消失；
检查武士是否存活决定武士是否消失；
然后总部奖励获胜武士；
然后武士取得城市生命值；
然后将城市的武士指针暂存并将城市中武士指针清空；
然后武士前进
*/