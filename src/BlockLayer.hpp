/*Copyright 2009 Alex Graves

This file is part of RNNLIB.

RNNLIB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RNNLIB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RNNLIB.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_BlockLayer_h  
#define _INCLUDED_BlockLayer_h  

#include "Layer.hpp"

namespace rnnlib {

struct BlockLayer: public Layer
{
	//data
	vector<size_t> blockShape;
	vector<int> blockOffset;
	vector<int> inCoords;
	size_t sourceSize;
	CVI blockIterator;
	vector<size_t> outSeqShape;
		
	//functions
	BlockLayer(Layer* src, const vector<size_t>& blockshape, WeightContainer* wc, DataExportHandler* dEH):
		Layer(src->name + "_block", src->num_seq_dims(), 0, product(blockshape) * src->output_size(), wc, dEH, src),
		blockShape(blockshape),
		blockOffset(this->num_seq_dims()),
		inCoords(this->num_seq_dims()),
		sourceSize(src->outputActivations.depth),
		blockIterator(blockShape),
		outSeqShape(this->num_seq_dims())
	{
		assert(blockShape.size() == this->num_seq_dims());
		assert(!in(blockShape, 0));
		weightContainer->link_layers(this->source->name, this->name, this->source->name + "_to_" + this->name);
		display(this->outputActivations, "activations");
		display(this->outputErrors, "errors");
	}
	virtual void print(ostream& out = cout) const
	{
		Layer::print(out);
		out << " block " << blockShape;
	}
	virtual void start_sequence()
	{	
		for (int i = 0; i < outSeqShape.size(); ++i)
		{
			outSeqShape.at(i) = ceil((double)this->source->output_seq_shape().at(i) / (double)blockShape.at(i));
		}
		outputActivations.reshape(outSeqShape, 0);
		outputErrors.reshape(outSeqShape, 0);
	} 
	virtual void feed_forward(const vector<int>& outCoords)
	{
		double* outIt = this->outputActivations[outCoords].begin();
		range_multiply(blockOffset, outCoords, blockShape);
		for (blockIterator.begin(); !blockIterator.end; ++blockIterator)
		{
			range_plus(inCoords, *blockIterator, blockOffset);
			View<double> inActs = this->source->outputActivations.at(inCoords);
			if (inActs.begin())
			{
				std::copy(inActs.begin(), inActs.end(), outIt);
			}
			else
			{
				std::fill(outIt, outIt + sourceSize, 0);
			}
			outIt += sourceSize;
		}
	}
	virtual void feed_back(const vector<int>& outCoords)
	{
		const double* outIt = this->outputErrors[outCoords].begin();
		range_multiply(blockOffset, outCoords, blockShape);
		for (blockIterator.begin(); !blockIterator.end; ++blockIterator)
		{
			range_plus(inCoords, *blockIterator, blockOffset);
			double* inErr = this->source->outputErrors.at(inCoords).begin();
			if (inErr)
			{
				transform(outIt, outIt + sourceSize, inErr, inErr, plus<double>());
			}
			outIt += sourceSize;
		}
	}
};

};

#endif

