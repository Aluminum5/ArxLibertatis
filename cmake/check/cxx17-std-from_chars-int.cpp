
#include <cstdint>
#include <charconv>
#include <string_view>
#include <cstddef>

int main(int argc, const char * argv[]) {
	
	if(argc < 1) {
		return 0;
	}
	
	std::string_view string(argv[0]);
	
	std::int32_t result = 0;
	std::from_chars(string.data(), string.data() + string.length(), result);
	
	return static_cast<int>(result);
}
