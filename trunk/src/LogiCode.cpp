/**Copyright Jordan Earls <http://Earlz.biz.tm>
All rights reserved.
See "copyright.txt" for information on the copyright of this project and source file
This file is part of the "Robot AI" project.
**/

#include <iostream>

#include "config.h"
#include "Robots_AI.h"

namespace LogiCode{
using namespace std;

RobotList *robots;
Robot *crobot; //the current robot
typedef void (*opcode)(void);
opcode opcode_map[256];
unsigned char size_map[256]; //these and the above are directly used with each other..
//we use the size_map to tell the length of each instruction..every instruction is a fixed length unlike other archs

RobotList *first_list;
RobotList *last_list;
unsigned int list_count;


void unknown(){
	uint32_t temp;
	uint32_t temp2;
	temp=crobot->GetIP();
	temp2=crobot->GetCurrentByte();
	throw(unknown_op_exp("unknown opcode",temp,temp2));
	cout <<"Unknown Opcode!" << endl;
	cout <<"Opcode IP: " << crobot->GetIP()<<endl;
	cout <<"Opcode Byte: 0x";
	printf("%x",crobot->GetCurrentByte()); //arg! stupid cout not able to convert ints to strings!
	cout <<endl;
	//SDL_Delay(1000);
	crobot->turn=0;

}

/**Instructions**/

void if_general(){

}


void move_forward(){
	cout <<"x";
	crobot->Move(1);
	crobot->EndTurn();
}
void move_backward(){
	crobot->Move(-1);
	crobot->EndTurn();
}

void rotate_cw(){
	crobot->Rotate(CLOCKWISE);
	crobot->EndTurn();
}

void rotate_ccw(){
	crobot->Rotate(CCLOCKWISE);
	crobot->EndTurn();
}

void sprint_forward(){
	crobot->IncIP(1);
	unsigned char amount=crobot->GetCurrentByte();
	if(amount>20){Error(BAD_ARGUMENT);}
	if(amount==0){return;}
	unsigned char tmp=crobot->GetAttack();
	cout << "amount: "<< (int)amount<<endl;
	crobot->Move(amount);
	crobot->ChangeHP(-(amount*(tmp+2)));
	crobot->EndTurn();
}

void sprint_backward(){
	crobot->IncIP(1);
	unsigned char amount=crobot->GetCurrentByte();
	if(amount>20){Error(BAD_ARGUMENT);}
	if(amount==0){return;}
	unsigned char tmp=crobot->GetAttack();
	crobot->Move(-1*(amount)); //the only difference..
	crobot->ChangeHP(-(amount*(tmp+2)));
	crobot->EndTurn();
}





/**IF opcodes**/
void else_skip(){
	crobot->IncIP(1); //get off of if instruction..
	unsigned int level=0;
	for(;;){
		if((crobot->GetCurrentByte()&IF_CODE)==IF_CODE){
			level++;
		}
		if(crobot->GetCurrentByte()==ENDIF_CODE){if(level==0){return;}level--;}
		//if(crobot->GetCurrentByte()==ELSE_CODE){if(level==0){crobot->IncIP(1);return;}} //we skip ove rhte else code, so that it doesn't skip over the "good" code
		crobot->IncIP(size_map[crobot->GetCurrentByte()]);
	}

}

void if_skip(){
	crobot->IncIP(1); //get off of if instruction..
	unsigned int level=0;
	for(;;){
		if((crobot->GetCurrentByte()&0xF0)==IF_CODE){
			cout <<"t";
			level++;
		}
		if(crobot->GetCurrentByte()==ENDIF_CODE){if(level==0){return;}level--;}
		if(crobot->GetCurrentByte()==ELSE_CODE){if(level==0){return;}} //we skip ove rhte else code, so that it doesn't skip over the "good" code
		crobot->IncIP(size_map[crobot->GetCurrentByte()]);
	}
}

void endif(){
	crobot->if_level--;
}
void else_op(){ //have to skip if above was done!
	else_skip();
}

void if_register_imm8(){ //if(register COMPARE immediate)
	crobot->IncIP(1);
	unsigned char compare=((crobot->GetCurrentByte())&0xF0)>>4;
	unsigned short cmp1=crobot->r[((crobot->GetCurrentByte())&0x0F)];
	crobot->IncIP(1);
	unsigned char cmp2=crobot->GetCurrentByte();
	crobot->if_level++;
	switch(compare){
		case EQUALS:
		if(cmp1==cmp2){

		}else{if_skip();}
		break;

		default:
		Error(BAD_ARGUMENT);
		break;
	}

}





/**Standard ASM opcodes(minimalistic)**/
void mov_r_r(){ //moves ry to rx with y and x being in the byte like x|y
	crobot->IncIP(1);
	unsigned char tmp1=crobot->GetCurrentByte();
	unsigned char tmp2=(tmp1&0x0F);
	tmp1=(tmp1&0xF0)>>4;
	crobot->r[tmp1]=crobot->r[tmp2];
}

void mov_r_imm8(){
	crobot->IncIP(1);
	unsigned char reg=(crobot->GetCurrentByte()&0x0F);
	crobot->IncIP(1);
	crobot->r[reg]=crobot->GetCurrentByte();
}

void xchg_r_r(){
	crobot->IncIP(1);
	unsigned char tmp1=crobot->GetCurrentByte();
	unsigned char tmp2=(tmp1&0x0F);
	tmp1=(tmp1&0xF0)>>4;
	unsigned char tmp3=crobot->r[tmp1];
	crobot->r[tmp1]=crobot->r[tmp2];
	crobot->r[tmp2]=tmp3;
}



/**Misc stuff**/
void get_gene_imm8_r(){ //this will get a gene of imm8 and put it in rx
	crobot->IncIP(1);
	unsigned int gene=crobot->GetCurrentByte();
	crobot->IncIP(1);
	unsigned char reg=crobot->GetCurrentByte()&0x0F; //freespace in upper nibble..
	crobot->r[reg]=(short)crobot->GetGene(gene);
}










//can you say code salvaging!? lol
void InstallOp(unsigned char num,opcode func,unsigned char size){
     opcode_map[num]=func;
     size_map[num]=size;
}

void UninstallOp(unsigned char num){ //actually just sets the opcode to unknown
     opcode_map[num]=unknown;
}



void InitOpcodes(){
	unsigned int i;
	for(i=0;i<=0xFF;i++){
		   InstallOp(i,unknown,1); //set all opcodes to unknown..
	}
	InstallOp(0x00,move_forward,1);
	InstallOp(0x01,move_backward,1);
	InstallOp(0x02,rotate_cw,1);
	InstallOp(0x03,rotate_ccw,1);
	InstallOp(0x04,sprint_forward,2);
	InstallOp(0x05,sprint_backward,2);

	InstallOp(0x10,mov_r_imm8,3);
	InstallOp(0x11,mov_r_r,2);

	InstallOp(IF_CODE|0,if_register_imm8,3);

	InstallOp(0xA0,get_gene_imm8_r,3);
	/*0xF0 is endif opcode*/

	InstallOp(ENDIF_CODE,endif,1);
	InstallOp(ELSE_CODE,else_op,1);






}




void InitLogiCode(){
	InitOpcodes();
}

unsigned char test_brain[512]={
	0x00, //move_forward
	0x02, //rotate CW
	0x01, //move backward
	0xFF //0xFF throws invalid opcode
};


void DoRobot(){
	InitLogiCode();
	cerr << "hi";
	crobot=new Robot(5,2,test_brain);
	robots=new RobotList(crobot);
	//crobot=new Robot(10,61,test_brain);
	//robots->AddToList(crobot);
	//robots->AddToList(new Robot(30,5,test_brain));
	//if(crobot==(*robots)[1]){cout << "pass" <<endl;}
	//robots->RemoveFromList(1);
	//InitRobotList(crobot);
	getc(stdin);
	unsigned int cbot=0;
	crobot=(*robots)[cbot];
	for(;;){
		crobot->turn=1;

		DoBrain(*crobot);
		crobot->IncIP(1);
		if(crobot->turn==0){
			cbot++;
			if(cbot>=robots->GetLen()){cout << "y" << endl; cbot=0;}
			cout << "Turn over?"<<endl;
			crobot=(*robots)[cbot];
		}

		getc(stdin); //this is just helpful for testing..
	}


	for(;;){} //hold control..




}


int DoBrain(Robot &target){
	unsigned int i;
	target.turn=1;
	for(i=0;i<=BRAIN_SIZE;i++){

		opcode_map[target.GetCurrentByte()]();
		cout <<"x";
		if(target.turn==0){return 0;}
		target.IncIP(1);
	}
	return 1;
}


int Error(unsigned int code){ //this should eventually be replaced with exceptions
	switch(code){
		case IP_OVERFLOW:
		throw ip_over_exp("IP Overflow error");
		break;
		default:
		cout << "Undefined Error"<<endl<<"Error Code:"<<code<<endl;
		for(;;){}
		break;


	}
	return 0;
}






};





