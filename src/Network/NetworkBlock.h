/*
 * Block.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Darius Malysiak
 */

#ifndef SHNET_NETWORKBLOCK_H_
#define SHNET_NETWORKBLOCK_H_

#include <Core/DataStructures/Block.h>

namespace Lazarus {



/**
 * An async source will produce continuously produce outputs in a separate thread.
 * The network source block will wait for connections from network drains.
 * There will be only drain->source pairs, i.e. no multiple connections to a single source.
 * */
template<typename OUTPUT_TYPE>
class NetworkSourceBlockAsync : public Lazarus::SourceBlock<OUTPUT_TYPE>, Lazarus::Thread {
public:
	NetworkSourceBlockAsync(unsigned int output_count) : Lazarus::SourceBlock<OUTPUT_TYPE>(0,output_count)
	{

	}

	virtual ~NetworkSourceBlockAsync()
	{
	}


	virtual void processInput(const void* in, unsigned int index){};

	void run()
	{

		//create the output
		for(unsigned int i = 0;i<this->m_output_count;i++)
		{
			OUTPUT_TYPE* output = (OUTPUT_TYPE*)(this->m_block_output[i]);

			generateOutput(output,i);
		}

		//trigger the block chain
		this->selfTrigger();
	}

	bool process_()
	{
		return true;
	}

};

/**
 * A source will produce continuously produce outputs with each call to start.
 * 'start' will block until the complete chain was executed.
 * */
template<typename INPUT_TYPE>
class NetworkDrainBlock : public Lazarus::DrainBlock<INPUT_TYPE> {
public:
	NetworkDrainBlock(unsigned int input_count) : Block<INPUT_TYPE,void>(input_count,0)
	{
	}

	virtual ~NetworkDrainBlock()
	{
	}

	virtual void processOutput(const void* out, unsigned int index){};

	bool process_()
	{
		//process the input
		for(unsigned int i = 0;i<this->m_input_count;i++)
		{
			INPUT_TYPE* input = getInput(i);

			processInput(input,i);
		}

		return true;
	}

};


} /* namespace TKSKBTracker */

#endif /* BLOCKS_BLOCK_H_ */
