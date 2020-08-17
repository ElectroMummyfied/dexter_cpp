#include "Tensor.hxx"

#pragma region [con/de]structures //{
	dxtr::Tensor::Tensor() : m_default_val(0), m_dummy_val(0), m_dims(1, 1) {
		m_data.insert(std::pair<uint, dat>(0, m_default_val));
	};
	/*Field(const Field& field) : m_rank(field.m_rank), m_default_val(field.m_default_val), m_dummy_val(field.m_dummy_val), m_dims(field.m_dims), m_strides(field.m_strides), m_data(field.m_data) {

	}*/
	dxtr::Tensor::Tensor(const Tensor& field) = default;
	dxtr::Tensor::Tensor(const uvec& dims) : m_default_val(0), m_dummy_val(0), m_dims(dims) {
		set_strides();
	};
	dxtr::Tensor::Tensor(const uvec& dims, const udmap& data) : Tensor(dims) {
		m_data = data;
	};
#pragma endregion //} [con/de]structures

#pragma region set_strides //{
	void dxtr::Tensor::set_strides() {
		uvec temp = m_dims;
		m_strides.assign(m_dims.size(), 1);
		for (uint it = 0; it < temp.size() - 1; it++) {
			std::rotate(temp.rbegin(), temp.rbegin() + 1, temp.rend());
			*temp.begin() = 1;
			std::transform(m_strides.begin(), m_strides.end(), temp.begin(), m_strides.begin(), std::multiplies<uint>());
		}
	}
#pragma endregion //} set_strides
#pragma region set_default_val //{ 
	void dxtr::Tensor::set_default_val(const dat& val) {
		m_default_val = val;
	};
#pragma endregion //} set_default_val
#pragma region resize //{
	/*void resize(const uvec&& dims) {
		resize(dims);
	}*/
	void dxtr::Tensor::resize(const uvec& dims) {
		if(dims.size() != m_dims.size()) {
			throw std::length_error("rank does not have the same size.");
		}
		m_dims = dims;
	}
#pragma endregion //} resize
#pragma region rank //{
	dxtr::ull dxtr::Tensor::rank() const {
		return m_dims.size();
	}
	void dxtr::Tensor::rank(const ull& rank) {
		m_dims.resize(rank);
	}
#pragma endregion rank //}

#pragma region n_elem //{
	dxtr::ull dxtr::Tensor::n_elem() const {
		return std::accumulate(m_dims.begin(), m_dims.end(), static_cast<uint>(1), std::multiplies<uint>());
	}
#pragma endregion //} n_elem
#pragma 
bool dxtr::Tensor::insertion_enabled() {
	return m_allow_insertion;
}
void set_insertion_state(bool val) {
	m_allow_insertion = val;
}
uvec dxtr::Tensor::addr(uint key) const {
	if ((key / (*m_strides.rbegin())) >= (*m_dims.rbegin())) {
		throw std::length_error("key exceeds max number of elements.");
	}
	uvec addr(m_strides.size());
	uvec::reverse_iterator addr_it;
	addr_it = addr.rbegin();
	for (uvec::const_reverse_iterator it = m_strides.rbegin(); it != m_strides.rend(); it++) {
		*addr_it = key / (*it);
		key %= (*it);

		addr_it++;
	}

	return addr;
}

dat& dxtr::Tensor::at(const uint& key) {
	uvec ad = addr(key);
	return at(ad);
}
dat& dxtr::Tensor::at(const uvec& ad) {
	if (ad.size() != m_dims.size()) {
		throw std::length_error("address size does not match tensor dimesional size.");
	}

	if (!std::equal(ad.begin(), ad.end(), m_dims.begin(), std::less<uint>())) {
		throw std::length_error("invalid address.");
	}

	uint key;
	udmap::iterator it;
	bool inserted = false;
	key = std::inner_product(m_strides.begin(), m_strides.end(), ad.begin(), static_cast<uint>(0));


	it = m_data.find(key);
	if (it == m_data.end()) {
		if (m_allow_insertion) {
			std::tie(it, inserted) = m_data.insert(std::pair<uint, dat>(key, m_default_val));
			if (!inserted) {
				throw std::runtime_error("failed to insert element.");
			}
		}
		else {
			return m_dummy_val;
		}
	}

	return it->second;
};

// work in progress
void dxtr::for_each_addr(const Tensor& field, void(*callback)(ullvec&)) {
	ull count = 0;
	ullvec ad1, ad2;
	Tensor::udmap::const_iterator ref;

	for (ull it1 = 1; it1 < field.n_elem(); it1++) {
		ad1 = field.addr(it1 - 1);
		ad2 = field.addr(it1);
	}
}
Field dxtr::transpose(const Tensor& field, ull it1 = 0, ull it2 = 1) {
	ullvec ad, adt;
	Tensor::dat val1, val2;
	Tensor T;
	Tensor::udmap::const_iterator ref;
	bool insertion_state = T.insertion_enabled();

	if (field.rank() == 1) {
		T.resize({{1, field.m_dims.at(0)}});
		goto exit;
	}

	if (it1 >= field.rank() || it2 >= field.rank()) {
		std::cerr << "invalid input arguments.\n";
		goto exit;
	}
	T = field;

	T.set_insertion_state(false);
	for (ull it = 0; it < T.n_elem(); it++) {
		ad = T.addr(it);
		if (ad.at(it2) > ad.at(it1)) {
			adt = ad;
			adt.at(it1) = ad.at(it2);
			adt.at(it2) = ad.at(it1);

			val1 = T.at(ad);
			val2 = T.at(adt);

			if (val1 != T.m_default_val) {
				T.at(adt) = val1;
			}
			if (val2 != T.m_default_val) {
				T.at(ad) = val2;
			}
		}
	}
	T.set_insertion_state(insertion_state);

	exit:
		return T;
}
void dxtr::print(const Tensor& obj) {
	Tensor field = transpose(obj);
	Tensor::uint count = 0;
	Tensor::uvec ad1, ad2;
	Tensor::udmap::const_iterator ref;

	ref = field.m_data.find(0);
	//std::cout << " [" << 0 << "] " << (ref != m_data.end() ? ref->second : m_default_val);
	std::cout << (ref != field.m_data.end() ? ref->second : field.m_default_val);
	for (Tensor::uint it1 = 1; it1 < field.n_elem(); it1++) {
		ad1 = field.addr(it1 - 1);
		ad2 = field.addr(it1);

		for (Tensor::uint it2 = 0; it2 < ad1.size(); it2++) {
			if (ad2.at(it2) - (ad1.at(it2)) == 1) {
				count = it2;
				break;
			}
		}

		if (count == 0) {
			std::cout << "\t";
		}
		else {
			for (Tensor::uint it3 = 0; it3 < count; it3++) {
				std::cout << "\n";
			}
		}
		//::print(ad2);
		ref = field.m_data.find(it1);
		//std::cout << " [" << it1 << "] " << (ref != m_data.end() ? ref->second : m_default_val);
		std::cout << (ref != field.m_data.end() ? ref->second : field.m_default_val);
	}
	std::cout << std::endl;
}

//void dxtr::print(const std::vector<std::size_t>& vec) {
//	std::cout << "(";
//	for (std::size_t it = 0; it < vec.size() - 1; it++) {
//		std::cout << vec.at(it) << ", ";
//	}
//	std::cout << *vec.rbegin() << ")";
//}