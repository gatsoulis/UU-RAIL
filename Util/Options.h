#include <boost/program_options.hpp>
namespace boostoptions = boost::program_options;

/* Function used to check that option is specified */
inline void option_required(const boostoptions::variables_map& vm, const char* for_what)
{
    if (!vm.count(for_what) || vm[for_what].defaulted())
            throw std::logic_error(std::string("Option '") + for_what + "' required!");
}

/* Function used to check that one of options is specified */
inline void option_eclusive_or(const boostoptions::variables_map& vm, const char* for_what, const char* for_what2)
{
    if (!vm.count(for_what) || vm[for_what].defaulted()) {
    	if (!vm.count(for_what2) || vm[for_what2].defaulted()) {
            throw std::logic_error(std::string("Option '") + for_what + "' OR '" + for_what2 + "' must be given!");
    	}
    } else {
    	if (vm.count(for_what2) && !vm[for_what2].defaulted()) {
			throw std::logic_error(std::string("Option '") + for_what + "' and '" + for_what2 + "' must not be used together!");
		}
    }
}

/* Function to fill in a bool flag - is it there or is it not. */
inline void option_boolean(const boostoptions::variables_map& vm, const char* for_what, bool &variable) {
	if (vm.count(for_what)) {
		variable = true;
	}
	variable = false;
}
