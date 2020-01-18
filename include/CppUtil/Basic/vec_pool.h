#ifndef _BASIC_HEADER_VECPOOL_H_
#define _BASIC_HEADER_VECPOOL_H_

#include <vector>
#include <unordered_set>

#include <assert.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class vec_pool {
		public:
			template<typename ... Args>
			size_t request(Args && ... args) {
				if (emptyIndices.empty()) {
					buffer.emplace_back(std::forward<Args>(args)...);
					return buffer.size() - 1; // >= 0
				}

				auto iter = emptyIndices.begin();
				size_t idx = *iter;
				emptyIndices.erase(idx);

				buffer[idx] = T(std::forward<Args>(args)...);
				return idx;
			}

			bool recycle(size_t idx) {
				if (idx >= buffer.size())
					return false;
				if (emptyIndices.find(idx) != emptyIndices.end())
					return false;
				emptyIndices.insert(idx);
				return true;
			}

			void reserve(size_t n) {
				buffer.reserve(n);
				emptyIndices.reserve(n);
			}

			void clear() {
				buffer.clear();
				emptyIndices.clear();
			}

			T& at(size_t n) {
				assert(n < buffer.size() && emptyIndices.find(n) == emptyIndices.end());
				return buffer[n];
			}
			const T& at(size_t n) const{ const_cast<vec_pool*>(this)->at(n); }
			T& operator[](size_t n) { return at(n); }
			const T& operator[](size_t n) const { return at(n); }

		private:
			std::vector<T> buffer;
			std::unordered_set<size_t> emptyIndices;
		};
	}
}

#endif // !_BASIC_HEADER_VECPOOL_H_