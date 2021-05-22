#include "Agreement.h"

V2::Agreement::Agreement(std::string _first, std::string _second, std::string _type, const date& _start_date):
	type(std::move(_type)), first(std::move(_first)), second(std::move(_second)), start_date(_start_date) {}
