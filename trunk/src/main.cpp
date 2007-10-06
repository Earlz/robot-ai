/**Copyright Jordan Earls <http://Earlz.biz.tm>
All rights reserved.
See "copyright.txt" for information on the copyright of this project and source file
This file is part of the "Robot AI" project.
**/

#include "config.h"
#include "Robots_AI.h"
using namespace std;


SDL_Thread *messaging,*disk_cache,*robots_thread;
SDL_Event event;

Screen display;
volatile int quit_all=0;


//classes..
/**RobotList**/

Robot * RobotList::operator[](unsigned int index){
	return GetFromList(index)->robot;
}

RobotList::RobotList(Robot *robot){
	first=new RobotObject;
	last=first;
	first->next=first;
	first->prev=first; //point to itself
	first->robot=robot;
	length=1;
}

void RobotList::AddToList(Robot *robot){
//	LockList();
	//RobotObject *tmp=last;
	RobotObject *add=new RobotObject;
	last->next=add;
	add->prev=last;
	add->next=first;
	add->robot=robot;
	last=add;
	length++;
//	UnlockList();
}

RobotObject * const RobotList::GetFromList(unsigned int index){
	index++;
	if(index>length){return last;}
	RobotObject *current=first;
	index--;
	for(;index>0;index--){
		current=current->next;
	}
	return current;
}
/*
RobotObject *const RobotList::FindRobot(Robot *robot){
	unsigned int i;
	RobotObject *tmp;
	for(i=0;i<length;i++){
		tmp=GetFromList(i);
		if(tmp->robot==robot){return *tmp;}
	}
	return NULL; //wasn't found
}
*/
unsigned int const RobotList::FindRobot(Robot *robot){
	unsigned int i;
	RobotObject *tmp;
	for(i=0;i<length;i++){
		tmp=GetFromList(i);
		if(tmp->robot==robot){return i;}
	}
	return 0; //wasn't found
}

void RobotList::RemoveFromList(Robot *robot){
	unsigned int tmp=FindRobot(robot);
	RemoveFromList(tmp);
}


void RobotList::RemoveFromList(unsigned int index){
	RobotObject *tmp=GetFromList(index);
	RobotObject *tmp2=tmp->prev;
	if(length==1){return;} //can't remove!!
	if(tmp==last){
		tmp2->next=first; //just set the previous pointer to first for rollover
		last=tmp2;
	}else{
		if(tmp==first){
			tmp2->prev=tmp2;
			tmp2->next=tmp->next;
			first=tmp2;
		}else{
			tmp2->next=tmp->next; //patch the links
			tmp->next->prev=tmp2;
		}
	}
	length--;
	delete tmp;
}



















/**Screen**/
void Screen::Render(void){
	SDL_UpdateRect((SDL_Surface*)this->screen,0,0,screen->w,screen->h);
}

Screen::Screen(){
}

void Screen::Point(int x,int y, unsigned int color){
	unsigned int offset;
	char *buffer=(char*)vbuffer;
	offset=x+(screen->w*y);
	buffer[offset]=(unsigned char)color;
}
unsigned char Screen::GetPoint(int x,int y){
	unsigned int offset;
	char *buffer=(char*)vbuffer;
	offset=x+(screen->w*y);
	cout << "getpoint" << endl;
	return (unsigned char)buffer[offset];
}

void *Screen::RowPtr(int start_x,int y){
	unsigned int offset;
	char *buffer=(char*)vbuffer;
	offset=start_x+(screen->w*y);
	return &buffer[offset];
}

void Screen::FlushBuffer(){
	unsigned int aoffset;
	unsigned int offset;
	int x,y;
	char *abuffer=(char*)screen->pixels;
	char *buffer=(char*)vbuffer;
	for(y=0;y<screen->h;y++){
		aoffset=screen->pitch*y; //the actual offset in memory includes the pitch
		offset=screen->w*y; //our buffered offset is just width
		abuffer=&abuffer[aoffset];
		buffer=&buffer[offset]; //yay! figured out how the crap to add an offset to a pointer!!
		for(x=0;x<screen->w;x++){
			*abuffer=*buffer;
			buffer++;
			abuffer++;
		}
		abuffer=(char*)screen->pixels;
		buffer=(char*)vbuffer;

	}

}



void Screen::Clear(unsigned int color){
	int x;
	int y;
	for(y=0;y<=screen->h;y++){
		unsigned char *row=(unsigned char*)RowPtr(0,y);
		for(x=0;x<screen->w;x++){
			*row=(unsigned char) color;
			row++;
		}
	}
}

void Screen::Clear(){
	Clear(FREESPACE_COLOR); //clear to white
}








/***Robot***/

//inline const unsigned short GetCurrentWord()

unsigned short const Robot::GetGene(unsigned int number){ //grr...can't ^%^$# make it inline because of gay GCC/LD..
	if(number>=GENE_SIZE){LogiCode::Error(GENE_OVERRUN);}
	return genes[number];
}


const unsigned char Robot::GetFrontPixel(){
	switch(rotation){
		case RIGHT:
		return display.GetPoint(posx+1,posy);
		break;
		case LEFT:
		return display.GetPoint(posx-1,posy);
		break;
		case UP:
		return display.GetPoint(posx,posy-1);
		break;
		case DOWN:
		return display.GetPoint(posx,posy+1);
		break;
	}
	return 0;
}

const unsigned char Robot::GetBackPixel(){
	switch(rotation){
		case RIGHT:
		return display.GetPoint(posx-1,posy);
		break;
		case LEFT:
		return display.GetPoint(posx+1,posy);
		break;
		case UP:
		return display.GetPoint(posx,posy+1);
		break;
		case DOWN:
		return display.GetPoint(posx,posy-1);
		break;
	}
	return 0;
}


Robot::Robot(int init_x,int init_y,const unsigned char *brain): attack_power(5), life(MAX_LIFE) {
	//cerr <<"t";
	//for(;;){}
	if_level=0;
	memcpy(brain_code,brain,BRAIN_SIZE); //copy brain code
	if(display.GetPoint(init_x,init_y)!=FREESPACE_COLOR){
		/**Means that the requested coordinates are actually filled.**/
		cout << "Error: Requested coordinates for robot not valid!" << endl;
		exit(1);
	}
	ip=0;
	posx=init_x;
	posy=init_y;
	display.Point(posx,posy,NORMAL_ROBOT_COLOR);
	rotation=UP;
	hp=HP_INIT;
	//have to setup matin gcode too!
}

Robot::~Robot(){
}


unsigned int Robot::ChangeHP(signed int change){
	hp+=change;
	if(hp>HP_LIMIT){hp=HP_LIMIT;}
	if(hp<=0){hp=0;LogiCode::Error(I_DIED);}
	return hp;
}

void Robot::Kill(){
	//to be continued..lol
}

int Robot::Move(signed int amount){ //I better be happy this has signing..it's a piece of crap to deal with!
	if(amount>1){
		for(;amount!=0;amount--){
			if(Move(1)==0){return 0;}
		}
	}
	if(amount<-1){
		for(;amount!=0;amount++){
			if(Move(-1)==0){return 0;} //recursive functions are so much fun..
		}
	}
	if(amount==0){return 0;}

	if(amount==1){
		if(GetFrontPixel()!=FREESPACE_COLOR){
			return 0;
		}
	}
	if(amount==-1){
		if(GetBackPixel()!=FREESPACE_COLOR){
			return 0;
		}
	}


	switch(rotation){ //forgot that computer graphs and actual graphs are inverted..
		//lol..forgot that since it's signed, you plus it and it works right...
		case UP:
		SetPos(posx,posy-amount);
		//posy+=amount;
		break;
		case DOWN:
		SetPos(posx,posy+amount);
		//posy-=amount;
		break;
		case RIGHT:
		SetPos(posx+amount,posy);
		//posx+=amount;
		break;
		case LEFT:
		SetPos(posx-amount,posy);
		//posx-=amount;
		break;
	}
	return 1;

}


void Robot::SetPos(int x,int y){ //this will do stuff for us...prevent constant code rewriting..
	display.Point(posx,posy,FREESPACE_COLOR); //erase the robot color
	posx=x;
	posy=y;
	display.Point(posx,posy,NORMAL_ROBOT_COLOR); //draw it at new location
}



void Robot::Rotate(bool way){
	if(way==CLOCKWISE){
		rotation++;
	}else{
		rotation--;
	}
	cout <<"rotation:"<<(int)rotation<<endl;
	/**We use binary round off for this to ensure it's valid..(it is unsigned char:2)**/
}

void Robot::EndTurn(){
	ChangeHP(-(attack_power/2)+1);
	turn=0;
}













void *PixelPointers;




/**Ok, now to keep track of which robot is at which point, easily and fast, we will
keep an array of pointers that are directly relational to the actual displayed pixels
the pointers are NULL if the space is empty, or else tell the current object at the point,
be it food, or a robot, or a dead robot..


**/














//Globals..
#ifdef _WIN32
/*The Mingw compiler doesn't use the "proper" memory allocation method for windows...it makes bugs with threading*/
void * operator new(size_t size){
	//std::cout << "size: " << (int)size << std::endl;
	void *tmp=HeapAlloc(GetProcessHeap(),0,size);
	if(tmp==NULL){std::cout << "NULL" <<endl;}
	//std::cout << "operator new" << std::endl;
	return tmp;
}

void operator delete(void *ptr){

	HeapFree(GetProcessHeap(),0,ptr);
	return;
}
#endif







int Thread_Messaging(void * arg){
	display.vbuffer=new char[640*480*1+1]; /**THE FIX!!!--INITIALIZE THIS _BEFORE_ SDL!! --this also fixed the exiting problem!**/
	if( SDL_Init(SDL_INIT_VIDEO) == -1 ){exit(1);}

	display.screen=SDL_SetVideoMode(640,480,8,SDL_HWSURFACE);
	if(display.screen==NULL){
		cout << "Error Setting Video Mode" << endl;
		exit(2);
	}

	//SDL_Delay(10000);

    volatile unsigned int quit=0;
	while(quit==0){
		display.FlushBuffer();
		display.Render();
		if(SDL_PollEvent(&event)==1){
			switch(event.type){
				case SDL_QUIT:
				quit=1;
				quit_all=1;
				exit(0);
				break;
			}
		}else{
			SDL_Delay(50); //don't waste all the processing time..
		}
	}
	SDL_Quit();
	quit_all=1;

	//SDL_QuitSubSystem(SDL_INIT_VIDEO);
	//while(quit_all==1){}

	//exit(0);
	//while(1){}
	return 0;

}

int Thread_DiskCache(void *arg){

	for(;;){}
	return 0;

}


//just a helper funciton...
void DrawBorder(){
	int i;
	for(i=0;i<display.screen->w;i++){
		display.Point(i,0,WALL_COLOR); //draw the top border
	}
	for(i=0;i<display.screen->h;i++){
		display.Point(0,i,WALL_COLOR); //draw the left border
	}
	for(i=0;i<display.screen->w;i++){
		display.Point(i,display.screen->h-1,WALL_COLOR); //draw bottom border;
	}
	for(i=0;i<display.screen->h;i++){
		display.Point(display.screen->w-1,i,WALL_COLOR);
	}




}



int DoRobots(void *trash){
	cout << "Waiting for SDL...";
    SDL_Delay(500);
    cout << endl <<"done!";
	display.Clear();
	DrawBorder();
	LogiCode::DoRobot();



	return 0;
}
/*
extern "C" int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
){
	cout << "HI!" << endl;


}
*/
#ifdef _WIN32 //This will overcome the bugs with MingW and GCC
int main(int argc,char**v){return 0xbad;}

//int SDL_main(int t,char **v){}

extern "C" int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
#else
int main(int argc,char **argv)
#endif
{
    // initialize SDL video

	//fprintf(stderr,"y");
	if ( SDL_Init( SDL_INIT_EVENTTHREAD ) < 0 )
    {
       cout<< "Unable to init SDL: " << SDL_GetError() << endl;
        exit(1);
    }

   // if( SDL_Init(SDL_INIT_VIDEO) == -1 ){exit(1);}
    // make sure SDL cleans up before exit
    atexit(SDL_Quit);
    // create a new window

    //messaging=SDL_CreateThread(Thread_Messaging,0);
    //disk_cache=SDL_CreateThread(Thread_DiskCache,0);
    //fprintf(stderr,"t");
    /*
	display.vbuffer=new char[640*480*1+1]; **THE FIX!!!--INITIALIZE THIS _BEFORE_ SDL!! --this also fixed the exiting problem!**
	if( SDL_Init(SDL_INIT_VIDEO) == -1 ){exit(1);}

	display.screen=SDL_SetVideoMode(640,480,8,SDL_HWSURFACE);
	if(display.screen==NULL){
		cout << "Error Setting Video Mode" << endl;
		exit(2);
	}
	*/
    robots_thread=SDL_CreateThread(DoRobots,0);
    Thread_Messaging(NULL);

	//PixelPointers=new void*[display.screen->w*display.screen->h*1];

    //DoRobots();

    // all is well ;)
    while(quit_all==0){
    	SDL_Delay(500); //fix extreme cpu usage
    	if(quit_all==1){
			quit_all++;
			while(1){}
			//SDL_WaitThread(messaging, 0);
			SDL_Quit();
		}
	} //infinite loop...
	//while(1){}
    return 0;
}




int BinaryRound(int source,int low,int high){



	return 0;

}






