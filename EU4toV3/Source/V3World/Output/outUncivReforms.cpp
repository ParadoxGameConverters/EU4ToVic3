#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const UncivReforms& uncivReforms)
{
	if (uncivReforms.reforms[0]) {
		output << "land_reform=yes_land_reform\n";
	}
	else
	{
		output << "land_reform=no_land_reform\n";
	}

	if (uncivReforms.reforms[1]) {
		output << "admin_reform=yes_admin_reform\n";
	}
	else
	{
		output << "admin_reform=no_admin_reform\n";
	}

	if (uncivReforms.reforms[3] && uncivReforms.reforms[2]) {
		output << "finance_reform=finance_reform_two\n";
	}
	else if (uncivReforms.reforms[2]) {
		output << "finance_reform=yes_finance_reform\n";
	}
	else
	{
		output << "finance_reform=no_finance_reform\n";
	}

	if (uncivReforms.reforms[4]) {
		output << "education_reform=yes_education_reform\n";
	}
	else
	{
		output << "education_reform=no_education_reform\n";
	}

	if (uncivReforms.reforms[5]) {
		output << "transport_improv=yes_transport_improv\n";
	}
	else
	{
		output << "transport_improv=no_transport_improv\n";
	}

	if (uncivReforms.reforms[6]) {
		output << "pre_indust=yes_pre_indust\n";
	}
	else
	{
		output << "pre_indust=no_pre_indust\n";
	}

	if (uncivReforms.reforms[7]) {
		output << "industrial_construction=yes_industrial_construction\n";
	}
	else
	{
		output << "industrial_construction=no_industrial_construction\n";
	}

	if (uncivReforms.reforms[8]) {
		output << "foreign_training=yes_foreign_training\n";
	}
	else
	{
		output << "foreign_training=no_foreign_training\n";
	}

	if (uncivReforms.reforms[9]) {
		output << "foreign_weapons=yes_foreign_weapons\n";
	}
	else
	{
		output << "foreign_weapons=no_foreign_weapons\n";
	}

	if (uncivReforms.reforms[10]) {
		output << "military_constructions=yes_military_constructions\n";
	}
	else
	{
		output << "military_constructions=no_military_constructions\n";
	}

	if (uncivReforms.reforms[11]) {
		output << "foreign_officers=yes_foreign_officers\n";
	}
	else
	{
		output << "foreign_officers=no_foreign_officers\n";
	}

	if (uncivReforms.reforms[12]) {
		output << "army_schools=yes_army_schools\n";
	}
	else
	{
		output << "army_schools=no_army_schools\n";
	}

	if (uncivReforms.reforms[13]) {
		output << "foreign_naval_officers=yes_foreign_naval_officers\n";
	}
	else
	{
		output << "foreign_naval_officers=no_foreign_naval_officers\n";
	}

	if (uncivReforms.reforms[14]) {
		output << "naval_schools=yes_naval_schools\n";
	}
	else
	{
		output << "naval_schools=no_naval_schools\n";
	}

	if (uncivReforms.reforms[14]) {
		output << "foreign_navies=yes_foreign_navies\n";
	}
	else
	{
		output << "foreign_navies=no_foreign_navies\n";
	}
	return output;
}
