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
void setBombPower(int bp)//���ܲ��ĳ�ʼ������ֵ����ʼ��ը����power,��˿��Է�ֹ�������������
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

//problem, ��ǰ��ʵ�ֵ�����ʿ������ǰ��,���������д�����ʱ�����������ʿָ������飬ͬʱ��������
//����ʿָ����Ϊ��
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

//�������������ʺϱ�������
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
	yell = false;//��Ҫ��������������ȡ��ʤ���Ž�yell��Ϊtrue
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
static void AttackEnemyOutp(int h, int m, char *aHead, const char *aName, int aNo, 
	char *pHead, const char *pName, int pNo, int cityId, int wLv, int wF)
{
	printf("%03d:%02d %s %s %d attacked %s %s %d in city %d with %d elements and force %d",
		h, m,
		aHead, aName, aNo,
		pHead, pName, pNo,
		cityId, wLv, wF);
}//�����������
static void FightBackOutp();


//������������ս���������������ʿ�Լ�һ��һ��
//��������������ʿʬ�彫���ڳ����У�������Ҫ��ǰ�������н�����Ӧ�ļ�⣩
//������һ����ս��������������Ҳ��Ҫ�жϵз���ʿ�����ǻ�,��ص�ս�������ս�������н���
//�ú����൱����������,�ú���ȥ���÷�������
int CDragon::AttackEnemy(CWarrior *enemy, int force, int h, int m, int cityId)
{
	char aheadColor[10];
	char pheadColor[10];
	int tmp = 0;
	int i;
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;

	//������������
	//ʹ���������й���
	// �Լ��ǻ�ĵз�Ҫ�ǻ�Ĳ�ʹ����������������ֻʹ��sword
	if (this->wLifeValue > 0 && enemy->wLifeValue > 0
		&& wps[SWORD] != NULL)
	{
		wps[SWORD]->UseWeapon(this, enemy);
	}
	//ʹ�ù��������й���
	if (this->wLifeValue > 0 && enemy->wLifeValue > 0)
	{
		this->pHeadquarter->GetColor(aheadColor);
		enemy->pHeadquarter->GetColor(pheadColor);
		AttackEnemyOutp(h, m,
			aheadColor, this->Names[this->nKindNo], this->nNo,
			pheadColor, enemy->Names[enemy->nKindNo], enemy->nNo,
			cityId, this->wLifeValue, this->attackForce);//�������������ڵ���˫�����ǻ�������
		enemy->wLifeValue -= force;
	}
	if (enemy->wLifeValue <= 0)//����������˱��������ͱ�ɱ����������˴�����������һ���ǻ��
	{
		//ս��ʤ����������
		//�˴��������ֺ��壺һ�������������������ģ� �����������¶Է�����
		fmorale += 0.2;
		yell = true;
		this->wLifeValue += tmp;
		return fightResult = WIN;//�Դ���Ϊ��ǣ���ɨ�����г���ʱ�ٽ��н��������߳�������ֵ
	}

	//��������������
	//ִ�е��˴���˵���Է�û�б�ɱ�������������������һ�������������������ʿ���������ĺ�������������ʿ������������ɱ��
	{
		//ִ�е��˴�˵����������,�����ȿ�����������������ʿ���������ĵ����������ȹ�������Ȩ����ʱҲ�ᷢ������
		//������ʱû�з������
		//Ҳ������ս����û����������ɱ������ʱ�з�������������������ڷ���������������ʿ������
		if (enemy->FightBack(this, enemy->attackForce / 2, h, m, cityId) == DRAW)
			fightResult = DRAW;
		else
			fightResult = LOSE;
		fmorale -= 0.2;
		return fightResult;
	}
}

int CDragon::FightBack(CWarrior *enemy, int force, int h, int m, int cityId)
{
	int tmp = 0;
	char aheadColor[10];
	char pheadColor[10];
	if (enemy->nKindNo == LION)
		tmp = enemy->wLifeValue;
	if (enemy->wLifeValue > 0)
	{
		if (wps[SWORD] != NULL)//����ֻ��ʹ��Sword
			wps[SWORD]->UseWeapon(this, enemy);
		this->pHeadquarter->GetColor(aheadColor);
		enemy->pHeadquarter->GetColor(pheadColor);
		printf("%03d:%02d %s %s %d attacked %s %s %d in city %d",
			h, m,
			aheadColor, this->Names[this->nKindNo], this->nNo,
			pheadColor, enemy->Names[enemy->nKindNo], enemy->nNo,
			cityId);
		enemy->wLifeValue -= force;
	}
	if (enemy->wLifeValue <= 0)
	{
		//ս��ʤ�������
		//�˴��������ֺ��壺һ�Ƿ������������ģ� �����������¶Է�����
		fmorale += 0.2;
		this->wLifeValue += tmp;
		return fightResult = WIN;
	}
	else
	{	//��ִ�е��˴���˵��ս��ƽ�� 
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
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//ʹ�õ�������bomb
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
	//����������������Ĵ洢�ṹ�޸ģ��ʶ�Ҫ����
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
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)//�����������ͱ��������ж���ʹ��
		{
			wps[i]->UseWeapon(this, enemy);//���������bomb����bomb�����н����ж��Ƿ�ʹ��
		}
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//ʹ�õ�������bomb
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
		if (wps[i] != NULL && wps[i]->nKindNo == SWORD)//������ʹ��bomb
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
	//����������жϣ���Ϊ�������þ�ʱ�ᱻ���������˴���������������ʿ����
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
			ChangeLoyalty(loyaltyDec);//Lion ���������Ҳ����ɨ����������ʱ�����
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
		//���ܳ��ֽɻ�������sword�����
		if (wps[i] != NULL && wps[i]->nKindNo != ARROW)
			wps[i]->UseWeapon(this, enemy);
	}
	if (enemy->wLifeValue == 0 && this->wLifeValue == 0)//ʹ�õ�������bomb
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
		//���ܳ��ֽɻ�������sword�����
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

//��weapon�����趨UseWeapon���������ض��������и����������	
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
	//�ж���ʿ�Ƿ�ӵ��Arrow
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

//�ڸú��������Ƿ�ʹ��ը��, FightBack������֤��ֻ��AttackEnemy�����ſ���ʹ��Bomb
//�ʶ��˺�������Ҫ�ж�passive�Ĺ�������һ���sword����
//�ܺ��Ƿ����active������ֵ
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
	//��ʦ���ⲿ�ִ�����ȥѰ���ܲ������������ʿ���ͣ�������Ŀ�в�Ѱ��ֻ�ȴ�
	//while( CWarrior::InitialLifeValue[MakingSeq[nColor][nCurMakingSeqIdx]] > nTotalLifeValue && 
	//	nSearchingTimes < WARRIOR_NUM ) {
	//	nCurMakingSeqIdx = ( nCurMakingSeqIdx + 1 ) % WARRIOR_NUM ;
	//	nSearchingTimes ++;
	//}
	int nKindNo = MakingSeq[nColor][nCurMakingSeqIdx];

	//���Դ���
	if (CWarrior::InitialLifeValue[nKindNo] > nTotalLifeValue)
	{
		char headColor[20];
		GetColor(headColor);
		cout << headColor << "headquater is waiting to produce: " 
			<< CWarrior::Names[nKindNo] << endl;
		return 0;
	}
		
	//�ⲿ�ִ���˵������ע�͵���whileѭ������Ϊ�ܲ�����ֵ���Ͳ������������ʿ��
	//if( CWarrior::InitialLifeValue[nKindNo] > nTotalLifeValue ) {
	//	bStopped = true;
	//	if( nColor == RED)
	//		printf("%0WEAPON_NUMd red headquarter stops making warriors\n",nTime);
	//	else
	//		printf("%0WEAPON_NUMd blue headquarter stops making warriors\n",nTime);
	//	return 0;
	//}
	nTotalLifeValue -= CWarrior::InitialLifeValue[nKindNo];//��ʵ����ʿ����֮ǰ���ܲ�������ֵ�Ѿ��۵���Ҫ���ɵ���ʿ������ֵ
	nCurMakingSeqIdx = ( nCurMakingSeqIdx + 1 ) % WARRIOR_NUM ;
	
	//int nTmp = anWarriorNum[nKindNo]; //ͳ��ÿ����ʿ�����˶��ٸ�
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
	pWarriors[nTotalWarriorNum]->PrintResult(nTime);//��ӡ��һ�����
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
