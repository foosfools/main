/*
 * motorController.h
 *
 * Authors:
 *
 *
 *
 */

#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
//#include <file.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
//using namespace std;\



class motorController
{
public:
	int fd;
	/*
	 *  Constructor
	 */
	motorController();

	/*
	 *  Detructor
	 */
	~motorController();


	/**
	 *
	 */
	void rotateGoalie(int degree);

	/**
	 *
	 */
	void moveGoalie(unsigned int steps);

	/**
	 *
	 */
	void sendMessage(char*message);

};

#endif
