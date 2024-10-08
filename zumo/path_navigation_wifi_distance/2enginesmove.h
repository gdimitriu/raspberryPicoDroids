/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * 2enginesmove.h (actual driving the engines)
 * 
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of raspberryPicoDroids project.

 * raspberryPicoDroids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * raspberryPicoDroids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with raspberryPicoDroids; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#ifndef _2ENGINESMOVE_H_
#define _2ENGINESMOVE_H_

	/**
	 * break the engines
	 */
	void breakEngines();
	
	/**
	 * move with different speed on each engine
	 * @param speedLeft left engine power
	 * @param sppedRight right engine power
	 */
	void go(int speedLeft, int speedRight);
	
	/**
	 * stop the left engine
	 */
	void stopLeftEngine();
	
	/**
	 * stop the right engine
	 */
	void stopRightEngine();
#endif //_2ENGINES_MOVE_H_
