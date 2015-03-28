#include <iostream>
#include "CWarrior.h"
using namespace std;

int minutes[10] = {0, 5, 10, 20, 30, 35, 38, 40, 50, 55};
bool isHeadTaken(int h, int m, CHeadquarter &hd);
void hqTaken(int h, int m, int color);
void LionFlee(CCity &city, int h, int m);

//几个亮点：将red的武士挂在城市中的[RED],将武器挂在武士中相应的位置

int main()
{
	int t;
	int M, N, R, K, T;
	//freopen("war2.in","r",stdin);
	CHeadquarter RedHead, BlueHead;
	scanf("%d", &t);
	int nCaseNo = 1;
	while (t--) {
		printf("Case:%d\n", nCaseNo++);
		cin >> M >> N >> R >> K >> T;
		for (int i = 0; i < WARRIOR_NUM; i++)
			scanf("%d", &CWarrior::InitialLifeValue[i]);
		for (int i = 0; i < WARRIOR_NUM; i++)
			scanf("%d", &CWarrior::InitialAttackForce[i]);

		int i;
		RedHead.Init(RED, M);
		BlueHead.Init(BLUE, M);

		setArrowForce(R), setBombPower(M), setLoyaltyDecK(K);

		CCity *cities = (CCity*) new CCity[N];
		CCity *cityForTmp = (CCity*) new CCity[N];
		for (i = 0; i < N; i++)
			cities[i].cityID = i;
		int nHour = 0;
		int *nMinute = minutes;
		while (T) {
			//此处将产生1st输出
			RedHead.Produce(nHour);
			BlueHead.Produce(nHour);

			//2sec,此处还需要处理总部处的可能逃跑的Lion,目前尚未处理
			nMinute++;
			for (i = 0; i < N; i++)
			{
				LionFlee(cities[i], nHour, *nMinute);
			}

			//3rd, 先写上，然后再考虑封装成函数,这部分需要从新思考
			//总部的往前走的武士为pWarriors[nTatalWarriorNum],当其往前走后
			//需要将该位置置为NULL
			//武士往前走后，还需要对城市中的killedByArrow置为false
			nMinute++;
			{
				for (i = 0; i < N; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						cityForTmp[i].warriorInCity[j] = cities[i].warriorInCity[j];
						cities[i].warriorInCity[j] = NULL;
					}
				}
				//走向总部的武士,如果武士到达总部后即将总部的enemy加一，然后武士析构
				CWarrior *blueWarrToHq = cityForTmp[0].warriorInCity[BLUE];
				if (blueWarrToHq != NULL)
				{
					printf("%03d:%02d blue %s %d reached red headquarter with %d elements and %d force",
						nHour, *minutes, blueWarrToHq->Names[blueWarrToHq->nKindNo],
						blueWarrToHq->wLifeValue, blueWarrToHq->attackForce);
					RedHead.enemies += 1;
					blueWarrToHq->~CWarrior();
					blueWarrToHq = NULL;
				}


				//总部的武士, 此处写成函数才能同时进行武士前进和 城市事件输出
				cities[0].warriorInCity[RED] = RedHead.pWarriors[RedHead.nTotalWarriorNum];//这几行代码保证了城市中0号放红方的武士，1号放蓝方的武士
				RedHead.pWarriors[RedHead.nTotalWarriorNum] = NULL;
				cities[N-1].warriorInCity[BLUE] = BlueHead.pWarriors[BlueHead.nTotalWarriorNum];
				BlueHead.pWarriors[BlueHead.nTotalWarriorNum] = NULL;
				for (i = 0; i < N - 1; i++)//红方的在城市中的武士向前走
				{
					cities[i + 1].warriorInCity[RED] = cityForTmp[i].warriorInCity[RED];
					cityForTmp[i].warriorInCity[RED] = NULL;
				}
				for (i = N - 1; 0 < i; i--)//蓝方的在城市中武士向前走
				{
					cities[i - 1].warriorInCity[BLUE] = cityForTmp[i].warriorInCity[BLUE];
					cityForTmp[i].warriorInCity[BLUE] = NULL;
				}
				
				//输出前进中发生的事件
				CWarrior *redWarrToHq = cityForTmp[N-1].warriorInCity[RED];
				if (redWarrToHq != NULL)
				{
					printf("%03d:%02d blue %s %d reached red headquarter with %d elements and %d force",
						nHour, *minutes, redWarrToHq->Names[redWarrToHq->nKindNo],
						redWarrToHq->wLifeValue, redWarrToHq->attackForce);
					RedHead.enemies += 1;
					redWarrToHq->~CWarrior();
					redWarrToHq = NULL;
				}

			}

			//4th
			nMinute++;
			for (i = 0; i < N; i++)
			{
				cities[i].CityLifeValInc();
			}

			//5th 30minute,此时战斗还没有发生，城市生命值不取走是因为该城市只有一个武士
			//而武士战斗循环结束后会有新的循环来让总部完成奖励然后循环来完成战利品（城市生命值和旗帜更换等事件）
			nMinute++;
			{
				for (i = 0; i < N; i++)
				{
					CWarrior *tmp;
					if (cities[i].warriorInCity[RED] != NULL && cities[i].warriorInCity[BLUE] == NULL)
					{
						tmp = cities[i].warriorInCity[RED];
						printf("%03d:%02d red %s earned %d elements for his headquarter",
							nHour, *nMinute, tmp->Names[tmp->nKindNo], cities[i].cityLifeValue); 
						tmp->TakeCityLifeVal(&cities[i]);
					}
					if (cities[i].warriorInCity[BLUE] != NULL && cities[i].warriorInCity[RED] == NULL)
					{
						tmp = cities[i].warriorInCity[BLUE];
						printf("%03d:%02d blue %s earned %d elements for his headquarter",
							nHour, *nMinute, tmp->Names[tmp->nKindNo], cities[i].cityLifeValue);
						tmp->TakeCityLifeVal(&cities[i]);
					}
				}
			}

			//6th 35minute
			nMinute++;
			{
				//实现了相互射箭,此处不进行战斗结果的设置，故而也实现了放箭不算战斗
				CWarrior *red, *blue;
				for (i = 0; i < N - 1; i++)
				{
					red = cities[i].warriorInCity[RED];
					blue = cities[i + 1].warriorInCity[BLUE];
					if (red != NULL && blue != NULL && red->wps[ARROW] != NULL)
					{
						red->wps[ARROW]->UseWeapon(red, blue);
						printf("%03d:%02d red %s %d shot", nHour, *nMinute, red->Names[red->nKindNo], red->nKindNo);
						if (blue->wLifeValue <= 0)
						{
							cities[i + 1].killedByArrow = true;
							printf("and killed blue %s %d", blue->Names[blue->nKindNo], blue->nKindNo);
						}
					}
					if (blue != NULL && red != NULL && blue->wps[ARROW] != NULL)
					{
						for (int j = 0; j < 2; j++)
						{
							if (blue->wps[j] != NULL && blue->wps[j]->nKindNo == ARROW)
								blue->wps[j]->UseWeapon(blue, red);
						}
						printf("%03d:%02d blue %s %d shot", nHour, *nMinute, blue->Names[blue->nKindNo], blue->nKindNo);
						if (red->wLifeValue <= 0)
						{
							cities[i].killedByArrow = true;
							printf("and killed red %s %d", red->Names[red->nKindNo], red->nKindNo);
						}
					}

				}
			}

			//38minute
			{
				CWarrior *red, *blue;
				for (i = 0; i < N; i++)
				{
					red = cities[i].warriorInCity[RED], blue = cities[i].warriorInCity[BLUE];
					if (red != NULL && blue != NULL)
					{
						if (red->wps[BOMB] != NULL)
							red->wps[BOMB]->UseWeapon(red, blue);
						if (blue->wps[BOMB] != NULL)
							blue->wps[BOMB]->UseWeapon(blue, red);
					}
				}
			}

			//40 minute
			{
				CWarrior *red, *blue;
				for (i = 0; i < N; i++)
				{
					red = cities[i].warriorInCity[RED], blue = cities[i].warriorInCity[BLUE];
					if (red != NULL && blue != NULL)
					{
						if (cities[i].flagColor == RED || i % 2 == 1)
						{

						}
						if (cities[i].flagColor == BLUE || i % 2 == 0)
						{

						}
					}




				}


			}



			if (isHeadTaken(nHour, *nMinute, RedHead))//产生13th输出
				break;
			if (isHeadTaken(nHour, *nMinute, BlueHead))
				break;
			nMinute = minutes;
		}
		delete[] cities;
		delete[] cityForTmp;
	}
	return 0;
}


void hqTaken(int h, int m, int color)
{
	if (color == RED)
		printf("%03d:%02d red headquarter was taken", h, m);
	if (color == BLUE)
		printf("%03d:%02d blue headquarter was taken", h, m);

}

bool isHeadTaken(int h, int m, CHeadquarter &hd)
{
	if (hd.enemies == 2)
	{
		hqTaken(h, m, hd.GetColor());
		return true;
	}
	else
		return false;
}

void LionFlee(CCity &city, int h, int m)
{
	for (int i = 0; i < 2; i++)
	{
		CWarrior* tmp = city.warriorInCity[i];
		if (tmp != NULL && tmp->nKindNo == LION && tmp->ReportLionLoyalty() == 0)
			{
				if (tmp->HqColor() == RED)
					printf("%03d:%02d red lion %d ran away", h, m, tmp->nNo);
				else
					printf("%03d:%02d blue lion %d ran away", h, m, tmp->nNo);
			}
	}
}