//In the name of Allah
/*
	Simple Packman

	C 2015 by Hadi Safari
	
	ICSP- fall 94- finall project
	Department of Software Engineering
	School of Electrical and Computer Engineering 
	College of Engineering
	University of Tehran
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include"header.h"
#define ML 30
#define L 100
#define DELAY 50
#define GUN_RANGE 6
#define GUN_BULLETS 15
#define MONSTER_CREATION_RATIO 5
inline void asciiTable(){//print ASCII Table
	clrscr();
	set_color(15);
	for(char ch=-128;ch<127;ch++)
		printf("%d(%c)\t",ch,ch);
	printf("%d(%c)\t",127,127);
}
inline void colorTable(){//print Color Table
	clrscr();
	set_color(15);
	for(int i=0;i<256;i++){
		set_color(i);
		printf("%d\t",i);
	}
}
struct monster{	//any node
	int pos[2];
	struct monster *next;
};
inline int abs(int n){	//calculate absolute value of an integer
	return n<0?(-1)*n:n;
}
inline void gotop(int x, int y){	//make gotoxy easier for use :-"
	gotoxy(y+5,x+7);
}
inline void printHeader(){
	set_color(15);
	gotoxy(2,0);
	printf("Simple Packman\tC 2015 Hadi Safari\t(for ICSP Fall 94)");
}
inline void printErr(char msg[]){	//print errors in a more beautiful way
	clrscr();
	printHeader();
	gotoxy(3,3);
	set_color(12);
	printf("ERR\t");
	set_color(15);
	printf("%s\n",msg);
}
int addMons(struct monster *head, int x, int y){	//add monsters to a given position
	struct monster *newMons=(struct monster*)malloc(sizeof(struct monster));
	if(!newMons){
		printErr("Couldn't add new node to linked list.");
		return -1;//unsuccessfull
	}
	newMons->pos[0]=x;
	newMons->pos[1]=y;
	newMons->next=head->next;
	head->next=newMons;
	gotop(x,y);
	set_color(12);
	printf("%c",-50);//print new monster
	return 0;//successfull
}
int addRandMons(int mapLength[2], int wall[ML][ML], int burnPos[2], int bulletPos[7], struct monster *head){
	int flag=1, i,j;
	while(flag){
		flag=0;
		i=rand()%mapLength[0];	//determine a random position
		j=rand()%mapLength[1];
		if(wall[i][j]){	//check if random position is not on the wall :|
			flag=1;
			continue;
		}
		if((abs(i-burnPos[0])<GUN_RANGE)||abs((j-burnPos[1])<GUN_RANGE)){	//ensure that random position is out of range of gun
			flag=1;
			continue;
		}
		if(bulletPos[6]&&((i==bulletPos[0]&&j==bulletPos[1])||((GUN_RANGE-bulletPos[4])&&(i==bulletPos[0]&&j==bulletPos[1]+bulletPos[3])||(i==bulletPos[0]+bulletPos[2]&&j==bulletPos[1])||(i==bulletPos[0]+bulletPos[2]&&j==bulletPos[1]+bulletPos[3])))){	//ensure that the shutted bullet won't kill monster untill monster's next turn.
			flag=1;
			continue;
		}
		struct monster *mon=head->next;
		while(mon){	//ensure that there isn't any other monster in the position
			if(i==mon->pos[0] && j==mon->pos[1]){
				flag=1;
				break;
			}
			mon=mon->next;
		}
	}
	return addMons(head,i,j);
}
int map(int mapLength[2], int wall[ML][ML], int burnPos[2], int end[2], struct monster *head){	//print map
	clrscr();
	printHeader();
	gotoxy(0,3);
	for(int i=0;i<ML;i++)
		for(int j=0;j<ML;j++)
			wall[i][j]=0;	//making wall matrix free
	char add[L];	//map address
	printf("Enter address of map:\t");
	scanf("%99s",add);
	clrscr();
	FILE* mapPtr=fopen(add,"r");	//reading map
	if(!mapPtr){
		printErr("Couldn't open map.");
		return -1;//unsuccessfull
	}
	int i=0,j=0;
	do{
		char inputChar;
		int check=fscanf(mapPtr,"%c",&inputChar);
		if(!inputChar){
			printErr("Couldn't read map.");
			fclose(mapPtr);
			return -1;//unsuccessfull
		}
		switch(inputChar){
		case '#'://wall
			wall[i][j]=1;
			gotop(i,j);
			set_color(14);
			printf("%c",-79);
			break;
		case '$'://start point
			burnPos[0]=i;
			burnPos[1]=j;
			gotop(i,j);
			set_color(15);
			printf("%c",-2);
			break;
		case '@'://end point
			end[0]=i;
			end[1]=j;
			wall[i][j]=2;//2 means end point 
			gotop(i,j);
			set_color(15);
			printf("%c",-108);
			break;
		case 'M'://Monster
			if(addMons(head,i,j)){
				fclose(mapPtr);
				return -1;//unsuccessfull
			}
			break;
		case '.'://free space
			gotop(i,j);
			set_color(7);
			printf("%c",-80);
			break;
		case '\n'://newline
			if(i)
				mapLength[0]=i;
			if(j)
				mapLength[1]=j;
			j=-1;
			i++;
			break;
		default:	//unvalid character
			printErr("Unvalid map.");
			fclose(mapPtr);
			return -1;//unsuccessfull
		}
		j++;
		if(i>=ML || j>=ML){
			printErr("Map is too huge.");
			fclose(mapPtr);
			return -1;//unsuccessfull
		}
	}while(!feof(mapPtr));
	fclose(mapPtr);
	set_color(15);
	gotoxy(2,2);
	printf("Map:\t%s",add);	//print name of map
	gotop(burnPos[0],burnPos[1]);
	return 0;
}
void freeList(struct monster *head){	//free all nodes of linked list
	struct monster *mons=head;
	if(!(mons->next)){
		free(mons);
		return;
	}
	if(mons->next->next)
		freeList(mons->next);
	else{
		free(mons->next);
		mons->next=NULL;
	}
}
void clean(int i,int j, int wall[ML][ML]){	//clean any position of map (only a visual work.)
	gotop(i,j);
	set_color(15);
	if(wall[i][j])	//==2 endPos
		printf("%c",-108);
	else	//==0 free space
		printf("%c",-80);
}
int killMons(int i, int j, int wall[ML][ML], struct monster *head){	//kill any monster on the given position
	struct monster *mons=head;
	if(!head){
		printErr("Unvalid list.");
		return -1;//unsuccessfull
	}
	int ret=-1;
	while((mons->next)!=NULL){
		if(mons->next->pos[0]==i && mons->next->pos[1]==j){
			struct monster *temp=mons->next;
			mons->next=mons->next->next;
			clean(temp->pos[0],temp->pos[1],wall);//clean the position
			free(temp);
			ret=0;
		}
		else
			mons=mons->next;
	}
	if(ret==-1)
		printErr("Couldn't find killed monster. Did you play with sprites :-?");
	return ret;//unsuccessfull
}
inline void loose(int *continueFlag){	//loosing the game
	gotoxy(2,5);
	set_color(12);
	printf("You LOST this set!");
	*continueFlag=0;	//break the main loop
}
inline void victory(int *continueFlag, int bulletPos[7]){	//winning the game
	gotoxy(2,5);
	set_color(10);
	printf("You WON this set! ");
	set_color(15);
	printf("Your score is %d.",bulletPos[5]);
	*continueFlag=0;	//break the main loop
}
int moveMons(int mapLength[2],int wall[ML][ML],int burnPos[2], int bulletPos[7], struct monster *head,int *cont){	//moving all monsters on the map
	struct monster *mons=head;
	if(!head){
		printErr("Unvalid list.");
		return -1;//unsuccessfull
	}
	while(mons->next){
		mons=mons->next;
		int i,j;
		do{
			i=mons->pos[0];
			j=mons->pos[1];
			if(rand()%2){//determine this turn is vertical or horizontal
				if(rand()%2)
					i++;//forward
				else
					i--;//backward
			}
			else{
				if(rand()%2)
					j++;//forward
				else
					j--;//backward
			}
			if(i<0 || i>=mapLength[0] || j<0 || j>=mapLength[1] || wall[i][j]==1)//ensure that new position is valid
				continue;
			break;
		}while(1);
		clean(mons->pos[0],mons->pos[1],wall);//clean previous position
		mons->pos[0]=i;
		mons->pos[1]=j;
	}
	mons=head;
	set_color(12);
	while(mons->next){
		mons=mons->next;
		gotop(mons->pos[0],mons->pos[1]);
		printf("%c",-50);//printing monster
		if(burnPos[0]==mons->pos[0] && burnPos[1]==mons->pos[1])//check if monster has eaten gamer
			loose(cont);
		if(bulletPos[6] && mons->pos[0]==bulletPos[0] && mons->pos[1]==bulletPos[1])//check if monster has committed a suicide! :D
			return killMons(mons->pos[0],mons->pos[1],wall,head);
	}
	return 0;//successfull
}
int checkMonsBull(int bulletPos[7], int wall[ML][ML], struct monster *head){ //check if bullet reach any monster
	struct monster *mons=head;
	if(!head){
		printErr("Unvalid list.");
		return -1;//unsuccessfull
	}
	while(mons->next){
		mons=mons->next;
		if(bulletPos[6] && mons->pos[0]==bulletPos[0] && mons->pos[1]==bulletPos[1]){
			bulletPos[0]=bulletPos[1]=bulletPos[2]=bulletPos[3]=bulletPos[4]=bulletPos[6]=0;
			return killMons(mons->pos[0],mons->pos[1],wall,head);
		}
	}
	return 0;
}
int checkBurnMons(int burnPos[2], struct monster *head, int *continueFlag){	//check if gamer has committed suicide. :-"
	struct monster *mons=head;
	if(!head){
		printErr("Unvalid list.");
		return -1;//unsuccessfull
	}
	while(mons->next){
		mons=mons->next;
		if(burnPos[0]==mons->pos[0] && burnPos[1]==mons->pos[1])
			loose(continueFlag);
	}
	return 0;//successfull
}
inline void printBull(int bPos[7]){	//print bullets on CLI
	gotoxy(2,3);
	set_color(15);
	printf("Bullets:\t");
	set_color(13);
	for(int i=0;i<GUN_BULLETS;i++)
		printf("%c",i<bPos[5]?-8:' ');
}
int main(){
	init_console_functions();
	srand(time(NULL));
	/**///asciiTable();
	/**///colorTable();
	/**///return 0;
	int wall[ML][ML],//wall matrxi (0 for free, 1 for wall, 2 for end point.) 
		pos[2],//position of gamer
		end[2],//position of end point
		mpl[2],//length of map
		bPos[7]={0,0,0,0,0,GUN_BULLETS,0};//bPos[7]={X,Y,X-Axis move,Y-Axis move, moved, left bullets, is_shutted}
	struct monster *monHead=(struct monster*)malloc(sizeof(struct monster));//head of linked list of monsters
	if(!monHead){
		printErr("Couldn't make linked list");
		while(!kbhit());
		return -1;//unsuccessfull
	}
	monHead->next=NULL;
	if(map(mpl,wall,pos,end,monHead)){
		while(!kbhit());
		freeList(monHead);
		return -1;//unsuccessfull
	}
	printHeader();
	printBull(bPos);
	int evenFlag=0,//to give bullets twice speed than other things
		monsTime=time(NULL),//to determine when add new monster to map
		contFlag=1;//main loop flag
	while(contFlag){
		evenFlag=!evenFlag;
		if(evenFlag){
			if(moveMons(mpl,wall,pos,bPos,monHead,&contFlag)){
				while(!kbhit());
				freeList(monHead);
				return -1;//unsuccessfull
			}
			gotop(pos[0],pos[1]);
			if(kbhit()){//check if any key has been hitted
				char ch;
				ch=getch();
				if(bPos[5]&&!bPos[6]){//there isn't any bullet on the map and there is bullet in the gun :D
					bPos[6]=1;
					switch(ch){//shut
					case 'w'://shut to up
						bPos[2]=-1;
						break;
					case 's'://shut to down
						bPos[2]=1;
						break;
					case 'a'://shut to left
						bPos[3]=-1;
						break;
					case 'd'://shut to right
						bPos[3]=1;
						break;
					default:
						bPos[6]=0;
					}
					if(bPos[6]){
						bPos[5]--;//set left bullets
						bPos[0]=pos[0];
						bPos[1]=pos[1];
						printBull(bPos);
					}
				}
				if(ch==-32){//move
					ch=getch();
					int i=0,j=0,f=1;
					switch(ch){
					case 72://move up
						i=-1;
						break;
					case 80://move down
						i=1;
						break;
					case 75://move left
						j=-1;
						break;
					case 77://move right
						j=1;
						break;
					default:
						f=0;//just a flag
					}
					if(f && pos[0]+i>=0 && pos[0]+i<mpl[0] && pos[1]+j>=0 && pos[1]+j<mpl[1] && wall[pos[0]+i][pos[1]+j]!=1){//check if new position is valid
						clean(pos[0],pos[1],wall);//clean previous position
						pos[0]+=i;//determine new position
						pos[1]+=j;
						gotop(pos[0],pos[1]);
						set_color(15);
						printf("%c",-2);//print gamer
						if(checkBurnMons(pos,monHead,&contFlag)){//check if gamer has committed suicide. 
							while(!kbhit());
							freeList(monHead);
							return -1;//unsuccessfull
						}
						if(pos[0]==end[0] && pos[1]==end[1])//check if gamer reaches end point
							victory(&contFlag,bPos);
					}
				}
				fflush(stdin);
			}
			if((int)time(NULL)-monsTime>=MONSTER_CREATION_RATIO){//create new monters
				monsTime=time(NULL);
				if(addRandMons(mpl,wall,pos,bPos,monHead)){
					while(!kbhit());
					freeList(monHead);
					return -1;//unsuccessfull
				}
			}
		}
		if(bPos[6]){//shutted
			if(bPos[4])//not first move
				clean(bPos[0],bPos[1],wall);
			bPos[4]++;//increase moved positions
			bPos[0]+=bPos[2];
			bPos[1]+=bPos[3];
			if(bPos[4]>GUN_RANGE || bPos[0]<0 || bPos[0]>=mpl[0] || bPos[1]<0 || bPos[1]>=mpl[1] || wall[bPos[0]][bPos[1]]==1)
				bPos[0]=bPos[1]=bPos[2]=bPos[3]=bPos[4]=bPos[6]=0;
			if(bPos[6]){
				set_color(13);
				gotop(bPos[0],bPos[1]);
				printf("%c",-8);//print bullet
				if(checkMonsBull(bPos,wall,monHead)){//check if bullet kills any monster
					while(!kbhit());
					freeList(monHead);
					return -1;//unsuccessfull
				}
			}
		}
		delay(DELAY);//delay :|
	}
	freeList(monHead);
	while(!kbhit());
	gotoxy(0,1);
	set_color(15);
	return 0;
}
