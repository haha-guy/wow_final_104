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
�췽˾����� iceman��lion��wolf��ninja��dragon ��˳��������ʿ�� 
����˾����� lion��dragon��ninja��iceman��wolf ��˳��������ʿ�� 
*/

//����Ƶ�һ��������һЩ���Թ̶��趨�ĳ�Ա�����ڹ��캯���н���
//����Ҫ��̬���ⲿ�����ӻ�������Խ��г�ʼ���ĳ�Ա��������Init����, �˺�����һ�����������е���
//����Ҳ��ֱ�ۣ���Ϊֻ������������֪����ص�������Ϣ
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
	public:
		CHeadquarter *pHeadquarter;
		int steps;
		int nNo;//��ʿ�������е����
		int fightResult;//ս�����-1 ����0 ƽ�� 1 ʤ����ʼֵ-2��������ʿ����һ������ǰ��ʱ����Ҫ�޸�ս�����Ϊ-2
		int nKindNo;
		int wLifeValue;
		int attackForce;
		CWeapon *wps[3];//������ȻӦ���ǿɼ���,��Ӧ������������Ӧ��λ��

		static const char * Names[WARRIOR_NUM];
		static int InitialLifeValue [WARRIOR_NUM];
		static int InitialAttackForce[WARRIOR_NUM];
		CWarrior( CHeadquarter * p,int nNo_, int kind);
		void TakeCityLifeVal(CCity *city);
		void GetAwardFromHd(void);
		int HqColor(void);
		void StepForward(CCity *next);	//StepForward �ı�steps��ֵ��������ҵ���Ӧ������
		
/******AttackEnemy �� FightBack ����˵��*********
*��Ŀ�趨��֤һ���������ֻ��һ��ĳһ������ʿ����һ�����г���������ʿ����
*Ҫ����ս��.ͨ��ɨ��CCity��������飬�Ծ����Ƿ���ս���������ͽ�ʱ�临�Ӷ�
*��������O(N).�ú����������Ҫ�Ĺ��ܣ���Ҫ�и��ܲ��������н������޸���������������
*/
		virtual int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		virtual int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
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
		int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
		void CountMorale(void);
		void PrintResult(int nTime);
		double ReportDragonMoral(void){ return fmorale; }
		~CDragon();
};

class CNinja :public CWarrior
{
	public:
		CNinja(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
		void PrintResult(int nTime);
		~CNinja();
};

class CIceman :public CWarrior
{
	public:
		CIceman(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
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
		int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
		int ReportLionLoyalty(void) { return nLoyalty; };
		void ChangeLoyalty(int K);//�ı��ҳ϶�
		void PrintResult(int nTime);
		~CLion();
};

class CWolf :public CWarrior
{
	public:
		CWolf(CHeadquarter * p, int nNo_, int kind);
		int AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId);
		int FightBack(CWarrior *enemy, int force, int h, int m, int cityId);
		void wolfSeizeWeapon(CWarrior *enemy);
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
		CWarrior *warriorInCity[2];//�������б������ж��󹹳ɵ�����,�������Ƿ���������ʿ�Ծ����Ƿ���ս��

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
		
		//���ض�������ʹ�÷�װ��UseWeapon���������,��ÿ�����������Լ�������ʹ�ú���
		//��UseBomb���������е�UseWeapon����UseBomb
		//UseWeapon����ԭ�͵ĺ��塰˭��˭ʹ��������
		//��UseWeapon���ж������Ƿ���ʧ
		virtual int UseWeapon(CWarrior *active, CWarrior *passive) = 0;
		virtual int CheckWeaponStatus(void) = 0;//Ϊ��ʿ�����Ƿ�ʧȥ�������ṩ�ӿ�
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

CWeapon* WeaponProduce(int nKindNo, int nForce);//ʹ�øú����������������󣬲�����������Ӧ����ʿ
#endif

/*����ÿһ�����У�ɨ��������ʿ������״̬���������Ƿ���ʧ��
�����ʿ�Ƿ��������ʿ�Ƿ���ʧ��
Ȼ���ܲ�������ʤ��ʿ��
Ȼ����ʿȡ�ó�������ֵ��
Ȼ�󽫳��е���ʿָ���ݴ沢����������ʿָ����գ�
Ȼ����ʿǰ��
*/