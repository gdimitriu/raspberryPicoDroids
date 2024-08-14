
#ifndef __MOVE_ENCODER_H__
#define __MOVE_ENCODER_H__

	extern bool humanCommand;
	
	/**
	 * move the platform in front/rear on moveData distance
	 * return:
	 * 		-1 if is block in front but not started to run,
	 * 		0 all ok
	 *		1 if had an object in front after start moving
	 */
	int moveWithDistance(float moveData);

	void rotateRightDegree(int nr);
	
	void rotateLeftDegree(int nr);
	
	void rotate90Right();
	
	void rotate90Left();
	
	/**
	 * clean the encoders
	 */
	void clearEncoders();
	
	/**
	 * callback
	 * @param gpio the gpio pin which trigger the callback
	 * @param events the event triggered 
	 */
	void gpio_callback(uint gpio, uint32_t events);	

	/**
	 * return the left engine counts
	 */ 
	unsigned long getLeftEngineCounts();
	
	/**
	 * return right engine counts
	 */
	unsigned long getRightEngineCounts();
	
	/**
	 * return the left engine distance
	 */ 
	float getLeftEngineDistance();
	
	/**
	 * return right engine counts
	 */
	float getRightEngineDistance();
#endif
