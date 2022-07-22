#include <tl/tl.hpp>

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <unordered_set>

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101043
// GCC does not like TL inside templates.
template <typename T>
std::unordered_set<T> intersection(std::unordered_set<T> lhs, std::unordered_set<T> rhs) {
	std::erase_if(lhs, [&] TL(!rhs.contains(_1)));
	return lhs;
}

int main() 
{
	assert(intersection<int>({1, 2, 3, 4, 5}, {2, 3, 4, 5, 6}) == std::unordered_set<int>({2, 3, 4, 5}));
}

