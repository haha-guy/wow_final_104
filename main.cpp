#include <iostream>
#include "CWarrior.h"
using namespace std;

int minutes[10] = {0, 5, 10, 20, 30, 35, 38, 40, 50, 55};
bool isHeadTaken(int h, int m, CHeadquarter &hd);
void hqTaken(int h, int m, int color);
void LionFlee(CCity &city, int h, int m);

//�������㣺��red����ʿ���ڳ����е�[RED],������������ʿ����Ӧ��λ�ã��˴����������õ����ݱ�ʾ
//�������Ĵ�����ڻ��ຯ���У����������ض���ĸ��Ǻ����е��û������غ���
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
			//�˴�������1st���
			RedHead.Produce(nHour);
			BlueHead.Produce(nHour);

			//2sec,�˴�����Ҫ�����ܲ����Ŀ������ܵ�Lion,Ŀǰ��δ����
			nMinute++;
			for (i = 0; i < N; i++)
			{
				LionFlee(cities[i], nHour, *nMinute);
			}

			//3rd, ��д�ϣ�Ȼ���ٿ��Ƿ�װ�ɺ���,�ⲿ����Ҫ����˼��
			//�ܲ�����ǰ�ߵ���ʿΪpWarriors[nTatalWarriorNum],������ǰ�ߺ�
			//��Ҫ����λ����ΪNULL
			//��ʿ��ǰ�ߺ󣬻���Ҫ�Գ����е�killedByArrow��Ϊfalse
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
				//�����ܲ�����ʿ,�����ʿ�����ܲ��󼴽��ܲ���enemy��һ��Ȼ����ʿ����
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


				//�ܲ�����ʿ, �˴�д�ɺ�������ͬʱ������ʿǰ���� �����¼����
				cities[0].warriorInCity[RED] = RedHead.pWarriors[RedHead.nTotalWarriorNum];//�⼸�д��뱣֤�˳�����0�ŷź췽����ʿ��1�ŷ���������ʿ
				RedHead.pWarriors[RedHead.nTotalWarriorNum] = NULL;
				cities[N-1].warriorInCity[BLUE] = BlueHead.pWarriors[BlueHead.nTotalWarriorNum];
				BlueHead.pWarriors[BlueHead.nTotalWarriorNum] = NULL;
				for (i = 0; i < N - 1; i++)//�췽���ڳ����е���ʿ��ǰ��
				{
					cities[i + 1].warriorInCity[RED] = cityForTmp[i].warriorInCity[RED];
					cityForTmp[i].warriorInCity[RED] = NULL;
				}
				for (i = N - 1; 0 < i; i--)//�������ڳ�������ʿ��ǰ��
				{
					cities[i - 1].warriorInCity[BLUE] = cityForTmp[i].warriorInCity[BLUE];
					cityForTmp[i].warriorInCity[BLUE] = NULL;
				}
				
				//���ǰ���з������¼�
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

			//5th 30minute,��ʱս����û�з�������������ֵ��ȡ������Ϊ�ó���ֻ��һ����ʿ
			//����ʿս��ѭ������������µ�ѭ�������ܲ���ɽ���Ȼ��ѭ�������ս��Ʒ����������ֵ�����ĸ������¼���
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
				//ʵ�����໥���,�˴�������ս����������ã��ʶ�Ҳʵ���˷ż�����ս��
				//ͬʱ����Ҫ����������ʿͬʱ���Է���������ʱ��Ҫ������������
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

			//40 minute���˴�ս��ֻ����ʹ��sword
			//��������������ʿʬ�彫���ڳ����У�������Ҫ��ǰ�������н�����Ӧ�ļ�⣩
			//������һ����ս��������������Ҳ��Ҫ�жϵз���ʿ�����ǻ�,��ص�ս�������ս�������н���
			//�����ĺô���ս��ʱ����ͳһ����,������Ҫע�⵱������ʿͬʱ���Է�����
			//�������������ѭ���д���
			{
				CWarrior *red, *blue;
				for (i = 0; i < N; i++)
				{
					red = cities[i].warriorInCity[RED], blue = cities[i].warriorInCity[BLUE];
					//����ʿǰ������5����ǰ�ķɼ�ɱ������ʿ���ڳ��У�Ҳ��һ��ս�������е��¼�Ҳһ������
					if (cities[i].killedByArrow)
					{
						//���ż�����ʿǰ���������÷ɼ�ɱ������ʿ�ĳ���ʱ����
						//��ȥ����ʿ��������
						if (red != NULL)//˵��blue��redɱ��
						{
							//
						}
						if (blue != NULL)//˵��red��blue�ķɼ�ɱ��
						{

						}

					}
					else
					{
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


			}



			if (isHeadTaken(nHour, *nMinute, RedHead))//����13th���
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