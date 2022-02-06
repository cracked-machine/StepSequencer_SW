/*
 * mainapp.hpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#ifndef MAINAPP_HPP_
#define MAINAPP_HPP_

#if defined(X86_UNIT_TESTING_ONLY)
	// only used when unit testing on x86
    #include <mock_cmsis.hpp>
	#include <iostream>
#endif

#ifdef __cplusplus
	extern "C"
	{
	
#endif


	void mainapp();
	// void DMA1_Channel1_IRQHandler(void);

#ifdef __cplusplus
	}
#endif

#endif /* MAINAPP_HPP_ */
