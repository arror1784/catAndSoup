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

#define INTERACTION_NOTHING          0
#define INTERACTION_SCRATCH          1
#define INTERACTION_MOUSE_TOY        2
#define INTERACTION_POINTER_TOY      3

#define DELAY_STEP 500
#define DELAY_INTRO 1000
#define DELAY_TURN 2500

typedef struct {
    int catTower;
    int scratcher;
    int mouseToy;
    int pointerToy;
}Items;

typedef struct {
    int catPos;
    int catPreviousPos;
}CatPositions;

typedef struct {
    int intimacy;
    int soupCount;
    int cutePoint;
    int catEmotion;
}GameState;


void reset(int milliSec);
void mSleep(int milliSec);
void introAndGetName(char* catName, int catNameLength);
void showStatus(char* catName, GameState gameState);

// return 0 noting, 1 scratch, 2 mouse toy, 3 pointer toy 
// hasMouseToy, hasPointertoy 0 means NO
int getInteraction(Items items);
int rollDice(void); // return 1~6
void showRoom(CatPositions catPositions, Items items);
int makeSoup(void); // return 1,2,3 is potato, mushroom, broccoli

int main()
{
    char catName[100] = { 0 };
    GameState gameState = { 0 };
    gameState.intimacy = INTIMACY_DEFAULT;
    gameState.soupCount = 0;
    gameState.cutePoint = 0;
    gameState.catEmotion = EMOTION_DEFAULT;
    
    CatPositions catPositions = { 0 };
    catPositions.catPos = HME_POS;
    catPositions.catPreviousPos = HME_POS;

    Items items = { 0 };

    srand((unsigned int)time(NULL));

    introAndGetName(catName, sizeof(catName));
    reset(DELAY_INTRO);

    while (1) {
        catPositions.catPreviousPos = catPositions.catPos;

        // show status
        showStatus(catName, gameState);
        mSleep(DELAY_STEP);

        // feel bad
        printf("주사위 눈이 %d이하면 그냥 기분이 나빠집니다. 고양이니까?\n", 6 - gameState.intimacy);
        printf("주사위를 굴립니다. 또르르...\n");
 
        int rollValue = rollDice();
        printf("%d이(가) 나왔습니다.\n",rollValue);
        if (rollValue <= (6 - gameState.intimacy)) {
            int catPrevEmotion = gameState.catEmotion;
            if (gameState.catEmotion > EMOTION_MIN)
                gameState.catEmotion -= 1;
            printf("%s의 기분이 나빠집니다: %d -> %d\n", catName, catPrevEmotion, gameState.catEmotion);
        }
        else {
            printf("다행히 %s의 기분이 나빠지지 않았습니다.\n",catName);
        }
        printf("\n");
        

        // cat move
        printf("%s 이동: 집사와 친밀할수록 냄비 쪽으로 갈 확률이 높아집니다.\n", catName);
        printf("주사위 눈이 %d 이상이면 냄비 쪽으로 이동합니다.\n", 6 - gameState.intimacy);

        switch (gameState.catEmotion)
        {
        case 0:
            printf("기분이 매우 나쁜 %s은(는) 집으로 향합니다.\n", catName);
            if (catPositions.catPos < BWL_PO)
                catPositions.catPos++;
            break;
        case 1:
        {
            if (!items.scratcher && !items.catTower) {
                printf("%s은(는) 놀 거리가 없어서 기분이 매우 나빠집니다.\n", catName);
                if (gameState.catEmotion > EMOTION_MIN)
                    gameState.catEmotion -= 1;
                break;
            } else {
                int distanceToScracher = items.scratcher == -1 ? INT_MAX : items.scratcher - catPositions.catPos;
                int distanceToCatTower = items.catTower == -1 ? INT_MAX : items.catTower - catPositions.catPos;

                int absDistanceToScracher = abs(distanceToScracher);
                int absDistanceToCatTower = abs(distanceToCatTower);

                if (absDistanceToCatTower <= absDistanceToScracher) {
                    if (distanceToCatTower < 0)
                        catPositions.catPos--;
                    else if (distanceToCatTower > 0)
                        catPositions.catPos++;
                }
                else {
                    if (distanceToScracher < 0)
                        catPositions.catPos--;
                    else if (distanceToScracher > 0)
                        catPositions.catPos++;
                }
            }
            break;  
        }
        case 2:
            printf("%s은(는) 기분좋게 식빵을 굽고 있습니다.\n", catName);
            break;
        case 3:
            printf("%s은(는) 골골송을 부르며 수프를 만들러 갑니다.\n",catName);
            if (catPositions.catPos < BWL_PO)
                catPositions.catPos++;
            break;
        }
        mSleep(DELAY_STEP);


        // do action
        if (catPositions.catPos == HME_POS && catPositions.catPreviousPos == HME_POS) {
            printf("%s은(는) 집에서 기분좋게 휴식을 취했습니다.\n", catName);
        }
        else if(catPositions.catPos == BWL_PO){
            int soup = makeSoup();
            gameState.soupCount += 1;
            printf("%s이(가) %s를 만들었습니다!\n", catName,
                soup == POTATO_SOUP ? "감자 수프" :
                soup == MUSHROOM_SOUP ? "양송이 수프" : "브로콜리 수프");
            printf("현재까지 만든 수프: %d개\n", gameState.soupCount);
        }
        else if (catPositions.catPos == items.scratcher) {
            int catPrevEmotion = gameState.catEmotion;
            gameState.catEmotion += 1;
            if (gameState.catEmotion > EMOTION_MAX)
                gameState.catEmotion = EMOTION_MAX;
            printf("%s은(는) 스크래처를 긁고 놀았습니다.\n", catName);
            printf("기분이 제법 좋아졌습니다. %d -> %d\n", catPrevEmotion, gameState.catEmotion);
        }
        else if (catPositions.catPos == items.catTower) {
            int catPrevEmotion = gameState.catEmotion;
            gameState.catEmotion += 2;
            if (gameState.catEmotion > EMOTION_MAX)
                gameState.catEmotion = EMOTION_MAX;
            printf("%s은(는) 캣타워를 뛰어다닙니다.\n", catName);
            printf("기분이 제법 좋아졌습니다. %d -> %d\n", catPrevEmotion, gameState.catEmotion);
        }
        mSleep(DELAY_STEP);

        // showRoom
        showRoom(catPositions, items);
        mSleep(DELAY_STEP);

        // doInteraction
        int interaction = getInteraction(items);
        if (interaction == INTERACTION_NOTHING) {
            int catPrevEmotion = gameState.catEmotion;
            gameState.catEmotion -= 1;
            if (gameState.catEmotion < EMOTION_MIN)
                gameState.catEmotion = EMOTION_MIN;
            printf("아무것도 하지 않습니다.\n");
            printf("%s의 기분이 나빠졌습니다: %d -> %d\n", catName, catPrevEmotion, gameState.catEmotion);
            printf("5 / 6의 확률로 친밀도가 떨어집니다.\n");
            int rollValue = rollDice();
            if (rollValue <= 5) {
                if (gameState.intimacy > INTIMACY_MIN)
                    gameState.intimacy -= 1;
                printf("집사와의 관계가 나빠집니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
            else {
                printf("다행히 친밀도가 떨어지지 않았습니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
        }
        else if (interaction == INTERACTION_SCRATCH) {
            printf("%s의 턱을 긁어주었습니다.\n",catName);
            printf("%s의 기분이 그대로입니다: %d\n", catName, gameState.catEmotion);
            printf("2 / 6의 확률로 친밀도가 높아집니다.\n");
            int rollValue = rollDice();
            if (rollValue > 4) {
                if (gameState.intimacy < INTIMACY_MAX)
                    gameState.intimacy++;
                printf("집사와의 관계가 좋아집니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
            else {
                printf("친밀도는 그대로입니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
        }
        else if (interaction == INTERACTION_MOUSE_TOY) {
            int catPrevEmotion = gameState.catEmotion;
            gameState.catEmotion += 1;
            if (gameState.catEmotion > EMOTION_MAX)
                gameState.catEmotion = EMOTION_MAX;
            printf("장남감 쥐로 %s와 놀아 주었습니다.\n", catName);
            printf("%s의 기분이 조금 좋아졌습니다: %d -> %d\n", catName, catPrevEmotion, gameState.catEmotion);
            printf("2 / 6의 확률로 친밀도가 높아집니다.\n");
            int rollValue = rollDice();
            if (rollValue >= 4) {
                if (gameState.intimacy < INTIMACY_MAX)
                    gameState.intimacy++;
                printf("집사와의 관계가 좋아집니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
            else {
                printf("친밀도는 그대로입니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
        }
        else if (interaction == INTERACTION_POINTER_TOY) {
            int catPrevEmotion = gameState.catEmotion;
            gameState.catEmotion += 2;
            if (gameState.catEmotion > EMOTION_MAX)
                gameState.catEmotion = EMOTION_MAX;
            printf("레이저 포인터로 %s와 신나게 놀아 주었습니다.\n", catName);
            printf("%s의 기분이 꽤 좋아졌습니다: %d -> %d\n", catName, catPrevEmotion, gameState.catEmotion);
            printf("2 / 6의 확률로 친밀도가 높아집니다.\n");
            int rollValue = rollDice();
            if (rollValue >= 2) {
                if (gameState.intimacy < INTIMACY_MAX)
                    gameState.intimacy++;
                printf("집사와의 관계가 좋아집니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
            else {
                printf("친밀도는 그대로입니다.\n");
                printf("현재 친밀도 : %d\n", gameState.intimacy);
            }
        }

        // CP create
        int createdCP = max(0, gameState.catEmotion - 1) + gameState.intimacy;
        printf("%s의 기분(%d~%d): %d\n", catName, EMOTION_MIN, EMOTION_MAX, gameState.catEmotion);
        printf("집사와의 관계(%d~%d): %d\n", INTIMACY_MIN, INTIMACY_MAX, gameState.intimacy);
        printf("%s의 기분과 친밀도에 따라서 CP가 %d 포인트 생사되었습니다.\n", catName, createdCP);
        printf("보유 CP: %d 포인트\n", createdCP);

        // shop



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

void showStatus(char* catName, GameState gameState)
{
    printf("==================== 현재 상태 ===================\n");
    printf("현재까지 만든 수프 : %d개\n", gameState.soupCount);
    printf("CP: %d 포인트\n", gameState.cutePoint);
    printf("%s 기분(0~3): %d\n", catName, gameState.catEmotion);
    switch (gameState.catEmotion)
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
    printf("집사와의 관계(0~4) : %d\n", gameState.intimacy);
    if (gameState.intimacy < INTIMACY_MIN || gameState.intimacy > INTIMACY_MAX) {
        printf("ERROR: 허용된 STATUS 값을 벗어났습니다. STATUS:%d\n", gameState.intimacy);
        exit(-1);
    }
    switch (gameState.intimacy)
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

int getInteraction(Items items)
{
    int userInteraction = -1;
    int interactionCount = -1;

    printf("어떤 상호작용을 하시겠습니까?\n");

    interactionCount += 1;
    printf("%d. 아무것도 하지 않음\n", interactionCount);
    interactionCount += 1;
    printf("%d. 긁어 주기\n", interactionCount);
    if (items.mouseToy) {
        interactionCount += 1;
        printf("%d. 장난감 쥐로 놀아주기\n", interactionCount);
    }
    if (items.pointerToy) {
        interactionCount += 1;
        printf("%d. 레이저 포인터로 놀아주기\n", interactionCount);
    }

    do {
        printf(">> ");
        scanf_s("%d", &userInteraction);
    } while (!(userInteraction >= 0 && userInteraction <= interactionCount));

    // if userInteraction is 2 and no mouse Toy, it means 2 is pointer toy
    if (userInteraction == 2 && !items.mouseToy)
        return INTERACTION_POINTER_TOY;

    return userInteraction;
}

int rollDice(void)
{
    int random = rand() % 6 + 1;

    printf("주사위를 굴립니다.또르륵...\n");
    printf("%d이(가) 나왔습니다!\n", random);

    return random;
}

void showRoom(CatPositions catPositions, Items items) {
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
        else if (i == items.scratcher)
            printf("S");
        else if (i == items.catTower)
            printf("T");
        else if (i == 0 || i == ROOM_WIDTH - 1)
            printf("#");
        else
            printf(" ");
    }
    printf("\n");

    // cat
    for (int i = 0; i < ROOM_WIDTH; i++) {
        if (catPositions.catPos == i)
            printf("C");
        else if (catPositions.catPreviousPos == i)
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