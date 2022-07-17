// The parent-child relationship 
// 水平移動紅色方塊繞著綠色方塊進行逆時針方向旋轉
// 上下移動藍色方塊繞著紅色方塊進行逆時針方向旋轉

#include "header/Angel.h"
#include "Common/CQuad.h"
#include "Common/CBulletData.h"
#include "Common/CQuadEX1.h"
#include "Common/CQuadEX2B.h"
#include "Common/CBG.h"
#include "Common/EnemyB.h"
#include "Common/Boss.h"
#include "Common/CMfollow.h"
#include "Common/Cfire.h"

#define SPACE_KEY 32
#define SCREEN_SIZE 1000
#define HALF_SIZE (SCREEN_SIZE/2) 
#define VP_HALFWIDTH  15.0f
#define GRID_SIZE 4 
#define GDISTANCE 3
#define BDISTANCE 3
#define BG_SIZE 9  //number of bg line
#define BG_SPEED 10
#define MAX_BULLET 2
#define ENEMYA_NUM 7
#define ENEMYB_NUM 2
#define BOSS_OUT_TIME 750 //use f_clock to count
#define LIST_MAX_BULLET 8
#define HP 3 //health hp

//Color vec4
vec4 orichidColor = vec4(0.8549, 0.4392, 0.8392, 1);
vec4 lavanderColor = vec4(0.9019, 0.9019, 1);
vec4 RedColor = vec4(0.4, 0, 0, 1);
vec4 Black = vec4(0, 0, 0, 1);

// For grid quad--player
CQuad *g_pRQuad;				// 位於畫面正中間
CQuad *g_pRQuadA;
CQuad *g_pRQuadB;

CQuad *g_pGQuad[GRID_SIZE];	// 為 Red Quad 的 child
CQuad *g_pBQuad[GRID_SIZE];	// x為 Green Quad 的 child
CMfollow *pBulletQuad[MAX_BULLET];

CBulletData* BulletP, * pget, * phead, * ptail, * pBulletDraw,*pBulletMove,*pBulletCollide;//for link list bullet

CQuadEX1* gEnemyA[ENEMYA_NUM];//enemyA
EnemyB * gEnemyB;//enemyB
Boss* gBoss;//boss
CBulletData* p_enemyBul, * epget, * ephead, * eptail, * eBulletDraw, * eBulletMove, * eBulletCollide;//link list enemy bullet

CBG* pBg[BG_SIZE];//bg
CQuadEX2B* heartHp[HP];
Cfire* Pfire;

int I_fpscount = 1;
float fFps_count = 1;//onframe count
float f_Clock = 0;


float g_fRQuadT[3];
float g_fRQuadAT[3];
float g_fRQuadBT[3];

bool playerHit = false;//whether if player is hit

float g_fGQuadT[GRID_SIZE][3]={0};	// 紅色方塊的位移
float g_fBQuadT[GRID_SIZE][3]={0};	// 藍色方塊的位移，與紅色方塊的父子關係為一對一

float fBulletQuadT[MAX_BULLET][3] = {0};//player front bullet

//enemyA move
float AMoveY = 12;
float AMoveX = 0;
float EnACount = 0;//enemyA alive num

//enemyB move
float BMoveX = 0;
float BMoveY = 0;

float fBg[BG_SIZE][3] = { 0 };

//health
float fHp[HP][3] = { 0 };

int playerState = 0;

//for bg mat
mat4 mxBG[BG_SIZE];
mat4 BgT[BG_SIZE];

//for player
mat4 mxRT, mxGT[GRID_SIZE], mxBT[GRID_SIZE];
mat4 mxRR, mxGR[GRID_SIZE], mxBR[GRID_SIZE];
mat4 mxRAT, mxRBT;
mat4 mxBullT[MAX_BULLET];
mat4 mxGRSmall= Scale(0.5, 0.25, 0);
mat4 turnT = RotateZ(90);
mat4 sfireT = Scale(0.2, 0.2, 0);

// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxIdentity(1.0f);
mat4 g_mxProjection;

GLfloat g_fGAngle = 0; // Green 的旋轉角度

//for auto rotate
GLfloat g_fZAngle = 0;
GLfloat g_fDir = 1;   // 旋轉方向
GLfloat g_fDirAnother = -1;

bool    m_bAutoRotation = true; // Controlled by e Key

GLfloat g_fWingAngle = 0;//wings rotate
GLfloat g_fWingAngleB = 0;


//mouse move x,y
GLfloat g_fTx = 0, g_fTy = 0;

//mouse move T
mat4 mxMoveT;
//origin place T use for bullet
mat4 mxOriginT;

//is wing open T
mat4 mxIsOpenA;
mat4 mxIsOpenB;
mat4 mxIsCloseA;
mat4 mxIsCloseB;
//fire trg
bool IsFire = false;
//bool for shoot -- old bullet
bool CanShoot = true;
//boss time boss out
bool BossTime = false;
//player bullet bool
bool playerbul = false;
//enemy bullet bool
bool enemybul = false;
bool enemybulUlt = false;


//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();
void CreateQuadRelationship();
void CreateBackGround();
void AutomaticRotation(float);
void AutomaticAddYAxis(float);
void AutomaticEnemy(float);
void AutomaticBullet(float);
void AutomaticPlayerHurt(float);
void CollideEnemyWithBullet(float);
void CreateBullet();
void CreateEnemy();
void CreateUI();

void init( void )
{
	//  產生 projection 矩陣，此處為產生正投影矩陣
    g_mxProjection = Ortho(-VP_HALFWIDTH, VP_HALFWIDTH, -VP_HALFWIDTH, VP_HALFWIDTH, -1.0f, 1.0f);
	// 必須在 glewInit(); 執行完後,在執行物件實體的取得

	CreateQuadRelationship();
	
	CreateBackGround();
	 
	CreateBullet();

	CreateEnemy();

	CreateUI();

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); //  background color
}

//------------------------------------------------------------------------------------------

void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT ); // clear the window
	for (int i = 0; i < BG_SIZE; i++)
	{
		pBg[i]->draw();

	}//draw bg

	//hp ui
	for (int i = 0; i < HP; i++) heartHp[i]->draw();


	pBulletQuad[0]->draw();//blue bullet

	//player
	g_pRQuad->draw();
	g_pRQuadA->draw();
	g_pRQuadB->draw();
	if (IsFire) {
		Pfire->draw();
	}
	
	for( int i = 0 ; i < GRID_SIZE ; i++ ) g_pGQuad[i]->draw();


	//player bullet draw and link set
	pBulletDraw = phead;
	for (int i = 0; i < LIST_MAX_BULLET; i++) {
		if (pBulletDraw->playerShoot==true) {
			pBulletDraw->draw();

		}
		pBulletDraw = pBulletDraw->Link;
	}

	//draw enemy bullet
	eBulletDraw = ephead;
	for (int i = 0; i < 10; i++) {
		if (eBulletDraw->playerShoot == true) {
			eBulletDraw->draw();

		}
		eBulletDraw = eBulletDraw->Link;
	}
	
	//draw enemy
	for (size_t i = 0; i < ENEMYA_NUM; i++)
	{
		gEnemyA[i]->draw();
	}
	gEnemyB->draw();
	if (BossTime) 
	{
		gBoss->draw();
	}
	

	glutSwapBuffers();	// 交換 Frame Buffer
}

//time use function below-----------------------------------------------------------------
void AutomaticCountHealth(float dt) //hp count
{
	if (g_pRQuad->blood == 2) {
		heartHp[2]->setColor(Black);

	}
	else if(g_pRQuad->blood == 1)
	{
		heartHp[2]->setColor(Black);
		heartHp[1]->setColor(Black);
	}
	else if(g_pRQuad->blood==0){
		heartHp[2]->setColor(Black);
		heartHp[1]->setColor(Black);
		heartHp[0]->setColor(Black);
	}
}
void AutomaticWingOpen(float dt)
{
	mat4 mxWingT,mxFix;
	mat4 mxWingBT, mxFixB;
	int openAngle = 35;
	if (g_fWingAngle > -openAngle) {
		g_fWingAngle -= g_fDir * 1.0f;
	}
	mxWingT = RotateZ(g_fWingAngle);
	mxFix = Translate(0.25, 0.25, 0);
	g_pRQuadA->setTRSMatrix(mxMoveT * mxWingT *mxFix* mxRT * mxRAT);


	if (g_fWingAngleB < openAngle) {
		g_fWingAngleB += g_fDir * 1.0f;
	}
	mxWingBT = RotateZ(g_fWingAngleB);
	mxFixB = Translate(-0.25, 0.25, 0);
	g_pRQuadB->setTRSMatrix(mxMoveT * mxWingBT * mxFixB * mxRT * mxRBT);

	mxIsOpenA = mxWingT * mxFix * mxRT * mxRAT;
	mxIsOpenB = mxWingBT * mxFixB * mxRT * mxRBT;
	
}
void AutomaticWingClose(float dt)
{
	mat4 mxWingT, mxFix;
	mat4 mxWingBT, mxFixB;
	int closeAngle = 0;
	if (g_fWingAngle < closeAngle) {//g_fWingAngle=35
		g_fWingAngle -= g_fDirAnother * 1.0f;
	}
	mxWingT = RotateZ(g_fWingAngle);
	mxFix = Translate(0.25, 0.25, 0);
	g_pRQuadA->setTRSMatrix(mxMoveT * mxWingT * mxFix * mxRT * mxRAT);


	if (g_fWingAngleB > closeAngle) {
		g_fWingAngleB += g_fDirAnother * 1.0f;
	}
	mxWingBT = RotateZ(g_fWingAngleB);
	mxFixB = Translate(-0.25, 0.25, 0);
	g_pRQuadB->setTRSMatrix(mxMoveT * mxWingBT * mxFixB * mxRT * mxRBT);

	mxIsCloseA = mxWingT * mxFix * mxRT * mxRAT;
	mxIsCloseB = mxWingBT * mxFixB * mxRT * mxRBT;

}

void AutomaticRotation(float dt)
{
	mat4 mxGR;
	g_fGAngle += g_fDir * 1.25f;
	mxGR = RotateZ(g_fGAngle);
	
	//g_pRQuad->setTRSMatrix(mxRT * mxMoveT);
	for (int i = 0; i < 4; i++) {
		if (i == 2 || i == 3) {
			g_pGQuad[i]->setTRSMatrix(mxMoveT * mxRT * mxGR * mxGT[i] *turnT* mxGRSmall );
		}
		else {
			g_pGQuad[i]->setTRSMatrix(mxMoveT * mxRT * mxGR * mxGT[i] * mxGRSmall );
		}
		
	}
	
	//bg drop
	for (int i = 0; i < BG_SIZE; i++)
	{
		mat4 movingT;
		int speed=BG_SPEED ;
		if (BossTime) 
		{
			speed=speed/5;
		}
		else
		{
			speed = BG_SPEED;
		}
			
		pBg[i]->bgY -= (15-i) * speed * dt;
		if (pBg[i]->bgY < -17)
		{
			pBg[i]->bgY = 15;
		}

		movingT = Translate(0, pBg[i]->bgY, 0);
		pBg[i]->setTRSMatrix(movingT* BgT[i]);
		
		
	}
	
	
}

void AutomaticInstantiateEnemy(float dt) //enemyB A instantiate(revive)
{
	if (I_fpscount % 100 == 0 && gEnemyB->blood == 0)
	{
		gEnemyB->Yvalue = rand() % 14;
		gEnemyB->blood++;
	}
	for (int i = 0; i < ENEMYA_NUM; i++)
	{
		if (I_fpscount % 50 == 0 && gEnemyA[i]->blood == 0)
		{
			gEnemyA[i]->fEnemyY = /*rand() %*/ 14;
			gEnemyA[i]->fEnemyX = rand() % 12;
			gEnemyA[i]->blood++;
		}
	}
	
	
}
void AutomaticEnemy(float dt)   //enemy status -- move, killed
{
	
	mat4 mxEnemyT;
	//horizontal move
	mat4 mxEnemyXMoveT;

	//enemyA drop move
	for (int i = 0; i < ENEMYA_NUM; i++)
	{
		gEnemyA[i]->fEnemyX += i*dt;
		if (gEnemyA[i]->blood == 0) {
			gEnemyA[i]->Disable(dt);
		}
		else {
			gEnemyA[i]->Move(dt * i);
		}
		
	}

	//B enemy
	if (gEnemyB->blood == 0) {
		gEnemyB->Disable(dt);
	}
	else {
		gEnemyB->Move(dt, dt);
	}
	
	//boss
	if (gBoss->blood == 0) {
		gBoss->Disable(dt);
	}
	else if(gBoss->blood==1){
		gBoss->UltMove(dt);
	}
	else {
		gBoss->Move(dt, dt);
	}

	if (gBoss->blood<3) {
		gBoss->StateChange(dt);
	}
	
	//if boss out, kill all enemy
	if (BossTime == true) {
		for (int i = 0; i < ENEMYA_NUM; i++)
		{
			gEnemyA[i]->Disable(dt);
		}
		gEnemyB->Disable(dt);
	}


}

void AutomaticAddYAxis(float dt)
{//missile
	//Print(gBoss->fEnemyX);
	
	//bullet-follow move
	mat4 mxAddYT,bosT,mathT;
	
	float Folx = 0, Foly = 0;

	Folx = gBoss->fEnemyX - fBulletQuadT[0][0];
	Foly = gBoss->fEnemyY - fBulletQuadT[0][1];
	fBulletQuadT[0][0] += Folx*dt*3;
	fBulletQuadT[0][1] += Foly*dt+1;
	bosT = Translate(fBulletQuadT[0][0], fBulletQuadT[0][1], 0);
	pBulletQuad[0]->setTRSMatrix(bosT);
	
	//Print(g_fTx);
}

void AutomaticBullet(float dt) 
{
	//player bullet trigger setting
	if (playerbul)//switch to next bullet
	{           
		pget->playerShoot = true;
		pget->bulX = g_fTx;
		pget->bulY = g_fTy;
		pget->setTRSMatrix(Translate(pget->bulX, pget->bulY, 0));
		
		pget = pget->Link;
		ptail->Link = phead;
		ptail = phead;
		phead = pget;
		playerbul = false;
	}

	pBulletMove = phead;//set the value to *pbulletmove
	for (int i = 0; i < LIST_MAX_BULLET; i++) {
		if (pBulletMove->playerShoot == true) {
			pBulletMove->ShootLine(dt);
		}
		pBulletMove = pBulletMove->Link;
		//i++;
	}

	//enemy bullet trigger setting
	if (I_fpscount % 20 == 0 && BossTime == true && gBoss->blood > 1) {
		enemybul = true;
	}
	if (enemybul)//switch to next bullet
	{
		epget->playerShoot = true;
		epget->bulX = gBoss->fEnemyX;
		epget->bulY = gBoss->fEnemyY;
		epget->setTRSMatrix(Translate(epget->bulX, epget->bulY, 0));

		epget = epget->Link;
		eptail->Link = ephead;
		eptail = ephead;
		ephead = epget;
		enemybul = false;
	}

	if (gBoss->blood > 1) {

		eBulletMove = ephead;//set the value to *pbulletmove(enemy bullet move line)
		for (int i = 0; i < 10; i++) {
			if (eBulletMove->playerShoot == true) {
				eBulletMove->BossShootLine(dt);
			}
			eBulletMove = eBulletMove->Link;
			//i++;
		}
	}
	
	//enemy Ultimate trigger setting
	
	if (I_fpscount % 10 == 0 && BossTime == true && gBoss->blood == 1) {
		enemybulUlt = true;
		eBulletCollide->playerShoot = true;
	}
	if (enemybulUlt)//switch to next bullet
	{	
		mat4 aT, bT;
		epget->playerShoot = true;
		switch (I_fpscount % 3)
		{
		case 0:
			epget->bulX = gBoss->fEnemyX - 2.0;
			epget->bulY = gBoss->fEnemyY;
			aT = Translate(epget->bulX, epget->bulY, 0);
			epget->setTRSMatrix(aT);
			break;
		case 1:
			epget->bulX = gBoss->fEnemyX ;
			epget->bulY = gBoss->fEnemyY;
			epget->setTRSMatrix(Translate(epget->bulX, epget->bulY, 0));
			break;
		case 2:
			epget->bulX = gBoss->fEnemyX + 2.0;
			epget->bulY = gBoss->fEnemyY;
			aT = Translate(epget->bulX, epget->bulY, 0);
			epget->setTRSMatrix(aT);
			break;
		}
		
		epget = epget->Link;
		eptail->Link = ephead;
		eptail = ephead;
		ephead = epget;

		if (I_fpscount % 4 == 0) {
			enemybulUlt = false;
		}
		
		
		
		
	}
	if (gBoss->blood <= 1) {
		eBulletMove = ephead;//set the value to *pbulletmove(enemy bullet move line)
		for (int i = 0; i < 10; i++) {
			if (eBulletMove->playerShoot == true) {
				eBulletMove->BossUltLine(dt);
			}
			eBulletMove = eBulletMove->Link;
			//i++;
		}
	}




}

void AutomaticPlayerHurt(float dt) //animate player hurt reaction
{	
	g_pRQuad->playerHurtState(dt);
	g_pRQuadA->playerHurtState(dt);
	g_pRQuadB->playerHurtState(dt);
	//pBulletQuad[0]->playerHurtState(dt);

	if (g_pRQuad->playerIsShot == false)
	{
		g_pRQuad->setColor(vec4(0.8549, 0.4392, 0.8392, 1));//ORIGIN COLOR
		g_pRQuadA->setColor(vec4(0.8549, 0.4392, 0.8392, 1));//ORIGIN COLOR
		g_pRQuadB->setColor(vec4(0.8549, 0.4392, 0.8392, 1));//ORIGIN COLOR
		pBulletQuad[0]->setColor(vec4(0.7, 0.7, 0.9, 1));//ORIGIN COLOR
	}
	if (g_pRQuad->playerIsShot == true)
	{
		g_pRQuad->setColor(vec4(0.6, 0.2, 0.2, 1));//HURT COLOR
		g_pRQuadA->setColor(vec4(0.6, 0.2, 0.2, 1));//HURT COLOR
		g_pRQuadB->setColor(vec4(0.6, 0.2, 0.2, 1));//HURT COLOR
		pBulletQuad[0]->setColor(vec4(0.6, 0.2, 0.2, 1));//HURT COLOR
		if (I_fpscount % 3 == 0){
		g_pRQuad->playerIsShot = false;//use for react hurt, not player hp
		}
		

	}

}

void CollideEnemyWithBullet(float dt) 
{
	float a = 0;
	float b = 0;
	//check if collide
	pBulletCollide = phead;
	for (int i = 0; i < LIST_MAX_BULLET; i++)//if hit-- playershoot=false
	{
		//(x1-x2)^2 + (y1-y2)^2 --- math 
		for (int j = 0; j < ENEMYA_NUM; j++)//for A enemy
		{
			if (pBulletCollide->playerShoot == true&&gEnemyA[j]->blood>0) 
			{//A-ccquadex1
				a = (pBulletCollide->bulX - gEnemyA[j]->fEnemyX) * (pBulletCollide->bulX - gEnemyA[j]->fEnemyX);
				b = (pBulletCollide->bulY - gEnemyA[j]->fEnemyY) * (pBulletCollide->bulY - gEnemyA[j]->fEnemyY);
				if (a + b < 0.4f) {
					gEnemyA[j]->blood--;
					//Print("hit");
					pBulletCollide->playerShoot = false;
				}

			}
		}

		//enemy B
		if (pBulletCollide->playerShoot == true && gEnemyB->blood > 0)
		{//A-ccquadex1
			a = (pBulletCollide->bulX - gEnemyB->fEnemyX) * (pBulletCollide->bulX - gEnemyB->fEnemyX);
			b = (pBulletCollide->bulY - gEnemyB->fEnemyY) * (pBulletCollide->bulY - gEnemyB->fEnemyY);
			if (a + b < 1.3f) {
				gEnemyB->blood--;
				//Print("hit");
				pBulletCollide->playerShoot = false;
			}

		}

		//boss
		if (pBulletCollide->playerShoot == true && gBoss->blood > 0)
		{//A-ccquadex1
			if (fFps_count > BOSS_OUT_TIME) {
				a = (pBulletCollide->bulX - gBoss->fEnemyX) * (pBulletCollide->bulX - gBoss->fEnemyX);
				b = (pBulletCollide->bulY - gBoss->fEnemyY) * (pBulletCollide->bulY - gBoss->fEnemyY);
				if (a + b < 1.5f) {
					gBoss->blood--;
					//Print("hit");
					pBulletCollide->playerShoot = false;
				}
			}
			
		}
		pBulletCollide = pBulletCollide->Link;
	}

	//enemy bullet collide(hit player)
	float c = 0;
	float d = 0;
	eBulletCollide = ephead;
	for (int i = 0; i < 10; i++)
	{	
		if (g_pRQuad -> blood > 0 && eBulletCollide->playerShoot == true)
		{
			if (BossTime == true) {
				c = (eBulletCollide->bulX - g_fTx) * (eBulletCollide->bulX - g_fTx);
				d = (eBulletCollide->bulY - g_fTy) * (eBulletCollide->bulY - g_fTy);
				if (c + d < 0.5f) {
					g_pRQuad->blood--;
					g_pRQuad->playerHurt = true;
					g_pRQuad->playerIsShot = true;//reaction of player
					eBulletCollide->playerShoot = false;//is bullet exist 
					Print("player is hit by bullet");
				}
			}

		}
		eBulletCollide = eBulletCollide->Link;
	}
	
	//boss hit by missile
	if (BossTime == true && CanShoot == false&& gBoss->blood > 0) {
		c = (gBoss->fEnemyX - fBulletQuadT[0][0]) * (gBoss->fEnemyX - fBulletQuadT[0][0]);
		d = (gBoss->fEnemyY - fBulletQuadT[0][1]) * (gBoss->fEnemyY - fBulletQuadT[0][1]);
		if (c + d < 4.0f) {
			gBoss->blood--;			
			CanShoot = !CanShoot;

		}
	}


}

void onFrameMove(float delta)
{
	Print(g_pRQuad->blood);//player blood
	I_fpscount++;

	f_Clock++;
	if (m_bAutoRotation) AutomaticRotation(delta);

	AutomaticCountHealth(delta);

	if (!CanShoot)AutomaticAddYAxis(delta);
	if (playerState == 1)AutomaticWingOpen(delta);//f
	if (playerState == 2)AutomaticWingClose(delta);//g
	AutomaticBullet(delta);//bullet move

	if (g_pRQuad->playerHurt == true) {
		AutomaticPlayerHurt(delta);//hurt react
	}
	

	CollideEnemyWithBullet(delta);
	

	
	AutomaticEnemy(delta);

	if (fFps_count >= BOSS_OUT_TIME) {
		BossTime = true;
	}
	else {
		AutomaticInstantiateEnemy(delta);

	}

	fFps_count++;
	
	Print(f_Clock);

	GL_Display();
}
//---------------------------------------------------------------------------
void CreateBullet(void) 
{	//player bullet
	mat4 pgetT;
	for (int i = 0; i < LIST_MAX_BULLET; i++)
	{
		pget = new CBulletData;		//set link list
		pget->setShader(g_mxModelView, g_mxProjection);
		pget->bulX = g_fTx;
		pget->bulY = g_fTy;	
		pgetT = Translate(pget->bulX, pget->bulY, 0);
		pget->setTRSMatrix(pgetT);
		pget->Link = NULL;

		if (i == 0) {
			phead = pget;
		}
		else //i=1,2,3.....
		{
			ptail->Link = pget;
		}
		
		ptail = pget;
	}
	pget=phead;
	//while (pget != NULL) //output 
	//{
	//	pget = pget->Link;
	//	i++;
	//}

	//------------------------------------------------------------
	//enemy bullet
	mat4 epgetT;
	for (int i = 0; i < 10; i++)
	{
		epget = new CBulletData;		//set link list
		epget->setShader(g_mxModelView, g_mxProjection);
		epget->bulX = g_fTx;
		epget->bulY = g_fTy;
		epgetT = Translate(epget->bulX, epget->bulY, 0);
		epget->setTRSMatrix(epgetT);
		epget->Link = NULL;

		if (i == 0) {
			ephead = epget;
		}
		else //i=1,2,3.....
		{
			eptail->Link = epget;
		}

		eptail = epget;
	}
	epget = ephead;


}

void CreateBackGround() 
{
	GLuint uiShaderHandle;
	vec4 BGColor = vec4(0.6, 0.6, 0.6, 1);
	
	
	for (int i = 0; i < BG_SIZE; i++)
	{
		pBg[i] = new CBG;
		pBg[i]->setColor(BGColor);//gray
		BgT[i] =Translate((i-4)*3, 0, 0);

		pBg[i]->setShader(g_mxModelView, g_mxProjection);
		pBg[i]->setTRSMatrix(BgT[i]);

	}

}

void CreateEnemy() 
{
	GLuint uiShaderHandle;
	vec4 OrangeColor = vec4(1, 0.6174, 0, 1);
	vec4 GreenColor = vec4(0, 1, 0.6 ,1);
	vec4 AColor = vec4(0.388, 0.611, 0.913, 1);//blue

	mat4 mxEnemyT, mxEnemyBT;

	for (size_t i = 0; i < ENEMYA_NUM; i++)
	{
		gEnemyA[i] = new CQuadEX1;
		gEnemyA[i]->setColor(GreenColor);
		mxEnemyT = Translate(11, 12, 0);
		gEnemyA[i]->setShaderName("vsVtxColorEnemyA.glsl", "fsVtxColor.glsl");
		gEnemyA[i]->setShader(g_mxModelView, g_mxProjection);
		gEnemyA[i]->setTRSMatrix(mxEnemyT);
	
	}
	for (int i = 0; i < ENEMYA_NUM; i++)gEnemyA[i]->fEnemyX = 3;//set init posx
	


	gEnemyB = new EnemyB;
	gEnemyB->setColor(GreenColor);
	mxEnemyBT = Translate(5, 0, 0);
	gEnemyB->setShaderName("vsVtxColor.glsl", "fsVtxColor.glsl");
	gEnemyB->setShader(g_mxModelView, g_mxProjection);
	gEnemyB->setTRSMatrix(mxEnemyBT);
	gEnemyB->fEnemyX = -15;//enemyB init pos



	gBoss = new Boss;
	//gBoss->setColor(OrangeColor);
	gBoss->setShader(g_mxModelView, g_mxProjection);
	gBoss->setTRSMatrix(mxEnemyBT);
	gBoss->fEnemyX = -10;// init pos
	gBoss->fEnemyY = 10;
}

void CreateUI() 
{
	//hp health
	GLuint uiShaderHandle;
	vec4 hpColor = vec4(0.8, 0.1, 0.1, 1);
	mat4 hpPosT[HP];
	mat4 SmallT;
	for (int i = 0; i < HP; i++)
	{	
		int x = i * 2;
		heartHp[i] = new CQuadEX2B;
		heartHp[i]->setColor(hpColor);
		//hpPosT[i] = Translate(fHp[i][0], fHp[i][1], fHp[i][2]);
		hpPosT[i] = Translate(9.5 + x, 13.5, 0);
		SmallT = Scale(0.35, 0.35, 0);
		heartHp[i]->setShader(g_mxModelView, g_mxProjection);
		heartHp[i]->setTRSMatrix(mxRT * hpPosT[i]*SmallT);

	}

}

void CreateQuadRelationship()
{
	GLuint uiShaderHandle;
	vec4 vColor = vec4( 1, 0, 0, 1);
	vec4 PurpleColor = vec4(1, 0, 1, 1);
	vec4 Yellow = vec4(0.5, 0.5, 1, 1);

	vec4 vT;
	mat4 mxT;
	int idx = 0;

	// Red Quad 放在螢幕正中間
	g_pRQuad = new CQuad;
	g_pRQuad->setColor(orichidColor);
	g_fRQuadT[0] = g_fRQuadT[1] = g_fRQuadT[2] = 0;
	mxRT = Translate(g_fRQuadT[0],g_fRQuadT[1],g_fRQuadT[2]);
	g_pRQuad->setShader(g_mxModelView, g_mxProjection);
	g_pRQuad->setTRSMatrix(mxRT);

	g_pRQuadA = new CQuad;
	g_pRQuadA->setColor(orichidColor);
	g_fRQuadAT[0] = 0.75;
	g_fRQuadAT[1] = -0.75;
	g_fRQuadAT[2] = 0 ;
	mxRAT = Translate(g_fRQuadAT[0], g_fRQuadAT[1], g_fRQuadAT[2]);
	g_pRQuadA->setShader(g_mxModelView, g_mxProjection);
	g_pRQuadA->setTRSMatrix(mxRT * mxRAT);

	g_pRQuadB = new CQuad;
	g_pRQuadB->setColor(orichidColor);
	g_fRQuadBT[0] = -0.75;
	g_fRQuadBT[1] = -0.75;
	g_fRQuadBT[2] = 0;
	mxRBT = Translate(g_fRQuadBT[0], g_fRQuadBT[1], g_fRQuadBT[2]);
	g_pRQuadB->setShader(g_mxModelView, g_mxProjection);
	g_pRQuadB->setTRSMatrix(mxRT*mxRBT);

	//fire
	Pfire = new Cfire;
	Pfire->setColor(vec4(0.9,0.4,0.4,1));
	Pfire->setShader(g_mxModelView, g_mxProjection);
	Pfire->setTRSMatrix(mxRT*sfireT);

	//bullet quad--------------------------------
	vColor.x = 0; vColor.y = 1;vColor.z=1;
	//pos
	fBulletQuadT[0][0] = 0; fBulletQuadT[0][1] = 0.5; fBulletQuadT[0][2] = 0;
	fBulletQuadT[1][0] = -2; fBulletQuadT[1][1] = 2; fBulletQuadT[1][2] = 0;
	

	for (int i = 0; i < 1; i++)
	{	//new a quad
		pBulletQuad[i] = new CMfollow;
		pBulletQuad[i]->setColor(vec4(0.7, 0.7, 0.9, 1));
		mxBullT[i] = Translate(fBulletQuadT[i][0], fBulletQuadT[i][1], fBulletQuadT[i][2]);
		pBulletQuad[i]->setShader(g_mxModelView, g_mxProjection);
		//pBulletQuad[i]->setTRSMatrix(mxRT * mxBullT[i]);

	}

	// 放在 Red Quad 上下左右四個方向，距離各為 GDISTANCE -- quad below
	//防禦裝置-------------------------------------------------------
	g_fGQuadT[0][0] =  GDISTANCE; g_fGQuadT[0][1] = g_fGQuadT[0][2] = 0;
	g_fGQuadT[1][0] = -GDISTANCE; g_fGQuadT[1][1] = g_fGQuadT[1][2] = 0;
	g_fGQuadT[2][0] = g_fGQuadT[2][2]; g_fGQuadT[2][1] =  GDISTANCE;
	g_fGQuadT[3][0] = g_fGQuadT[3][2]; g_fGQuadT[3][1] = -GDISTANCE;

	for( int i = 0 ; i < 4 ; i++ ) {
		g_pGQuad[i] = new CQuad;
		g_pGQuad[i]->setColor(Yellow);
		mxGT[i] = Translate(g_fGQuadT[i][0], g_fGQuadT[i][1], g_fGQuadT[i][2]);
		g_pGQuad[i]->setShader(g_mxModelView, g_mxProjection);
		//g_pGQuad[i]->setTRSMatrix(mxRT*mxGT[i]);
	}
	

}
//----------------------------------------------------------------------------

void UpdateMatrix()
{
	mat4 mxGR, mxBR;
	mat4 downTF;
	

	mxGR = RotateZ(g_fGAngle);

	//fire
	downTF = Translate(0, -8, 0);
	Pfire->setTRSMatrix(mxMoveT * mxRT * sfireT*downTF);


	//moveT is the mouse move matrix
	g_pRQuad->setTRSMatrix(mxMoveT * mxRT);
	
	switch (playerState) 
	{
	case 0:
		g_pRQuadA->setTRSMatrix(mxMoveT * mxRT * mxRAT);
		g_pRQuadB->setTRSMatrix(mxMoveT * mxRT * mxRBT);
		break;
	case 1:
		g_pRQuadA->setTRSMatrix(mxMoveT*mxIsOpenA);
		g_pRQuadB->setTRSMatrix(mxMoveT*mxIsOpenB);
		break;
	case 2:
		g_pRQuadA->setTRSMatrix(mxMoveT * mxIsCloseA);
		g_pRQuadB->setTRSMatrix(mxMoveT * mxIsCloseB);
		break;
	}




	for( int i = 0 ; i < 4 ; i++ ) {    //the child -- protectSystem
		if (i == 2 || i == 3) {
			g_pGQuad[i]->setTRSMatrix(mxMoveT * mxRT * mxGR * mxGT[i] * turnT * mxGRSmall);
		}
		else {
			g_pGQuad[i]->setTRSMatrix(mxMoveT * mxRT * mxGR * mxGT[i] * mxGRSmall);
		}
	}

	//bullet from player and stay on player
	if (CanShoot) {
		pBulletQuad[0]->setTRSMatrix( mxMoveT*mxRT * mxBullT[0]);
		
	}
	
	
	
	
	
}
//----------------------------------------------------------------------------

void Win_Keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case  SPACE_KEY:
		playerbul = true;
		break;
	case 68: // D key
	case 100: // d key 
		playerState = 0;	//change player appearence---default
		break;
	case 102://f key  
		playerState = 1;	//change player appearence--close wing
		break;
	case 103://g key
		playerState = 2;	//change player appearence--open wing
		break;
	case 104://h key
		playerState = 3;    //change player appearence--shoot boost
		break;
	case 82: // R key
		g_pRQuad->playerIsShot = !g_pRQuad->playerIsShot;//HURT

		break;
	case 114: // r key--follow bullet
		CanShoot = !CanShoot;
		mxOriginT = mxMoveT;
		fBulletQuadT[0][0] = g_fTx;
		fBulletQuadT[0][1] = g_fTy;
		break;
	case 101: // e key
		m_bAutoRotation = !m_bAutoRotation;
		break;
	case 113: // q key
		//BossTime = !BossTime;
		IsFire = !IsFire;
		break;
	case 69://E key
		break;

    case 033:
		glutIdleFunc( NULL );
		delete g_pRQuad;
		delete g_pRQuadA;
		delete g_pRQuadB;
		delete Pfire;
		for( int i = 0 ; i < GRID_SIZE ; i++ ) delete g_pGQuad[i];
		delete pBulletQuad[0];

		for (int i = 0; i < ENEMYA_NUM; i++)delete gEnemyA[i];
		delete gEnemyB;
		delete gBoss;

		for (int i = 0; i < BG_SIZE; i++) delete pBg[i];
		
		for (int i = 0; i < HP; i++) delete heartHp[i];


        exit( EXIT_SUCCESS );
        break;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
			if ( state == GLUT_DOWN ) ;  
			break;
		case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 
			if ( state == GLUT_DOWN )  ;
			break;
		case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
			if ( state == GLUT_DOWN )  ; 
			break;
		default:
			break;
	} 
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	
	

	
}
//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	
	mat4 mxaaT ;
	g_fTx = 15.0f * (x - HALF_SIZE) / (HALF_SIZE);
	g_fTy = -15.0f * (y - HALF_SIZE) / (HALF_SIZE);
	mxaaT = Translate(g_fTx, g_fTy, 0);

	mxMoveT = mxaaT * mxRT;

	UpdateMatrix();


}

//use keyboard to move charactor
void Win_SpecialKeyboard(int key, int x, int y) {
	mat4 rx, ry, rz;
	switch(key) {
		case GLUT_KEY_LEFT:		// 目前按下的是向左方向鍵，移動 Red 往左
			/*g_fRQuadT[0] -= 0.25;
			mxRT = Translate(g_fRQuadT[0],g_fRQuadT[1],g_fRQuadT[2]);
			UpdateMatrix();*/
			break;
		case GLUT_KEY_RIGHT:		// 目前按下的是向右方向鍵，移動 Red 往右
			//g_fRQuadT[0] += 0.25;
			//mxRT = Translate(g_fRQuadT[0],g_fRQuadT[1],g_fRQuadT[2]);
			//UpdateMatrix();
			break;
		case GLUT_KEY_UP:		// 目前按下的是向右方向鍵，移動 Red 往右
			//g_fRQuadT[1] += 0.25;
			//mxRT = Translate(g_fRQuadT[0],g_fRQuadT[1],g_fRQuadT[2]);
			//UpdateMatrix();
			break;
		case GLUT_KEY_DOWN:		// 目前按下的是向右方向鍵，移動 Red 往右
			//g_fRQuadT[1] -= 0.25;
			//mxRT = Translate(g_fRQuadT[0],g_fRQuadT[1],g_fRQuadT[2]);
			//UpdateMatrix();
			break;
		default:
			break;
	}
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    
	glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( SCREEN_SIZE, SCREEN_SIZE );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "Shoot Game" );

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);

	glutPassiveMotionFunc(Win_PassiveMotion);//add follow mouse

    glutKeyboardFunc( Win_Keyboard );	// 處理 ASCII 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc( Win_SpecialKeyboard);	// 處理 NON-ASCII 按鍵如 F1、Home、方向鍵...等等
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
	

    glutMainLoop();
    return 0;
}
