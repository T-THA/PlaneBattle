
#include<graphics.h>
#include<conio.h>
#include<math.h>
#include<stdio.h>
#include<time.h>
#include<Windows.h>

#define X 510
#define Y 680
// 窗口的大小是510*680 飞机的大小是80*80
IMAGE bg;
IMAGE khn2,khn,ena,knd,mfy,mzkboss;
IMAGE an_khn2,an_khn,an_ena,an_knd,an_mfy,an_mzkboss;
IMAGE bullet,bullet2,prop1,prop2,prop3;
IMAGE an_bullet,an_bullet2,an_prop1,an_prop2,an_prop3;

void pl_action();
void en_action();
void bu_action();
void pr_action();
void crash();
void Drawinit();//图像加载进来
void imainit();
void playerinit();
void cr_enemy();
void cr_plbullet();
void cr_plbullet_buff();
void cr_prop(int x,int y);
void stage1();
void stage2();
void stage3();

typedef struct Plane{//能动的东西（包括自己，敌人，道具，子弹）相当于一个父类
   int x=0;
   int y=0;
   int alive=0;
   int HP=0;
   int type=0; // 0为玩家 子弹 
   // enemy模式下 1 为knd啥都不会干，只能前进  2为mfy会发射子弹  3为ena会发射子弹且血厚
   // buff模式下 1 攻击模式 2治疗 3子弹伤害
}plane;

plane player;
plane pl_bullet[100];
plane enemy[30];
plane en_bullet[30][100];
plane prop[30];

DWORD ti_shoot1,ti_shoot2;
DWORD ti_crenemy1,ti_crenemy2;


//因为子弹速度太快 用这个来控速
int bu_action_limit;
int pl_action_limit;
int en_action_limit;
int pr_action_limit;

int is_buffed[4];

int main(){
   // initgraph(X,Y,SHOWCONSOLE);//showconsole展示黑窗口
   initgraph(X,Y);
   Drawinit();
   playerinit();
   //游戏初始界面加载
   //进入关卡
   //移动飞机，进入游戏主题
   
   //双缓冲绘图 解决闪烁问题
   BeginBatchDraw();
   
   while(1){
      imainit();
      FlushBatchDraw();
      if(player.alive) pl_action();
      bu_action();
      en_action();
      pr_action();
      // printf("1\n");//test
      cr_enemy();
      crash();
   }
   EndBatchDraw();
   return 0;
}



void pl_action(){
   pl_action_limit+=1;
   if(GetAsyncKeyState(VK_UP)||GetAsyncKeyState('W')){
      if(player.y>=-40&&pl_action_limit%3==0) player.y-=1;
   }
   if(GetAsyncKeyState(VK_DOWN)||GetAsyncKeyState('S')){
      if(player.y<=Y-40&&pl_action_limit%3==0) player.y+=1;
   }
   if(GetAsyncKeyState(VK_LEFT)||GetAsyncKeyState('A')){
      if(player.x>=-40&&pl_action_limit%3==0) player.x-=1;
   }
   if(GetAsyncKeyState(VK_RIGHT)||GetAsyncKeyState('D')){
      if(player.x<=X-40&&pl_action_limit%3==0) player.x+=1;
   }
   if(bu_action_limit>=3) bu_action_limit%=3;
   //得加嗑药的判断
   if(GetAsyncKeyState(VK_SPACE)){
      ti_shoot2=GetTickCount();
      if((ti_shoot2-ti_shoot1>=150)){
         cr_plbullet();
         ti_shoot1=ti_shoot2;
      }
      
   }
}

void en_action(){
   en_action_limit+=1;
   for(int i=0;i<30;i++){
      if(enemy[i].alive&&enemy[i].HP<=0){
         enemy[i].alive=0;
         cr_prop(enemy[i].x,enemy[i].y);
      }
      if(enemy[i].alive&&en_action_limit%10==0){
         enemy[i].y-=1;
      }
   }
   if(en_action_limit>=4) en_action_limit%=10;
}

void bu_action(){
   bu_action_limit+=1;
   for(int i=0;i<30;i++){
      if(pl_bullet[i].y<=0){
         pl_bullet[i].alive=0;
      }
   
      if(pl_bullet[i].alive){
         if(is_buffed[1]){
            pl_bullet[i].y-=1;
         }else if(bu_action_limit%2==0){
            pl_bullet[i].y-=1;
         }
      }
   }

   if(bu_action_limit>=2) bu_action_limit%=2;
}


void pr_action(){
   pr_action_limit+=1;
   for(int i=0;i<30;i++){
      if(prop[i].alive&&pr_action_limit%40==0&&prop[i].y<=640) prop[i].y+=1;
   }

   if(pr_action_limit>=40) pr_action_limit%=40;
}


void crash(){
   for(int i=0;i<30;i++){
      if(pl_bullet[i].alive){
         for(int j=0;j<30;j++){
            if(enemy[j].alive){
               //有待修改
               if((pl_bullet[i].x>=enemy[j].x&&pl_bullet[i].x<=enemy[j].x+80)
                  &&(pl_bullet[i].y<=enemy[j].y+100&&pl_bullet[i].y>=enemy[j].y)){
                     //改成扣血
                  pl_bullet[i].alive=0;
                  if(is_buffed[3]){
                     enemy[j].HP-=2;
                  }else{
                     enemy[j].HP-=1;
                  }
                  break;
               }
            }
         }
      }

      if(player.alive){
         if(prop[i].alive){
            if(prop[i].x-75<=player.x&&prop[i].x+40>=player.x
            &&prop[i].y-75<=player.y&&prop[i].y+40>=player.y){
               prop[i].alive=0;
               
               int tmp=prop[i].type;
               is_buffed[tmp]=1;
            }
         }
      }
   }
}


void cr_plbullet(){
   int count=0;
   for(int i=0;i<30;i++){
      if(!pl_bullet[i].alive){
         switch(count){
            case 0:
               pl_bullet[i].x=player.x+40;
               break;
            case 1:
               pl_bullet[i].x=player.x;
               break;
            case 2:
               pl_bullet[i].x=player.x+80;
               break;
         }

         pl_bullet[i].y=player.y;
         pl_bullet[i].alive=1;
         count+=1;
      }
      if(is_buffed[1]){
         if(count==3) break;
      }else{
      if(count==1) break;
      }
   }
}

void cr_enemy(){
   int count=0;
   ti_crenemy2=GetTickCount();
   if(ti_crenemy2-ti_crenemy1>5000){
      for(int i=0;i<30;i++){
         if(!enemy[i].alive){
            enemy[i].x=rand()%430+40;
            // enemy[i].y=-20;
            enemy[i].y=340;
            enemy[i].alive=1;
            enemy[i].type=rand()%3+1;
            enemy[i].HP=3;
            count+=1;
            ti_crenemy1=ti_crenemy2;
         }
         if(count) break;
      }
   }
}
void cr_prop(int x,int y){
   int tmp=rand()%100;
   if(tmp<=30){
      for(int i=0;i<30;i++){
         if(!prop[i].alive){
            prop[i].x=x;
            prop[i].y=y;
            prop[i].alive=1;
         
            if(tmp<=10){
               prop[i].type=1;
            }else if(tmp>10&&tmp<=20){
               prop[i].type=2;
            }else{
               prop[i].type=3;
            }

            break;
         }
      }
   }
   
}

void playerinit(){
   player.x=X/2;
   player.y=Y-100;
   player.alive=1;
   player.HP=10;
   

   ti_shoot1=GetTickCount();
   ti_crenemy1=GetTickCount();

   srand(time(NULL));
}


void imainit(){//图像初始化
   putimage(0,0,&bg);
   putimage(player.x,player.y,&an_khn,NOTSRCERASE);//这两行顺序不能换
   putimage(player.x,player.y,&khn,SRCINVERT);
   for(int i=0;i<30;i++){
      if(pl_bullet[i].alive){
         putimage(pl_bullet[i].x,pl_bullet[i].y,&an_bullet2,NOTSRCERASE);//这两行顺序不能换
         putimage(pl_bullet[i].x,pl_bullet[i].y,&bullet2,SRCINVERT);
      }
   }
   for(int i=0;i<30;i++){
      if(enemy[i].alive){
         if(enemy[i].type==1){
            putimage(enemy[i].x,enemy[i].y,&an_knd,NOTSRCERASE);//这两行顺序不能换
            putimage(enemy[i].x,enemy[i].y,&knd,SRCINVERT);
         }else if(enemy[i].type==2){
            putimage(enemy[i].x,enemy[i].y,&an_mfy,NOTSRCERASE);//这两行顺序不能换
            putimage(enemy[i].x,enemy[i].y,&mfy,SRCINVERT);
         }else if(enemy[i].type==3){
            putimage(enemy[i].x,enemy[i].y,&an_ena,NOTSRCERASE);//这两行顺序不能换
            putimage(enemy[i].x,enemy[i].y,&ena,SRCINVERT);
         }     
      }
   }
   for(int i=0;i<30;i++){
      if(prop[i].alive){
         if(prop[i].type==1){
            putimage(prop[i].x,prop[i].y,&an_prop1,NOTSRCERASE);//这两行顺序不能换
            putimage(prop[i].x,prop[i].y,&prop1,SRCINVERT);
         }else if(prop[i].type==2){
            putimage(prop[i].x,prop[i].y,&an_prop2,NOTSRCERASE);//这两行顺序不能换
            putimage(prop[i].x,prop[i].y,&prop2,SRCINVERT);
         }else if(prop[i].type==3){
            putimage(prop[i].x,prop[i].y,&an_prop3,NOTSRCERASE);//这两行顺序不能换
            putimage(prop[i].x,prop[i].y,&prop3,SRCINVERT);
         }   
      }
   }
}


void Drawinit(){//图像先加载进来
   loadimage(&bg,"./mzk.jpg");//510*680的瑞希哥背景
   loadimage(&khn,"./khn.jpg");//80*80
   loadimage(&an_khn,"./antikhn.jpg");
   loadimage(&khn2,"./khn2.jpg");
   loadimage(&an_khn2,"./antikhn2.jpg");
   loadimage(&ena,"./ena.jpg");
   loadimage(&an_ena,"./antiena.jpg");
   loadimage(&knd,"./knd.jpg");
   loadimage(&an_knd,"./antiknd.jpg");
   loadimage(&mfy,"./mfy.jpg");
   loadimage(&an_mfy,"./antimfy.jpg");
   loadimage(&mzkboss,"./mzkboss.jpg");
   loadimage(&an_mzkboss,"./antimzkboss.jpg");
   loadimage(&bullet,"./bullet.jpg");//20*20
   loadimage(&an_bullet,"./antibullet.jpg");
   loadimage(&bullet2,"./bullet2.jpg");//20*20
   loadimage(&an_bullet2,"./antibullet2.jpg");
   loadimage(&prop1,"./prop_changebullet.jpg");
   loadimage(&an_prop1,"./antiprop_changebullet.jpg");
   loadimage(&prop2,"./prop_cure.jpg");
   loadimage(&an_prop2,"./antiprop_cure.jpg");
   loadimage(&prop3,"./prop_energy.jpg");
   loadimage(&an_prop3,"./antiprop_energy.jpg");
}