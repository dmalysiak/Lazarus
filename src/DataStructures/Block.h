/*
 * Block.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Darius Malysiak
 */

#ifndef Lazarus_BLOCK_H_
#define Lazarus_BLOCK_H_

#include <map>

#include <FastKTuple.h>
#include <Queue.h>
#include <../Threading/Semaphore.h>

namespace Lazarus {

/**
 *
 * Blocks must obey the following rules:
 * - A block must be connected to all 'output_count sources'.
 * - The actual output data is owned by the Block itself. The wrapper which propagates the data between blocks and the target blocks
 * shall never modify this input-data!
 * - Each block on the other hand is responsible to manage the output data.
 * - A source must trigger its processing by calling 'self trigger'.
 * - The data type carried by a block message corresponds to the interpretation of BLOCK_OUTPUT_TYPE.
 * */
class BlockBase : public Lazarus::BaseObject
{
public:

	BlockBase(unsigned int input_count, unsigned int output_count)
	{
		m_input_count = input_count;
		m_output_count = output_count;

		m_input_counter = 0;
		m_output_counter = 0;

		mp_next_blocks = new Lazarus::FastKTuple<BlockBase*>(output_count);
		mp_previous_blocks = new Lazarus::FastKTuple<BlockBase*>(input_count);
		mp_previous_block_output_indices = new Lazarus::FastKTuple<unsigned int>(input_count);

		m_block_output = Lazarus::FastKTuple<void*>(output_count);

		m_block_id = "UNNAMED BLOCK";

		m_data_input_count = 0;
	}

	virtual ~BlockBase()
	{
		delete mp_next_blocks;
		delete mp_previous_blocks;
	}

	unsigned int getInputCount()
	{
		return m_input_count;
	}

	void setBlockID(const std::string& name)
	{
		m_block_id = name;
	}

	/**
	 * Connects block b4's output no. 'b4_output_index' to b.
	 * */
	static void connectBlocks(BlockBase* b4, BlockBase* b, unsigned int b4_output_index)
	{
		b->registerInputBlock(b4,b4_output_index);
		b4->registerOutputBlock(b);
	}



	void* getBlockOutput(unsigned int i = 0)
	{
		return &(m_block_output[i]);
	}

	/**
	 * Register the given block as input source.
	 * One can not register more than the set number of inputs.
	 * */
	virtual void registerInputBlock(BlockBase* source_block, unsigned int output_index)
	{
		if(m_input_counter == m_input_count)
		{
			ERROR("all inputs have already been set");
			return;
		}

		(*mp_previous_blocks)[m_input_counter] = source_block;
		(*mp_previous_block_output_indices)[m_input_counter] = output_index;
		m_input_counter++;
	}

	/**
	 * Register the given block as output target.
	 * One can not register more than the set number of inputs.
	 * */
	virtual void registerOutputBlock(BlockBase* target_block)
	{
		if(m_output_counter == m_output_count)
		{
			ERROR("all outputs have already been set");
			return;
		}

		//update the priority lut
		m_priority_lut.insert(std::make_pair(target_block->getInputCount(), m_output_counter));


		(*mp_next_blocks)[m_output_counter] = target_block;
		m_output_counter++;
	}

	virtual void unlockBlock()
	{
		m_sem.incSem();
	}

	std::string getBlockID(){return m_block_id;}

private:

	//lookup table for next blocks in the propagate input (descending order)
	std::multimap<unsigned int, unsigned int, std::greater<unsigned int>> m_priority_lut;

	bool process()
	{
		bool res = process_();

		propagate();//inform the next blocks about new data, i.e. push the data

		waitForBlocks();

		//unlock the previous blocks
		unlockwaitingBlocks();

		m_data_input_count = 0;

		return res;
	}

	/**
	 * Inform the next blocks that the output is ready.
	 * */
	void propagate()
	{
		//for(unsigned int i=0;i<m_output_counter;++i)
		for(auto it = m_priority_lut.begin(); it != m_priority_lut.end(); ++it)
		{
			unsigned int i = it->second;

			if((*mp_next_blocks)[i] != NULL)
				(*mp_next_blocks)[i]->pushData();
		}
	}

protected:
	unsigned int m_input_count;
	unsigned int m_output_count;

	unsigned int m_input_counter;
	unsigned int m_output_counter;

	unsigned int m_data_input_count;
	Lazarus::Semaphore m_sem;

	std::string m_block_id;


	/**
	 * This will push the data to the registered outputs.
	 * Once all registered sources have pushed their data the processing will commence.
	 * */
	void pushData()
	{
		m_data_input_count++;

		if(m_data_input_count == m_input_count)
		{
			process();
		}

	}

	/**
	 * This method should be called by a source in order to trigger the
	 * chain propagation.
	 * */
	void selfTrigger()
	{
		process();
	}


	virtual void waitForBlocks()
	{
		for(unsigned int i=0;i<m_output_counter;++i)
			m_sem.decSem();//wait until the next blocks finish
	}

	virtual void unlockwaitingBlocks()
	{
		for(unsigned int i=0;i<m_input_counter;++i)
		{
			(*mp_previous_blocks)[i]->unlockBlock();
		}
	}


	/**
	 * this method shall process the current input and store the result in 'mp_output'.
	 * */
	virtual bool process_() = 0;

	Lazarus::FastKTuple<BlockBase*>* mp_next_blocks;
	Lazarus::FastKTuple<BlockBase*>* mp_previous_blocks;
	Lazarus::FastKTuple<unsigned int>* mp_previous_block_output_indices;

	Lazarus::FastKTuple<void*> m_block_output;


};


/**
 * This is a block message container, it will be used by e.g. the ControlServer.
 * */
class ServerMessageBase
{
public:

	ServerMessageBase()
	{
		mp_data = NULL;
		mp_sending_block = NULL;
		mp_sem = NULL;
	}

	ServerMessageBase(BlockBase* sender, void* data,Lazarus::Semaphore* mutex)
	{
		mp_data = data;
		mp_sending_block = sender;
		mp_sem = mutex;
	}

	BlockBase* mp_sending_block;
	void* mp_data;//some data
	Lazarus::Semaphore* mp_sem;
};

/**
 * The basic idea is that one can push server messages for processing into the
 * external (server-)queue while one can poll the incoming queue for any data from
 * e.g. the server.
 * */
template<typename INPUT_TYPE, typename OUTPUT_TYPE>
class Block : public BlockBase {
public:
	Block(unsigned int input_count, unsigned int output_count) : BlockBase(input_count,output_count)
	{
		for(unsigned int i=0;i<m_output_count;++i)
			m_block_output[i] = new OUTPUT_TYPE();


		mp_control_server_queue = NULL;

	}

	virtual ~Block()
	{
		for(unsigned int i=0;i<m_output_count;++i)
			delete (OUTPUT_TYPE*)(m_block_output[i]);
	}
	Lazarus::Queue<ServerMessageBase>* getIncomingQueue()
	{
		return &m_incoming_message_queue;
	}

	void registerControlServerQueue(Lazarus::Queue<ServerMessageBase>* server_queue)
	{
		mp_control_server_queue = server_queue;
	}

	/**
	 * Implement this method in order to process the input.
	 * */
	virtual void processInput(const INPUT_TYPE* in, unsigned int index) = 0;

	/**
	 * Implement this method in order to generate the i-th output.
	 * */
	virtual void generateOutput(OUTPUT_TYPE* out, unsigned int index) = 0;


	bool process_()
	{
		for(unsigned int i = 0;i<this->m_input_count;i++)
		{
			//get the input
			INPUT_TYPE* input = getInput(i);

			processInput(input,i);
		}

		//create the output
		for(unsigned int i = 0;i<this->m_output_count;i++)
		{
			OUTPUT_TYPE* output = (OUTPUT_TYPE*)(this->m_block_output[i]);

			generateOutput(output,i);
		}

		return true;
	}

protected:
	INPUT_TYPE m_blockinput_trait_helper;
	OUTPUT_TYPE m_blockoutput_trait_helper;

	Lazarus::Queue<ServerMessageBase> m_incoming_message_queue;//receive message here
	Lazarus::Queue<ServerMessageBase>* mp_control_server_queue;//send messages here

	INPUT_TYPE* getInput(unsigned int index)
	{
		unsigned int previous_block_output_index = (*(this->mp_previous_block_output_indices))[index];

		return (INPUT_TYPE*)((*(this->mp_previous_blocks))[index]->getBlockOutput( previous_block_output_index ) );
	}


	void sendServerMessage(void* data)
	{
		ServerMessageBase m;

		m.mp_sem = new Lazarus::Semaphore();
		m.mp_data = data;
		m.mp_sending_block = this;

		mp_control_server_queue->enqueue(m);
		m.mp_sem->decSem();//wait for the message being processed

		delete m.mp_sem;

	}


};



/**
 * An async source will produce continuously produce outputs in a separate thread.
 * */
template<typename OUTPUT_TYPE>
class SourceBlockAsync : public Block<void,OUTPUT_TYPE>, Thread {
public:
	SourceBlockAsync(unsigned int output_count) : Block<void,OUTPUT_TYPE>(0,output_count)
	{

	}

	virtual ~SourceBlockAsync()
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
template<typename OUTPUT_TYPE>
class SourceBlock : public Block<void,OUTPUT_TYPE> {
public:
	SourceBlock(unsigned int output_count) : Block<void,OUTPUT_TYPE>(0,output_count)
	{

	}

	virtual ~SourceBlock()
	{
	}


	virtual void processInput(const void* in, unsigned int index){};

	void start()
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
class DrainBlock : public Block<INPUT_TYPE,void> {
public:
	DrainBlock(unsigned int input_count) : Block<INPUT_TYPE,void>(input_count,0)
	{
	}

	virtual ~DrainBlock()
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
