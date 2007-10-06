/**Copyright Jordan Earls <http://Earlz.biz.tm>
All rights reserved.
See "copyright.txt" for information on the copyright of this project and source file
This file is part of the "Robot AI" project.
**/

#ifndef CONFIG_H
#define CONFIG_H
/**These change the behaviour of the program.**/
const unsigned int BRAIN_SIZE=512;
#define SPEED_UP
//#define LOW_MEMORY
const unsigned int MAX_ROBOTS=10000;
const unsigned int MIN_ROBOTS=5000;
const unsigned int HP_LIMIT=100;
const unsigned int HP_INIT=75;
const unsigned int MAX_LIFE=200;
const unsigned int EXT_MEM_SIZE=1024;
const unsigned int GENE_SIZE=128;
const unsigned int SPRINT_HP_DECREASE=2;


/**Colors for the enviroment**/
const unsigned char FREESPACE_COLOR=0x00;
const unsigned char WALL_COLOR=0x80;
const unsigned char NORMAL_ROBOT_COLOR=0xFF;



/**Constants**/ //these really don't make much difference if you change them..
const unsigned int IP_OVERFLOW=1;
const unsigned int BAD_ARGUMENT=2;
const unsigned int I_DIED=4;
const unsigned int GENE_OVERRUN=8;


/***LogiCode stuff***/

/**Comparison codes**/
const unsigned char EQUALS=1;
const unsigned char NOT_EQUALS=2;
const unsigned char GREATER=4; //greater and less can be OR'd with EQUALS
const unsigned char LESS=8;

/**misc. stuff**/
const unsigned char ENDIF_CODE=0xDF;
const unsigned char ELSE_CODE=0xDE;
const unsigned char IF_CODE=0x20; //this is only the base code..







#endif
