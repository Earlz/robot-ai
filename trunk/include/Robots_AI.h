/**Copyright Jordan Earls <http://Earlz.biz.tm>
All rights reserved.
See "copyright.txt" for information on the copyright of this project and source file
This file is part of the "Robot AI" project.
**/

#ifndef ROBOTS_AI_H
#define ROBOTS_AI_H
#include "config.h"
#include <cstdlib>
#include <SDL.h>
#include <iostream>
#include <SDL_thread.h>
#ifdef _WIN32
 #include <windows.h>
#endif

#define render() SDL_UpdateRect(screen,0,0,swidth,sheight)

namespace LogiCode{ //stupid f compiler..can't declare the whole logicode namespace until robot, but can't declare it before because it relies on robot...&*#$ stupid..
int Error(unsigned int code);

};

/**Exceptions**/
class ip_over_exp{
	public:
	ip_over_exp(std::string message){msg=message;}
	std::string msg;
};

class unknown_op_exp{
	public:
	std::string msg;
	uint32_t opip;
	uint8_t opbyte;
	unknown_op_exp(std::string message,uint32_t opip_, uint8_t opbyte_){msg=message;opip=opip_;opbyte=opbyte_;}
};



class Screen{

	public:
	volatile SDL_Surface *screen;
	~Screen(){};
	void Render(void);
	Screen();
	void Point(int x,int y,unsigned int color);
	void Clear();
	void Clear(unsigned int color);
	unsigned char GetPoint(int x,int y);
	void *RowPtr(int start_x,int y);
	volatile void *vbuffer; //this is required to be thread-safe
	void FlushBuffer();

}; //grr..always forget the semicolon..



#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define CLOCKWISE true
#define CCLOCKWISE false

class Robot{
	unsigned char brain_code[BRAIN_SIZE]; //this is inherited from one parent of another usually...there are mutations where they both merge or this is randomly generated hwoever
	unsigned char ext_memory[EXT_MEM_SIZE];
	unsigned short genes[GENE_SIZE];  //these are a combined set of both parents
	unsigned char mating_code[4]; //there is the initial one,the negated one, then the plus and minus 0x44 one
	volatile int posx,posy;
	unsigned char hp;
	const unsigned char attack_power;
	unsigned char rotation:2;
	unsigned int ip; //this shouldn't be public to prevent buffer overflow
	const unsigned int life; //this is the maximum amount one robot can live(in turns)
	public:
	unsigned short const GetGene(unsigned int number);
	unsigned int if_level;
	inline unsigned int IncIP(signed int adder){ip=(signed)ip+adder; if(((signed)ip<0) || ((signed)ip>(signed)BRAIN_SIZE)){LogiCode::Error(IP_OVERFLOW);}return ip;}
	const inline unsigned int GetIP(){return ip;}
	inline unsigned int SetIP(unsigned int replace){ip=replace; if(ip>BRAIN_SIZE){LogiCode::Error(IP_OVERFLOW);}return ip;}
	volatile bool turn;
	unsigned short r[16]; //r0-r15 for registers..

	Robot(int init_x,int init_y,const unsigned char *brain); //initial robots and mutations
	Robot(int init_x, int init_y,const Robot &parent1,const Robot &parent2);
	~Robot();
	//void Create(int init_x, int init_y, unsigned char *brain, unsigned char mate_code);
	unsigned int ChangeHP(signed int amount); //this is relational!
	unsigned int const GetHP(){return hp;}
	//It is constructed to bring to life and destructed to kill?
	int Move(signed int amount); //returns actual amount moved(in case something is in the way)
	void Rotate(bool way); //way is one for CW and zero for CCW
	char const GetDirection(){return rotation;} //returns RIGHT, LEFT, or UP or DOWN...
	//void Kill(); //this kills the Robot. Because of the way robots are linked in a list, we can't destruct it..
	void EndTurn(); //This decrements the hp for each turn
	void Attack(Robot &victim);
	const unsigned char GetFrontPixel();
	const unsigned char GetBackPixel();
	void Kill(void); //?
	const unsigned char GetCurrentByte(){if(ip<BRAIN_SIZE){return brain_code[ip];}else{LogiCode::Error(IP_OVERFLOW);return 1;}}
	const unsigned char GetByte(unsigned int pos){if(pos<BRAIN_SIZE){return brain_code[pos];}else{LogiCode::Error(IP_OVERFLOW);}}
	inline const unsigned short GetCurrentWord();
	inline const unsigned short GetWord(int pos);
	void SetPos(int x,int y);
	void SetPos(int x,int y,unsigned int color);
	const inline unsigned char GetAttack(){return attack_power;}
	bool operator==( const Robot& other ) const{return 1;}



};
class RobotList;

//How the crap to make a list of robots!?
class RobotObject{
	public:
	RobotObject *next;
	RobotObject *prev;
	Robot *robot;
};


class RobotList{
	unsigned int length;
	RobotObject *first,*last;
	public:
	unsigned int GetLen(){return length;}

	RobotList(Robot *first);
	void AddToList(Robot *to_add);
	void RemoveFromList(Robot *pointer); //this only looks for the first pointer that matches..
	void RemoveFromList(unsigned int index); //this could actually be easily accomplished by using operator []
	RobotObject * const GetFromList(unsigned int index);
	//RobotObject *const FindRobot(Robot *robot);
	unsigned int const FindRobot(Robot *robot);
	//the icing on the cake that makes things simple
	Robot *operator [] (unsigned int index); //HERE
	//RobotObject *operator[](unsigned int index);
//	bool operator==(const Robot cmp2){return 1;}
};








#define DEAD_ROBOT 1
#define FOOD_PACKET 0

class Food{
	int posx,posy;
	int increase; //be funny seeing how if we set this negative! lol
	int type;
	public:
	int GetX(){return posx;}
	int GetY(){return posy;}
	bool SetPos(int x,int y); //returns false for error, true for good

	Food(int x,int y,int increase,int type);

};



extern SDL_Thread *messaging,*disk_cache,*robots_thread;
extern SDL_Event event;

extern Screen display;
extern volatile int quit_all;


/**LogiCode**/


namespace LogiCode{
int DoBrain(Robot &target);
void DoRobot();
int Error(unsigned int code);

};













#endif


