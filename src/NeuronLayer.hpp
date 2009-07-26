/*Copyright 2009 Alex Graves

This file is part of rnn_lib.

rnn_lib is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

rnn_lib is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with rnn_lib.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_NeuronLayer_h  
#define _INCLUDED_NeuronLayer_h  

#include "Layer.hpp"


template <class F> struct NeuronLayer: public Layer
{
	NeuronLayer(const string& name, size_t numDims, size_t size):
		Layer(name, numDims, size, size)
	{
		init();
	}
	NeuronLayer(const string& name, const vector<int>& directions, size_t size):
		Layer(name, directions, size, size)
	{
		init();
	}
	~NeuronLayer(){}
	void init()
	{
		display(this->inputActivations, "inputActivations");
		display(this->outputActivations, "outputActivations");
		display(this->inputErrors, "inputErrors");
		display(this->outputErrors, "outputErrors");	
	}
	void feed_forward(const vector<int>& coords)
	{
		transform(this->inputActivations[coords], this->outputActivations[coords], F::fn);
	}
	void feed_back(const vector<int>& coords)
	{
		loop(TDDD t, zip(this->inputErrors[coords], this->outputActivations[coords], this->outputErrors[coords]))
		{
			t.get<0>() = F::deriv(t.get<1>()) * t.get<2>();
		}
	}
};

#endif
