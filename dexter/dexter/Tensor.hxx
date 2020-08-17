#pragma once

#include <map>
#include <vector>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace dxtr {
	typedef unsigned long long ull;
	typedef std::vector<ull> ullvec;

	class Tensor;

	void for_each_addr(const Tensor& field, void(*callback)(ullvec&));
	Tensor transpose(const Tensor&, ull, ull);
	void print(const Tensor&);
};

class dxtr::Tensor {
	typedef double dat;
	typedef std::map<ull, dat> udmap;

	public:
		

	private:
		dat m_default_val;
		dat m_dummy_val;
		std::vector<ull> m_dims;
		std::vector<ull> m_strides;

		udmap m_data;

		bool m_allow_insertion = false;
	private:
		void set_strides();

	public:
		Tensor();

		Tensor(const Tensor& field);
		Tensor(const ullvec& dims);
		Tensor(const ullvec& dims, const udmap& data);
		void set_default_val(const dat& val);
		void resize(const ullvec&& dims);
		void resize(const ullvec& dims);
		
		ull rank() const;
		void rank(const ull& rank);

		ull n_elem() const;
		bool insertion_enabled();
		void set_insertion_state(bool val);
		ullvec addr(ull key) const;

		dat& at(const ull& key);
		dat& at(const ullvec& ad);

		friend Tensor transpose(const Tensor&, ull, ull);
		friend void print(const Tensor&);
		friend int main(int argc, char* argv[]);
};
