#include "AllPanels.h"
#include <string>
#include <unordered_map>


namespace NightSnack {

	inline void DrawPrefabsPanel(std::vector<std::string> const& names, std::unordered_map<std::string, std::string> const& materials) {

		for (std::string const& n : names) {
			std::string mat = materials.at(n);
		}
		
	}

}	// NightSnack
