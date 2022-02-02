
#include<graphics.h>
#include<conio.h>
#include<math.h>
#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")


#define X 510
#define Y 680


IMAGE bg,an_blood,begin_bg,win_bg,lose_bg,blood;
IMAGE khn2,khn,ena,knd,mfy,mzkboss,mzkboss2;
IMAGE an_khn2,an_khn,an_ena,an_knd,an_mfy,an_mzkboss,an_mzkboss2;
IMAGE bullet,bullet2,prop1,prop2,prop3;
IMAGE stage[5],an_stage[5];
IMAGE an_bullet,an_bullet2,an_prop1,an_prop2,an_prop3;


typedef struct Plane{//能动的东西（包括自己，敌人，道具，子弹）相当于一个父类
   int x=0;
   int y=0;
   int alive=0;
   int HP=0;
   int direction=0;
   //移动方向的参数 敌机 0向下  1向上  2向左   3向右 
   //             子弹 0上下  1左下  2右下   3boss 速射
   int type=0;
   // enemy模式下 1knd  2mfy 3ena
   // buff模式下 1攻击 2治疗 3伤害
}plane;


void pl_action();
void en_action();
void en_action_x(int n);
void bu_action();//包括了khn 小怪 boss的子弹
void pr_action();
void boss_action();

void crash();
void Drawinit();//图像加载进来
void Imainit();
void Playerinit();

//create
void cr_enemy();
void cr_boss();
void cr_enbullet(int type,int number); 
void cr_plbullet();       //player bullet
void cr_plbullet_buff(); 
void cr_bossbullet(int n);
void cr_prop(int x,int y);
void cr_stagebg(int n);   //stage background



plane player;
plane boss;
plane boss_bullet[100];
plane pl_bullet[30];
plane enemy[30];
plane en_bullet[30][30];
plane prop[30];
plane stagebg[5];


DWORD ti_shoot1,ti_shoot2;
DWORD ti_crenemy1,ti_crenemy2;
DWORD ti_buff[4];
DWORD ti_crenbu[30][2];
DWORD ti_crbossbu1,ti_crbossbu2;
DWORD ti_bg1,ti_bg2;


//因为子弹速度太快 用这个来控速
int bu_action_limit;
int pl_action_limit;
int en_action_limit;
int pr_action_limit;
int boss_action_limit;

//控制标志
int is_buffed[4];
int stageflag1,stageflag2,stageflag3,stageflag_boss; 
int imageflag0,imageflag1,imageflag2,imageflag3,imageflagboss;
int beginflag;
int gameover;
int bossprop;
int score;   //敌机消灭总数
int num;    //敌机产生总数



int main(){
   initgraph(X,Y);
   Drawinit();
   Playerinit();
   //游戏初始化
   

   
   //双缓冲绘图 解决闪烁问题
   BeginBatchDraw();
   Imainit();
   FlushBatchDraw();

   //放歌
   mciSendString("open bgm.mp3", NULL, 0, NULL);
   mciSendString("play bgm.mp3 repeat", NULL, 0, NULL);

   char tmp=getch();
   beginflag=1;

   while(1){
      if(!gameover)
      {

      Imainit();
      FlushBatchDraw();
      if(player.alive) pl_action();
      bu_action();
      en_action();
      boss_action();
      pr_action();

      if(score==0&&num==0) cr_stagebg(1);
      if(score==10&&num==10){
         stageflag1=1;
         cr_stagebg(2);     
      }
      if(score==34&&num==34){
         stageflag2=1;
         cr_stagebg(3);
      }
      if(score==76&&num==76){
         stageflag3=1;
         cr_stagebg(4);
      }

      if(stageflag1&&stageflag2&&stageflag3
            &&(!stageflag_boss)&&stagebg[4].alive==0){
         cr_boss();
         stageflag_boss=1;
      }else if(!stageflag_boss){

         if(num==0||num==10||num==34||num==36){
            if(num==score){
               if(!(stagebg[0].alive||stagebg[1].alive||stagebg[2].alive
                  ||stagebg[3].alive||stagebg[4].alive)){
                  cr_enemy();
               }
            }
         }else if(num<76) cr_enemy();
      }

      }
         
      crash();

   }
   
   EndBatchDraw();
   return 0;
}



void pl_action(){
   pl_action_limit+=1;

   if(player.alive){
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

      if(pl_action_limit>=3) pl_action_limit%=3;
   
      if(1)    //索性改成了自动发射
   // if(GetAsyncKeyState(VK_SPACE))
      {
         ti_shoot2=GetTickCount();
         if((ti_shoot2-ti_shoot1>=150)&&is_buffed[1]==0){
            cr_plbullet();
            ti_shoot1=ti_shoot2;
         }else if(ti_shoot2-ti_shoot1>=100&&is_buffed[1]==1){
            cr_plbullet();
            ti_shoot1=ti_shoot2;
         }   
      }

      if(player.HP<=0){
         player.alive=0;
      }
   }
   
}

void en_action(){
   en_action_limit+=1;

   for(int i=0;i<30;i++){

      if(enemy[i].alive){

         if(enemy[i].HP<=0){
            enemy[i].alive=0;
            score+=1;
            cr_prop(enemy[i].x,enemy[i].y);
            break;
         }
         
         if(enemy[i].type==1){
            if(en_action_limit%40==0)  enemy[i].y+=1;

            if(enemy[i].y>=610){
               enemy[i].alive=0;
               score+=1;
               player.HP-=1;
            }
            
         }
         if(enemy[i].type==2){
            if(en_action_limit%30==0&&enemy[i].y<=120)  enemy[i].y+=1;
            else if(en_action_limit%60==0&&enemy[i].y>=120) en_action_x(i);
            
            ti_crenbu[i][1]=GetTickCount();
            if(ti_crenbu[i][1]-ti_crenbu[i][0]>=4000){
               cr_enbullet(2,i);
               ti_crenbu[i][0]=ti_crenbu[i][1];
            }
            
         }
         if(enemy[i].type==3){
            if(en_action_limit%60==0&&enemy[i].y<=200)  enemy[i].y+=1;
            else if(en_action_limit%40==0&&enemy[i].y>=200) en_action_x(i);

            ti_crenbu[i][1]=GetTickCount();
            if(ti_crenbu[i][1]-ti_crenbu[i][0]>=3500){
               cr_enbullet(3,i);
               ti_crenbu[i][0]=ti_crenbu[i][1];
            }
         }
      }
   }
  
   if(en_action_limit>=120) en_action_limit%=120;
}

void en_action_x(int n){
   if(enemy[n].direction==2){
      enemy[n].x-=1;
      if(enemy[n].x<=0){
         enemy[n].direction=3;
      }
   }else if(enemy[n].direction==3){
      enemy[n].x+=1;
      if(enemy[n].x>=430){
         enemy[n].direction=2;
      }
   }
}


void boss_action(){
   boss_action_limit+=1;
   
   if(boss.HP<=0&&stageflag_boss==1){
      boss.alive=0;
      score=100;
   }
   if(boss.alive){
      if(boss.HP%50==0&&bossprop!=boss.HP){
         cr_prop(boss.x+150,boss.y+220);
         bossprop=boss.HP;
      }
      if(boss.y<=50&&boss_action_limit%50==0) boss.y+=1;
      if(boss.y==50) ti_crbossbu1=GetTickCount();
      if(boss.y>50&&boss_action_limit%40==0){
         if(boss.direction==2){
            boss.x-=1;
            if(boss.x<=0) boss.direction=3;
         }if(boss.direction==3){
            boss.x+=1;
            if(boss.x>=210) boss.direction=2;
         }

         ti_crbossbu2=GetTickCount();
         if(ti_crbossbu2-ti_crbossbu1>=1000){
            cr_bossbullet(rand()%3);
            ti_crbossbu1=ti_crbossbu2;
      }
      }
      
   }
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

      for(int j=0;j<30;j++){
         if(en_bullet[i][j].y>=680){
            en_bullet[i][j].alive=0;
         }
         if(en_bullet[i][j].alive){
            if(bu_action_limit%8==0){
               switch(en_bullet[i][j].direction){
                  case 0:
                     en_bullet[i][j].y+=2;
                     break;
                  case 1:
                     en_bullet[i][j].y+=2;
                     en_bullet[i][j].x+=1;
                     break;
                  case 2:
                     en_bullet[i][j].y+=2;
                     en_bullet[i][j].x-=1;
                     break;
               }
            }
         }
      }
   }

   for(int i=0;i<100;i++){
      if(boss_bullet[i].y>=680) boss_bullet[i].alive=0;
      if(boss_bullet[i].alive){
         switch(boss_bullet[i].type){
            case 0:
               if(bu_action_limit%3==0) boss_bullet[i].y+=1;
               break;
            case 1:
               if(bu_action_limit%6==0){
                  boss_bullet[i].y+=2;
                  boss_bullet[i].x-=1;
               }
               break;
            case 2:
               if(bu_action_limit%6==0){
                  boss_bullet[i].y+=2;
                  boss_bullet[i].x+=1;
               }
               break;
            case 3:
               if(bu_action_limit%2==0) boss_bullet[i].y+=1;
               break;
         }
      }
   }
   if(bu_action_limit>=24) bu_action_limit%=24;
}


void pr_action(){
   pr_action_limit+=1;
   for(int i=0;i<30;i++){
      if(prop[i].alive&&pr_action_limit%40==0&&prop[i].y<=640) prop[i].y+=1;
   }

   if(pr_action_limit>=40) pr_action_limit%=40;


   ti_buff[0]=GetTickCount();
   if(is_buffed[1]&&ti_buff[0]-ti_buff[1]>=10000) is_buffed[1]=0;
   if(is_buffed[2]){
      is_buffed[2]=0;
      player.HP+=4;
      if(player.HP>10) player.HP=10;
   }
   if(is_buffed[3]&&ti_buff[0]-ti_buff[3]>=10000) is_buffed[3]=0;

}


void crash(){
   //各种碰撞
   for(int i=0;i<30;i++){
      if(pl_bullet[i].alive){
         if(boss.alive){
            if((pl_bullet[i].x>=boss.x+10&&pl_bullet[i].x<=boss.x+213)
                  &&(pl_bullet[i].y<=boss.y+183&&pl_bullet[i].y>=boss.y+10)){
               if(is_buffed[3]){
                  boss.HP-=2;
               }else{
                  boss.HP-=1;
               }
               pl_bullet[i].alive=0;
            }

            if(player.x>=boss.x-60&&player.x<=boss.x+213-70
                  &&player.y>=boss.y-60&&player.y<=boss.y+113){
               player.HP=0;
            }
         }

         for(int j=0;j<30;j++){
            if(enemy[j].alive){
               if((pl_bullet[i].x>=enemy[j].x-5&&pl_bullet[i].x<=enemy[j].x+83)
                     &&(pl_bullet[i].y<=enemy[j].y+70&&pl_bullet[i].y>=enemy[j].y)){

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
               ti_buff[tmp]=GetTickCount();
            }
         }

         if(enemy[i].alive){
            if(enemy[i].x-70<=player.x&&enemy[i].x+80>=player.x
               &&enemy[i].y-80<=player.y&&enemy[i].y+70>=player.y){
                  enemy[i].HP=0;
                  player.HP-=1;
               }
         }
         if(boss.alive){
            for(int i=0;i<100;i++){
               if(boss_bullet[i].alive){
                  if(boss_bullet[i].x-35<=player.x&&boss_bullet[i].x-25>=player.x
                        &&boss_bullet[i].y-35<=player.y&&boss_bullet[i].y-25>=player.y){
                     boss_bullet[i].alive=0;
                     player.HP-=1;
                  }
               }
            }
         }
         for(int j=0;j<30;j++){
            if(en_bullet[i][j].alive){
               if(en_bullet[i][j].x-40<=player.x&&en_bullet[i][j].x-20>=player.x
                     &&en_bullet[i][j].y-40<=player.y&&en_bullet[i][j].y-20>=player.y){
                  en_bullet[i][j].alive=0;
                  player.HP-=1;
               }
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
               pl_bullet[i].x=player.x+30;
               break;
            case 1:
               pl_bullet[i].x=player.x-10;
               break;
            case 2:
               pl_bullet[i].x=player.x+70;
               break;
         }

         pl_bullet[i].y=player.y-10;
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

void cr_enbullet(int type ,int number){
   int count=0;
   if(type>=2){
      for(int i=0;i<30;i++){
         if(!en_bullet[number][i].alive){
            en_bullet[number][i].x=enemy[number].x+45;
            en_bullet[number][i].y=enemy[number].y+70;
            en_bullet[number][i].alive=1;
            en_bullet[number][i].direction=count;
            count+=1;
         }

         if(type==2&&count==1) break;
         if(type==3&&count==3) break;
      }
   }
}

void cr_bossbullet(int n){
   int count=0;
   switch (n){
   case 0://直的
      for(int i=0;i<100;i++){
         if(!boss_bullet[i].alive){
            boss_bullet[i].alive=1;
            boss_bullet[i].type=0;
            boss_bullet[i].x=boss.x+151+125-(count%3)*125;
            boss_bullet[i].y=boss.y+100+(count/3)*45;
            count+=1;
         }
         if(count==24) break;
      }
      break;
   case 1://斜的
      for(int i=0;i<100;i++){
         if(!boss_bullet[i].alive){
            boss_bullet[i].alive=1;
            boss_bullet[i].type=(count+2)%3;
            boss_bullet[i].x=boss.x+180-(count%3)*30;
            boss_bullet[i].y=boss.y+180+(count/3)*60;
            count+=1;
         }
         if(count==12) break;
      }
      break;
   case 2://瞄准play的
      for(int i=0;i<100;i++){
         if(!boss_bullet[i].alive){
            boss_bullet[i].alive=1;
            boss_bullet[i].type=3;
            boss_bullet[i].x=player.x+30; //没有写错
            boss_bullet[i].y=boss.y+150+count*40;
            count+=1;
         }
         if(count==6) break;
      }
      break;
      
   }
}


void cr_enemy(){
   int count=0;
   ti_crenemy2=GetTickCount();
   
   if(ti_crenemy2-ti_crenemy1>6000){
      for(int i=0;i<30;i++){
         if(!enemy[i].alive){
            enemy[i].x=rand()%430;
            enemy[i].y=-20;

            if(enemy[i].x>=210){
               enemy[i].direction=2;
            }else enemy[i].direction=3;
            
            enemy[i].alive=1;
            enemy[i].type=rand()%(1+stageflag1+stageflag2)+1;
            enemy[i].HP=35-5*enemy[i].type;
            
            

            ti_crenemy1=ti_crenemy2;
            ti_crenbu[i][0]=GetTickCount();
            count+=1;
            num+=1;
            
         }
         if(count==2*(1+stageflag1+stageflag2)) break;
      }
   }
   
}

void cr_boss(){
   boss.alive=1;
   boss.x=X/2-150;
   boss.y=-150;
   boss.HP=1500;
   boss.direction=2;
   num+=1;
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

void cr_stagebg(int n){
   if(!stagebg[n].alive){
      stagebg[n].alive=1;
      ti_bg1=GetTickCount();
   }

   ti_bg2=GetTickCount();
   if(ti_bg2-ti_bg1>=5000){
      stagebg[n].alive=0;
      // ti_bg1=ti_bg2;
   }
}

void Playerinit(){
   player.x=X/2;
   player.y=Y-100;
   player.alive=1;
   player.HP=10;
   

   ti_shoot1=GetTickCount();
   ti_crenemy1=ti_shoot1;
   
   srand(time(NULL));
}


void Imainit(){//图像初始化
   if(beginflag){
      if(boss.alive==0&&stageflag_boss==1){
         gameover=1;
         putimage(0,0,&win_bg);
      }else if(player.alive==0){
         gameover=1;
         putimage(0,0,&lose_bg);
      }else{
         putimage(0,0,&bg);
      }
   }else{
      putimage(0,0,&begin_bg);
   }
   
   if(player.alive){
      putimage(player.x,player.y,&an_khn,NOTSRCERASE);//这两行顺序不能换
      putimage(player.x,player.y,&khn,SRCINVERT);
   }else{
      putimage(player.x,player.y,&an_khn2,NOTSRCERASE);
      putimage(player.x,player.y,&khn2,SRCINVERT);
   }

   if(boss.alive&&stageflag_boss){
      putimage(boss.x,boss.y,&an_mzkboss,NOTSRCERASE);
      putimage(boss.x,boss.y,&mzkboss,SRCINVERT);
   }else if(boss.alive==0&&stageflag_boss==1){
      putimage(boss.x,boss.y,&an_mzkboss2,NOTSRCERASE);
      putimage(boss.x,boss.y,&mzkboss2,SRCINVERT);
   }

   for(int i=0;i<100;i++){
      if(boss_bullet[i].alive){
         putimage(boss_bullet[i].x,boss_bullet[i].y,&an_bullet,NOTSRCERASE);
         putimage(boss_bullet[i].x,boss_bullet[i].y,&bullet,SRCINVERT);
      }
   }
   for(int i=0;i<30;i++){
      if(pl_bullet[i].alive){
         putimage(pl_bullet[i].x,pl_bullet[i].y,&an_bullet2,NOTSRCERASE);
         putimage(pl_bullet[i].x,pl_bullet[i].y,&bullet2,SRCINVERT);
      }

      if(prop[i].alive){
         if(prop[i].type==1){
            putimage(prop[i].x,prop[i].y,&an_prop1,NOTSRCERASE);
            putimage(prop[i].x,prop[i].y,&prop1,SRCINVERT);
         }else if(prop[i].type==2){
            putimage(prop[i].x,prop[i].y,&an_prop2,NOTSRCERASE);
            putimage(prop[i].x,prop[i].y,&prop2,SRCINVERT);
         }else if(prop[i].type==3){
            putimage(prop[i].x,prop[i].y,&an_prop3,NOTSRCERASE);
            putimage(prop[i].x,prop[i].y,&prop3,SRCINVERT);
         }   
      }

      if(enemy[i].alive){
         if(enemy[i].type==1){
            putimage(enemy[i].x,enemy[i].y,&an_knd,NOTSRCERASE);
            putimage(enemy[i].x,enemy[i].y,&knd,SRCINVERT);
         }else if(enemy[i].type==2){
            putimage(enemy[i].x,enemy[i].y,&an_mfy,NOTSRCERASE);
            putimage(enemy[i].x,enemy[i].y,&mfy,SRCINVERT);
         }else if(enemy[i].type==3){
            putimage(enemy[i].x,enemy[i].y,&an_ena,NOTSRCERASE);
            putimage(enemy[i].x,enemy[i].y,&ena,SRCINVERT);
         }     
      }

      for(int j=0;j<30;j++){
         if(en_bullet[i][j].alive){
            putimage(en_bullet[i][j].x,en_bullet[i][j].y,&an_bullet,NOTSRCERASE);
            putimage(en_bullet[i][j].x,en_bullet[i][j].y,&bullet,SRCINVERT);
         }
      }

   }
   for(int i=0;i<5;i++){
      if(stagebg[i].alive){
         putimage(stagebg[i].x,stagebg[i].y,&an_stage[i],NOTSRCERASE);
         putimage(stagebg[i].x,stagebg[i].y,&stage[i],SRCINVERT);
      }
   }
   for(int i=0;i<player.HP;i++){
      putimage(0,640-35*i,&an_blood,NOTSRCERASE);
      putimage(0,640-35*i,&blood,SRCINVERT);
   }
}


void Drawinit(){//图像先加载进来
   loadimage(&bg,"./mzk.jpg");//510*680的瑞希哥背景
   loadimage(&begin_bg,"./bg2.jpg");
   loadimage(&win_bg,"./bg4.jpg");
   loadimage(&lose_bg,"./bg3.jpg");
   loadimage(&khn,"./khn.jpg");
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
   loadimage(&mzkboss2,"./mzkboss2.jpg");
   loadimage(&an_mzkboss2,"./antimzkboss2.jpg");
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

   loadimage(&stage[1],"./stage1.jpg");
   loadimage(&an_stage[1],"./antistage1.jpg");
   loadimage(&stage[2],"./stage2.jpg");
   loadimage(&an_stage[2],"./antistage2.jpg");
   loadimage(&stage[3],"./stage3.jpg");
   loadimage(&an_stage[3],"./antistage3.jpg");
   loadimage(&stage[4],"./stageboss.jpg");
   loadimage(&an_stage[4],"./antistageboss.jpg");
   loadimage(&blood,"./blood.jpg");
   loadimage(&an_blood,"./antiblood.jpg");
}