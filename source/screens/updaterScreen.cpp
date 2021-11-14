#include "download.hpp"
#include "exiting.hpp"
#include "sound.h"
#include "updaterScreen.hpp"

extern bool dspfirmfound;
extern bool updatingSelf;
extern bool updated3dsx;
extern bool exiting;
extern sound *sfx_select;
extern sound *sfx_wrong;

struct {
	int x;
	int y;
} buttons2[] = { { 220, 168},
};

std::array<bool, 8> updateAvailable = {
	false,
};

UpdaterScreen::UpdaterScreen() {
	/*this->checkUpdates();*/ // Check for updates.
}

void UpdaterScreen::Draw(void) const {
	GFX::DrawTop(true);

	Gui::ScreenDraw(Bottom);
	GFX::DrawSprite(sprites_BS_background_idx, 0, 0);
	Gui::DrawString(6, 5, 0.55, WHITE, "Updater menu");
	// Draw buttons
	for (int i = (int)(sizeof(buttons2)/sizeof(buttons2[0]))-1; i >= 0; i--) {
		if (menuSelection == i) {
			// Button is highlighted.
			GFX::DrawSprite(sprites_BS_2page_extra_small_button_idx, buttons2[i].x, buttons2[i].y);
		} else {
			// Button is not highlighted. Darken the texture.
			if (buttonShading) {
				GFX::DrawSpriteBlend(sprites_BS_2page_extra_small_button_idx, buttons2[i].x, buttons2[i].y, GRAY);
			} else {
				GFX::DrawSprite(sprites_BS_2page_extra_small_button_idx, buttons2[i].x, buttons2[i].y);
			}
		}
		// Draw a green dot if an update is availible
		if(updateAvailable[i]) {
			GFX::DrawSprite(sprites_dot_green_idx, buttons2[i].x+75, buttons2[i].y-6);
		}

		// Determine the text height.
		// NOTE: Button texture size is 132x34.
		const int h = 32;

		// Draw the title.
		int y = buttons2[i].y + ((40 - h) / 2);
		int x_from_width = buttons2[i].x + title_spacing[i];
		Gui::DrawString(x_from_width, y, 0.75, BLACK, button_titles2[i]);

		if(!(i%2)) {
			Gui::DrawString(6, y, 0.60, WHITE, row_titles2[i/2]);
		}
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
}


void UpdaterScreen::Logic(u32 hDown, u32 hHeld, touchPosition touch) {

	if (hDown & KEY_TOUCH) {
		buttonShading = false;
	}

	if (menuSelection > 8) menuSelection = 1;
	else if (menuSelection > 7) menuSelection = 0;
	else if (menuSelection < -1) menuSelection = 6;
	else if (menuSelection < 0) menuSelection = 7;

	if (hDown & KEY_A) {
		setOption = true;
	}

	if (hDown & KEY_TOUCH) {
		for (int i = (int)(sizeof(buttons2)/sizeof(buttons2[0]))-1; i >= 0; i--) {
			if(updateAvailable[i]){
				if (touch.py >= (buttons2[i].y-6) && touch.py <= (buttons2[i].y+10) && touch.px >= (buttons2[i].x+75) && touch.px <= (buttons2[i].x+91)) {
					menuSelection = i;
					showMessage = true;
				}
			}
		}
		if(!showMessage) {
			for (int i = (int)(sizeof(buttons2)/sizeof(buttons2[0]))-1; i >= 0; i--) {
				if (touch.py >= buttons2[i].y && touch.py <= (buttons2[i].y+33) && touch.px >= buttons2[i].x && touch.px <= (buttons2[i].x+87)) {
					menuSelection = i;
					setOption = true;
				}
			}
		}
	}

	/*if (hDown & KEY_Y || showMessage) {
		switch (menuSelection)
		{
			case 0:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				showReleaseInfo("DS-Homebrew/TWiLightMenu", false);
				break;
			case 1:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				chooseCommit("TWLBot/Builds", "TWiLightMenu |", false);
				break;
			case 2:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				showReleaseInfo("ahezard/nds-bootstrap", false);
				break;
			case 3:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				chooseCommit("TWLBot/Builds", "nds-bootstrap |", false);
				break;
			case 4:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				showReleaseInfo("RocketRobz/TWiLightMenu-Updater", false);
				break;
			case 5:
				if(dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				chooseCommit("TWLBot/Builds", "TWiLightMenu-Updater |", false);
				break;
			default:
				if(dspfirmfound) {
					sfx_wrong->stop();
					sfx_wrong->play();
				}
				break;
		}
		showMessage = false;
	}*/

	if (setOption) {
		if(checkWifiStatus()) {
			std::string commit;
			switch (menuSelection) {
				case 0:	// Extras
					if(dspfirmfound) {
						sfx_select->stop();
						sfx_select->play();
					}
					downloadExtras();
					break;
			default:
					if(dspfirmfound) {
						sfx_wrong->stop();
						sfx_wrong->play();
					}
					break;
			}
		} else {
			if(dspfirmfound) {
				sfx_wrong->stop();
				sfx_wrong->play();
			}
			notConnectedMsg();
		}
		setOption = false;
	}
	if(hDown & KEY_START || updated3dsx) {
		exiting = true;
		fadecolor = 0;
		Gui::setScreen(std::make_unique<Exiting>(), true);
	}
}