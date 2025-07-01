//
// Created by 24148 on 03/07/2020.
//

#ifndef LCTD_FLATTEN_HASH_MAP_H
#define LCTD_FLATTEN_HASH_MAP_H

#include <vector>
using namespace std;


namespace koala {
	// idx, dis , middleSet
	template<typename kType, typename vType, typename sType>
	class _iterator {
	
	};

	
	template<typename V, typename S>
	// template<typename S>
	class my_openadd_hashmap {
#define LoadfactorInverse 1.5
#define nullele (4294967295U)

	private:
		static inline int put(unsigned int key, V &v, S & s, std::vector<std::tuple<unsigned int, V, S>> &table) {
			unsigned int idx = key % table.size();
			while (get<0>(table[idx]) != nullele && get<0>(table[idx]) != key) {
				++idx;
				idx = idx == table.size() ? 0 : idx;
			}
			//key 已 valid 存在

			if (get<0>(table[idx]) == nullele) {
				table[idx] = std::make_tuple(key, v, s);
				return static_cast<int>(idx);
				//static_cast<type>(x) 把 idx 的类型强制转化成type类型 
			}
			// 是时候进行插入了

			return -1;
		}
		
		static inline int put(std::tuple<unsigned int, V, S> p, std::vector<std::tuple<unsigned int, V, S>> &table) {
			return put(get<0>(p), get<1>(p), get<2>(p), table);
		}
		
		inline void expand() {
			unsigned int _new_size = 0;
			if (hash_table.size() == 0)_new_size = 1;
			else _new_size = hash_table.size() * 2;
			std::vector<std::tuple<unsigned int, V, S>> newtable(_new_size, std::make_tuple(nullele, V(), S()));
			for (auto &a:hash_table) if (get<0>(a) != nullele)put(a, newtable);
			swap(newtable, hash_table);
		}
		
		std::vector<std::tuple<unsigned int, V, S>> hash_table;
		unsigned int _size;
	public:
		//构造函数：初始化自定义类
		my_openadd_hashmap() : hash_table(0, std::make_tuple(nullele, V(), S())), _size(0) {}
		
		_iterator<unsigned int, V, S> begin() {
			return _iterator<unsigned int, V, S>();
		}
		
		_iterator<unsigned int, V, S> end() {
			return _iterator<unsigned int, V, S>();
		}
		
		unsigned int iterator() {
			unsigned int p = 0;
			while (p < hash_table.size() && get<0>(hash_table[p]) == nullele) ++p;
			return p;
		}
		
		unsigned int &next(unsigned int &p) {
			++p;
			while (p < hash_table.size() && get<0>(hash_table[p]) == nullele) ++p;
			return p;
		}
		
		bool has_next(unsigned int p) {
			return p != hash_table.size();
		}
		
		// inline std::pair<V,S> operator[](unsigned int key) {
		// 	int idx = find(key);
		// 	if (idx == -1) {
		// 		idx = insert(key, V(), S());
		// 	}
		// 	return {get<1>(hash_table[static_cast<unsigned int>(idx)]), get<2>(hash_table[static_cast<unsigned int>(idx)])};
		// }

		//return mp
		inline S &operator[](unsigned int key) {
			int idx = find(key);
			if (idx == -1) {
				idx = insert(key, V(), S());
			}
			return get<2>(hash_table[static_cast<unsigned int>(idx)]);
		}

		inline S getMP(unsigned int key) {
			int idx = find(key);
			if (idx == -1) return S();
			
			return get<2>(hash_table[static_cast<unsigned int>(idx)]);
		}

		//return dist
		inline V &operator()(unsigned int key) {
			int idx = find(key);
			if (idx == -1) {
				idx = insert(key, V(), S());
			}
			return get<1>(hash_table[static_cast<unsigned int>(idx)]);
		}

		inline V getDist(unsigned int key) {
			int idx = find(key);
			if (idx == -1) return -1;
			
			return get<1>(hash_table[static_cast<unsigned int>(idx)]);
		}

		inline tuple<unsigned int, V, S> getDistMSet(unsigned int key) {
			int idx = find(key);
			if (idx == -1) {
				idx = insert(key, V(), S());
			}
			return hash_table[static_cast<unsigned int>(idx)];
			// return {get<1>(hash_table[static_cast<unsigned int>(idx)]), get<2>(hash_table[static_cast<unsigned int>(idx)])};
		}

		
		
		inline std::tuple<unsigned int, V, S> &get_with_idx(unsigned int i) { return hash_table[i];}
		

		inline void update(unsigned int key, V v, S s) {
			int idx = find(key);
			if (idx == -1) {
				idx = insert(key, V(), S());
			}
			get<1>(hash_table[static_cast<unsigned int>(idx)]) = v;
			get<2>(hash_table[static_cast<unsigned int>(idx)]) = s;
		}
		
		inline void update(std::tuple<unsigned int, V, S> kv) {
			update(get<0>(kv), get<1>(kv), get<2>(kv));
		}

	
		
		inline int insert(unsigned int key, V v, S s) {
			if ((_size + 1) * LoadfactorInverse > hash_table.size()) {
				expand();
			}
			int idx = put(key, v, s, hash_table);
			if (idx >= 0)_size++;
			return idx;
		}
		
		inline int insert(std::tuple<unsigned int, V, S> kv) {
			// return insert(kv.first, kv.second);
			return insert(get<0>(kv), get<1>(kv), get<2>(kv));
		}
		
		/*
		 * if found, return idx of inner data structure
		 * if not found, return -1
		 */
		inline int find(unsigned int key) {
			if (_size == 0) return -1;
			unsigned int idx = key % hash_table.size();
			unsigned int tmpi = (idx + hash_table.size() - 1) % hash_table.size();
			while (get<0>(hash_table[idx]) != nullele && get<0>(hash_table[idx]) != key && tmpi != idx) {
				++idx;
				idx = idx == hash_table.size() ? 0 : idx;
			}
			if (get<0>(hash_table[idx]) == key) {
				return static_cast<int>(idx);
			}
			return -1;
		}	
		
		inline void clear() {
			hash_table.clear(), hash_table.shrink_to_fit();
			//shrink_to_fit(): deque、vector 或 string 退回不需要的内存空间
			_size = 0;
		}
		
		inline std::vector<unsigned int> &release_data() {
			return hash_table;
		}
		
		inline unsigned int size() { return _size; }
	
	
	};
}
#endif //LCTD_FLATTEN_HASH_MAP_H
