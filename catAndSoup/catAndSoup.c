#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

#define ROOM_WIDTH          10
#define HME_POS             1
#define BWL_PO              (ROOM_WIDTH - 2)

#define INTIMACY_MIN        0
#define INTIMACY_DEFAULT    2
#define INTIMACY_MAX        4

#define EMOTION_MIN        0
#define EMOTION_DEFAULT    3
#define EMOTION_MAX        3

#define POTATO_SOUP         1
#define MUSHROOM_SOUP       2
#define BROCCOLI_SOUP       3

#define INTERACTION_NOTHING       0
#define INTERACTION_SCRATCH      1

#define DELAY_STEP 500
#define DELAY_INTRO 1000
#define DELAY_TURN 2500

void reset(int milliSec);
void mSleep(int milliSec);
void introAndGetName(char* catName, int catNameLength);
void showStatus(char* catName, int intimacy, int soupCount, int cutePoint, int catEmotion);

// return 0 noting, 1 scratch, 2 mouse toy, 3 pointer toy 
// hasMouseToy, hasPointertoy 0 means NO
int getInteraction(int hasMouseToy, int hasPointerToy);
int rollDice(void); // return 1~6
void showRoom(const int catPos, const int catPreviousPos, const int scratcherPos, const int catTowerPos);
int makeSoup(void); // return 1,2,3 is potato, mushroom, broccoli

int main()
{
    char catName[100] = { 0 };
    int intimacy = INTIMACY_DEFAULT;
    int soupCount = 0;
    int cutePoint = 0;
    int catEmotion = EMOTION_DEFAULT;

    int catPos = HME_POS;
    int catPreviousPos = catPos;
    int scratcherPos = -1;
    int catTowerPos = -1;

    srand((unsigned int)time(NULL));

    introAndGetName(catName, sizeof(catName));
    reset(DELAY_INTRO);

    while (1) {
        // show status
        showStatus(catName, intimacy, soupCount, cutePoint, catEmotion);
        mSleep(DELAY_STEP);

        // cat move 
        printf("%s 이동: 집사와 친밀할수록 냄비 쪽으로 갈 확률이 높아집니다.\n", catName);
        printf("주사위 눈이 %d 이상이면 냄비 쪽으로 이동합니다.\n", 6 - intimacy);

        catPreviousPos = catPos;

        switch (catEmotion)
        {
        case 0:
            printf("기분이 매우 나쁜 %s은(는) 집으로 향합니다.\n", catName);
            if (catPos < BWL_PO)
                catPos++;
            break;
        case 1:
        {
            if (scratcherPos != -1 && catTowerPos - 1) {
                printf("%s은(는) 놀 거리가 없어서 기분이 매우 나빠집니다.\n", catName);
                if (catEmotion > EMOTION_MIN)
                    catEmotion -= 1;
                break;
            } else {
                int distanceToScracher = scratcherPos == -1 ? INT_MAX : scratcherPos - catPos;
                int distanceToCatTower = catTowerPos == -1 ? INT_MAX : catTowerPos - catPos;

                int absDistanceToScracher = abs(distanceToScracher);
                int absDistanceToCatTower = abs(distanceToCatTower);

                if (absDistanceToCatTower <= absDistanceToScracher) {
                    if (distanceToCatTower < 0)
                        catPos--;
                    else if (distanceToCatTower > 0)
                        catPos++;
                }
                else {
                    if (distanceToScracher < 0)
                        catPos--;
                    else if (distanceToScracher > 0)
                        catPos++;
                }
            }
            break;  
        }
        case 2:
            printf("%s은(는) 기분좋게 식빵을 굽고 있습니다.\n", catName);
            break;
        case 3:
            printf("%s은(는) 골골송을 부르며 수프를 만들러 갑니다.\n",catName);
            if (catPos < BWL_PO)
                catPos++;
            break;
        }
        mSleep(DELAY_STEP);

        // do action
        if (catPos == HME_POS && catPreviousPos == HME_POS) {
            printf("%s은(는) 집에서 기분좋게 휴식을 취했습니다.\n", catName);
        }
        else if(catPos == BWL_PO){
            int soup = makeSoup();
            soupCount += 1;
            printf("%s이(가) %s를 만들었습니다!\n", catName,
                soup == POTATO_SOUP ? "감자 수프" :
                soup == MUSHROOM_SOUP ? "양송이 수프" : "브로콜리 수프");
            printf("현재까지 만든 수프: %d개\n", soupCount);
        }
        else if (catPos == scratcherPos) {
            int catPrevEmotion = catEmotion;
            catEmotion += 1;
            if (catEmotion > EMOTION_MAX)
                catEmotion = EMOTION_MAX;
            printf("%s은(는) 스크래처를 긁고 놀았습니다.\n", catName);
            printf("기분이 제법 좋아졌습니다. %d -> %d\n", catPrevEmotion, catEmotion);
        }
        else if (catPos == catTowerPos) {
            int catPrevEmotion = catEmotion;
            catEmotion += 2;
            if (catEmotion > EMOTION_MAX)
                catEmotion = EMOTION_MAX;
            printf("%s은(는) 캣타워를 뛰어다닙니다.\n", catName);
            printf("기분이 제법 좋아졌습니다. %d -> %d\n", catPrevEmotion, catEmotion);
        }
        mSleep(DELAY_STEP);

        // showRoom
        showRoom(catPos, catPreviousPos, scratcherPos, catTowerPos);
        mSleep(DELAY_STEP);

        // doInteraction
        int interaction = getInteraction(scratcherPos, catTowerPos);
        if (interaction == INTERACTION_NOTHING) {
            printf("아무것도 하지 않습니다.\n");
            printf("4 / 6의 확률로 친밀도가 떨어집니다.\n");
            int rollValue = rollDice();
            if (rollValue <= 4) {
                if (intimacy > INTIMACY_MIN)
                    intimacy--;
                printf("친밀도가 떨어집니다.\n");
                printf("현재 친밀도 : %d\n", intimacy);
            }
            else {
                printf("다행히 친밀도가 떨어지지 않았습니다.\n");
                printf("현재 친밀도 : %d\n", intimacy);
            }
        }
        else if (interaction == INTERACTION_SCRATCH) {
            printf("쫀떡의 턱을 긁어주었습니다.\n");
            printf("2 / 6의 확률로 친밀도가 높아집니다.\n");
            int rollValue = rollDice();
            if (rollValue > 4) {
                if (intimacy < INTIMACY_MAX)
                    intimacy++;
                printf("친밀도가 높아집니다.\n");
                printf("현재 친밀도 : %d\n", intimacy);
            }
            else {
                printf("친밀도는 그대로입니다.\n");
                printf("현재 친밀도 : %d\n", intimacy);
            }
        }
        reset(DELAY_TURN);
    }
}

void reset(int milliSec) {
    mSleep(milliSec);
    system("cls");
}
void mSleep(int milliSec) {
    Sleep(milliSec);
}
void introAndGetName(char* catName, int catNameLength) {
    printf("****야옹이와 수프****\n\n");
    printf("      /\\_/\\ \n");
    printf(" /\\  / o o \\ \n");
    printf("//\\\\ \\~(*)~/\n");
    printf("`  \\/   ^ /\n");
    printf("   | \\|| ||\n");
    printf("   \\ '|| ||\n");
    printf("    \\)()-())\n\n");
    printf("야옹이의 이름을지어주세요: ");
    scanf_s("%s", catName, catNameLength);
    printf("야옹이의 이름은 %s입니다.\n\n", catName);
}

void showStatus(char* catName, int intimacy, int soupCount, int cutePoint, int catEmotion)
{
    printf("==================== 현재 상태 ===================\n");
    printf("현재까지 만든 수프 : %d개\n", soupCount);
    printf("CP: %d 포인트\n", cutePoint);
    printf("%s 기분(0~3): %d\n", catName, catEmotion);
    switch (catEmotion)
    {
    case 0:
        printf(" 기분이 매우 나쁩니다.\n");
        break;
    case 1:
        printf(" 심심해합니다.\n");
        break;
    case 2:
        printf(" 식빵을 굽습니다.\n");
        break;
    case 3:
        printf(" 골골송을 부릅니다.\n");
        break;
    }
    printf("집사와의 관계(0~4) : %d\n", intimacy);
    if (intimacy < INTIMACY_MIN || intimacy > INTIMACY_MAX) {
        printf("ERROR: 허용된 STATUS 값을 벗어났습니다. STATUS:%d\n", intimacy);
        exit(-1);
    }
    switch (intimacy)
    {
    case 0:
        printf(" 곁에 오는것조차 싫어합니다.\n");
        break;
    case 1:
        printf(" 간식 자판기 취급입니다.\n");
        break;
    case 2:
        printf(" 그럭저럭 쓸 만한 집사입니다.\n");
        break;
    case 3:
        printf(" 훌륭한 집사로 인정 받고 있습니다.\n");
        break;
    case 4:
        printf(" 집사 껌딱지입니다.\n");
        break;
    }
    printf("==================================================\n\n");
}

int getInteraction(int hasMouseToy, int hasPointerToy)
{
    int userInteraction = -1;
    printf("어떤 상호작용을 하시겠습니까? 0. 아무것도 하지 않음 1. 긁어 주기\n");

    do {
        printf(">> ");
        scanf_s("%d", &userInteraction);
    } while (userInteraction != 0 && userInteraction != 1);

    return userInteraction;
}

int rollDice(void)
{
    int random = rand() % 6 + 1;

    printf("주사위를 굴립니다.또르륵...\n");
    printf("%d이(가) 나왔습니다!\n", random);

    return random;
}

void showRoom(const int catPos, const int catPreviousPos, const int scratcherPos, const int catTowerPos) {
    printf("\n");
    for (int i = 0; i < ROOM_WIDTH; i++)
        printf("#");
    printf("\n");

    // home and bowl and scratcher and catTower
    for (int i = 0; i < ROOM_WIDTH; i++) {
        if (i == HME_POS)
            printf("H");
        else if (i == BWL_PO)
            printf("B");
        else if (i == scratcherPos)
            printf("S");
        else if (i == catTowerPos)
            printf("T");
        else if (i == 0 || i == ROOM_WIDTH - 1)
            printf("#");
        else
            printf(" ");
    }
    printf("\n");

    // cat
    for (int i = 0; i < ROOM_WIDTH; i++) {
        if (catPos == i)
            printf("C");
        else if (catPreviousPos == i)
            printf(".");
        else if (i == 0 || i == ROOM_WIDTH - 1)
            printf("#");
        else
            printf(" ");
    }
    printf("\n");

    for (int i = 0; i < ROOM_WIDTH; i++)
        printf("#");
    printf("\n\n");
}

int makeSoup(void)
{
    return rand() % 3 + 1;
}