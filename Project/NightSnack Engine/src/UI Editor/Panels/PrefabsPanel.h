#include "AllPanels.h"
#include <string>


namespace NightSnack {

	inline void DrawPrefabsPanel(std::vector<std::string> const& names, std::unordered_map<std::string, std::string> materials) {

		for (std::string const& n : names) {
			std::string mat = materials[n];
		}
		
	}

}	// NightSnack
